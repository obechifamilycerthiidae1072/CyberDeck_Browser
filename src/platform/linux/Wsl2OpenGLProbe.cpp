#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/gl.h>
#include <wayland-client.h>
#include <wayland-egl.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

namespace {

struct GlProbeResult {
    bool success = false;
    std::string backend;
    std::string error;
    int egl_major = 0;
    int egl_minor = 0;
    std::string vendor;
    std::string renderer;
    std::string version;
    std::string shading_language;
};

struct WaylandState {
    wl_compositor* compositor = nullptr;
};

std::string GlString(GLenum name) {
    const GLubyte* value = glGetString(name);
    return value == nullptr ? "unknown" : reinterpret_cast<const char*>(value);
}

bool Contains(std::string_view value, std::string_view needle) {
    return value.find(needle) != std::string_view::npos;
}

GlProbeResult Failure(std::string error) {
    GlProbeResult result;
    result.error = std::move(error);
    return result;
}

EGLDisplay GetWaylandEglDisplay(wl_display* wayland_display) {
    auto get_platform_display =
        reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));
    if (get_platform_display != nullptr) {
        EGLDisplay display = get_platform_display(EGL_PLATFORM_WAYLAND_KHR, wayland_display, nullptr);
        if (display != EGL_NO_DISPLAY) {
            return display;
        }
    }
    return eglGetDisplay(reinterpret_cast<EGLNativeDisplayType>(wayland_display));
}

EGLDisplay GetSurfacelessEglDisplay() {
    auto get_platform_display =
        reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));
    if (get_platform_display != nullptr) {
        EGLDisplay display = get_platform_display(EGL_PLATFORM_SURFACELESS_MESA, EGL_DEFAULT_DISPLAY, nullptr);
        if (display != EGL_NO_DISPLAY) {
            return display;
        }
    }
    return eglGetDisplay(EGL_DEFAULT_DISPLAY);
}

void RegistryGlobal(void* data, wl_registry* registry, uint32_t name, const char* interface, uint32_t version) {
    auto* state = static_cast<WaylandState*>(data);
    if (std::strcmp(interface, wl_compositor_interface.name) == 0) {
        const uint32_t bind_version = std::min<uint32_t>(version, 4);
        state->compositor = static_cast<wl_compositor*>(
            wl_registry_bind(registry, name, &wl_compositor_interface, bind_version));
    }
}

void RegistryGlobalRemove(void*, wl_registry*, uint32_t) {}

const wl_registry_listener kRegistryListener{
    .global = RegistryGlobal,
    .global_remove = RegistryGlobalRemove,
};

GlProbeResult FinishProbe(
    std::string backend,
    EGLDisplay display,
    EGLSurface surface,
    EGLContext context,
    int egl_major,
    int egl_minor) {
    GlProbeResult result;
    result.success = true;
    result.backend = std::move(backend);
    result.egl_major = egl_major;
    result.egl_minor = egl_minor;
    result.vendor = GlString(GL_VENDOR);
    result.renderer = GlString(GL_RENDERER);
    result.version = GlString(GL_VERSION);
    result.shading_language = GlString(GL_SHADING_LANGUAGE_VERSION);

    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(display, context);
    eglDestroySurface(display, surface);
    eglTerminate(display);
    return result;
}

GlProbeResult RunWaylandProbe() {
    wl_display* wayland_display = wl_display_connect(nullptr);
    if (wayland_display == nullptr) {
        return Failure("failed to connect to Wayland display");
    }

    wl_registry* registry = wl_display_get_registry(wayland_display);
    WaylandState state;
    wl_registry_add_listener(registry, &kRegistryListener, &state);
    wl_display_roundtrip(wayland_display);

    if (state.compositor == nullptr) {
        wl_registry_destroy(registry);
        wl_display_disconnect(wayland_display);
        return Failure("failed to bind Wayland compositor");
    }

    wl_surface* wayland_surface = wl_compositor_create_surface(state.compositor);
    wl_egl_window* egl_window = wayland_surface == nullptr ? nullptr : wl_egl_window_create(wayland_surface, 64, 64);
    if (wayland_surface == nullptr || egl_window == nullptr) {
        if (egl_window != nullptr) {
            wl_egl_window_destroy(egl_window);
        }
        if (wayland_surface != nullptr) {
            wl_surface_destroy(wayland_surface);
        }
        wl_compositor_destroy(state.compositor);
        wl_registry_destroy(registry);
        wl_display_disconnect(wayland_display);
        return Failure("failed to create Wayland EGL window");
    }

    EGLDisplay display = GetWaylandEglDisplay(wayland_display);
    int egl_major = 0;
    int egl_minor = 0;
    if (display == EGL_NO_DISPLAY || eglInitialize(display, &egl_major, &egl_minor) != EGL_TRUE) {
        wl_egl_window_destroy(egl_window);
        wl_surface_destroy(wayland_surface);
        wl_compositor_destroy(state.compositor);
        wl_registry_destroy(registry);
        wl_display_disconnect(wayland_display);
        return Failure("failed to initialize Wayland EGL");
    }

    const EGLint config_attributes[] = {
        EGL_SURFACE_TYPE,
        EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE,
        EGL_OPENGL_BIT,
        EGL_RED_SIZE,
        8,
        EGL_GREEN_SIZE,
        8,
        EGL_BLUE_SIZE,
        8,
        EGL_DEPTH_SIZE,
        24,
        EGL_NONE,
    };

    EGLConfig config = nullptr;
    EGLint config_count = 0;
    if (eglChooseConfig(display, config_attributes, &config, 1, &config_count) != EGL_TRUE || config_count < 1) {
        eglTerminate(display);
        wl_egl_window_destroy(egl_window);
        wl_surface_destroy(wayland_surface);
        wl_compositor_destroy(state.compositor);
        wl_registry_destroy(registry);
        wl_display_disconnect(wayland_display);
        return Failure("failed to choose Wayland EGL OpenGL config");
    }

    EGLSurface surface =
        eglCreateWindowSurface(display, config, reinterpret_cast<EGLNativeWindowType>(egl_window), nullptr);
    if (surface == EGL_NO_SURFACE || eglBindAPI(EGL_OPENGL_API) != EGL_TRUE) {
        eglTerminate(display);
        wl_egl_window_destroy(egl_window);
        wl_surface_destroy(wayland_surface);
        wl_compositor_destroy(state.compositor);
        wl_registry_destroy(registry);
        wl_display_disconnect(wayland_display);
        return Failure("failed to create Wayland EGL surface or bind OpenGL");
    }

    const EGLint context_attributes[] = {
        EGL_CONTEXT_MAJOR_VERSION,
        3,
        EGL_CONTEXT_MINOR_VERSION,
        3,
        EGL_NONE,
    };
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attributes);
    if (context == EGL_NO_CONTEXT || eglMakeCurrent(display, surface, surface, context) != EGL_TRUE) {
        if (context != EGL_NO_CONTEXT) {
            eglDestroyContext(display, context);
        }
        eglDestroySurface(display, surface);
        eglTerminate(display);
        wl_egl_window_destroy(egl_window);
        wl_surface_destroy(wayland_surface);
        wl_compositor_destroy(state.compositor);
        wl_registry_destroy(registry);
        wl_display_disconnect(wayland_display);
        return Failure("failed to create or activate Wayland EGL OpenGL context");
    }

    GlProbeResult result = FinishProbe("wayland-egl", display, surface, context, egl_major, egl_minor);
    wl_egl_window_destroy(egl_window);
    wl_surface_destroy(wayland_surface);
    wl_compositor_destroy(state.compositor);
    wl_registry_destroy(registry);
    wl_display_disconnect(wayland_display);
    return result;
}

GlProbeResult RunSurfacelessProbe() {
    EGLDisplay display = GetSurfacelessEglDisplay();
    int egl_major = 0;
    int egl_minor = 0;
    if (display == EGL_NO_DISPLAY || eglInitialize(display, &egl_major, &egl_minor) != EGL_TRUE) {
        return Failure("failed to initialize surfaceless EGL");
    }

    const EGLint config_attributes[] = {
        EGL_SURFACE_TYPE,
        EGL_PBUFFER_BIT,
        EGL_RENDERABLE_TYPE,
        EGL_OPENGL_BIT,
        EGL_RED_SIZE,
        8,
        EGL_GREEN_SIZE,
        8,
        EGL_BLUE_SIZE,
        8,
        EGL_DEPTH_SIZE,
        24,
        EGL_NONE,
    };

    EGLConfig config = nullptr;
    EGLint config_count = 0;
    if (eglChooseConfig(display, config_attributes, &config, 1, &config_count) != EGL_TRUE || config_count < 1) {
        eglTerminate(display);
        return Failure("failed to choose surfaceless EGL OpenGL config");
    }

    const EGLint surface_attributes[] = {
        EGL_WIDTH,
        64,
        EGL_HEIGHT,
        64,
        EGL_NONE,
    };
    EGLSurface surface = eglCreatePbufferSurface(display, config, surface_attributes);
    if (surface == EGL_NO_SURFACE || eglBindAPI(EGL_OPENGL_API) != EGL_TRUE) {
        eglTerminate(display);
        return Failure("failed to create surfaceless EGL surface or bind OpenGL");
    }

    const EGLint context_attributes[] = {
        EGL_CONTEXT_MAJOR_VERSION,
        3,
        EGL_CONTEXT_MINOR_VERSION,
        3,
        EGL_NONE,
    };
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attributes);
    if (context == EGL_NO_CONTEXT || eglMakeCurrent(display, surface, surface, context) != EGL_TRUE) {
        if (context != EGL_NO_CONTEXT) {
            eglDestroyContext(display, context);
        }
        eglDestroySurface(display, surface);
        eglTerminate(display);
        return Failure("failed to create or activate surfaceless EGL OpenGL context");
    }

    return FinishProbe("surfaceless-egl", display, surface, context, egl_major, egl_minor);
}

}  // namespace

int main(int argc, char** argv) {
    const bool require_nvidia = argc > 1 && std::strcmp(argv[1], "--require-nvidia") == 0;

    GlProbeResult result = RunWaylandProbe();
    if (!result.success) {
        std::cerr << "Wayland EGL probe failed: " << result.error << "\n";
        result = RunSurfacelessProbe();
    }

    if (!result.success) {
        std::cerr << "OpenGL probe failed: " << result.error << "\n";
        return EXIT_FAILURE;
    }

    std::cout << "backend: " << result.backend << "\n";
    std::cout << "EGL version: " << result.egl_major << "." << result.egl_minor << "\n";
    std::cout << "OpenGL vendor: " << result.vendor << "\n";
    std::cout << "OpenGL renderer: " << result.renderer << "\n";
    std::cout << "OpenGL version: " << result.version << "\n";
    std::cout << "OpenGL GLSL: " << result.shading_language << "\n";

    if (Contains(result.renderer, "llvmpipe")) {
        std::cerr << "software renderer detected: " << result.renderer << "\n";
        return EXIT_FAILURE;
    }

    if (require_nvidia && !Contains(result.renderer, "NVIDIA")) {
        std::cerr << "NVIDIA renderer required but active renderer is: " << result.renderer << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
