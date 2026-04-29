cmake_minimum_required(VERSION 3.24)

project(
    CyberDeckBrowser
    VERSION 0.1.0
    DESCRIPTION "Native Windows 11 C++20 browser shell using CEF and OpenGL Deck Space"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(CYBERDECK_REQUIRE_CEF "Fail configure if CEF_ROOT is not provided or invalid." OFF)
set(CEF_ROOT "" CACHE PATH "Path to an extracted CEF binary distribution.")

add_executable(CyberDeckBrowser WIN32
    src/app/Application.cpp
    src/app/Application.h
    src/browser/BrowserHost.cpp
    src/browser/BrowserHost.h
    src/browser/UrlNavigation.cpp
    src/browser/UrlNavigation.h
    src/common/AppInfo.cpp
    src/common/AppInfo.h
    src/deck/BookmarkNode.cpp
    src/deck/BookmarkNode.h
    src/deck/BookmarkStore.cpp
    src/deck/BookmarkStore.h
    src/deck/FaviconStore.cpp
    src/deck/FaviconStore.h
    src/common/Logger.cpp
    src/common/Logger.h
    src/deck/DeckSpaceController.cpp
    src/deck/DeckSpaceController.h
    src/history/HistoryStore.cpp
    src/history/HistoryStore.h
    src/main/MainWindow.cpp
    src/main/MainWindow.h
    src/render/DeckAnimation.cpp
    src/render/DeckAnimation.h
    src/render/DeckGeometry.cpp
    src/render/DeckGeometry.h
    src/render/DeckLayout.cpp
    src/render/DeckLayout.h
    src/render/GpuPreference.cpp
    src/render/NeonMaterial.cpp
    src/render/NeonMaterial.h
    src/render/OpenGLDeckView.cpp
    src/render/OpenGLDeckView.h
    src/settings/SettingsStore.cpp
    src/settings/SettingsStore.h
    src/main/WinMain.cpp
    src/ui/Theme.cpp
    src/ui/Theme.h
)

target_compile_definitions(CyberDeckBrowser
    PRIVATE
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

target_include_directories(CyberDeckBrowser
    PRIVATE
        src
)

add_executable(CyberDeckUrlNavigationTests
    src/browser/UrlNavigation.cpp
    src/browser/UrlNavigation.h
    tests/UrlNavigationTests.cpp
)

add_executable(CyberDeckHistoryStoreTests
    src/common/Logger.cpp
    src/common/Logger.h
    src/history/HistoryStore.cpp
    src/history/HistoryStore.h
    tests/HistoryStoreTests.cpp
)

add_executable(CyberDeckSettingsStoreTests
    src/common/Logger.cpp
    src/common/Logger.h
    src/settings/SettingsStore.cpp
    src/settings/SettingsStore.h
    tests/SettingsStoreTests.cpp
)

add_executable(CyberDeckLoggerTests
    src/common/Logger.cpp
    src/common/Logger.h
    tests/LoggerTests.cpp
)

add_executable(CyberDeckDeckGeometryTests
    src/render/DeckGeometry.cpp
    src/render/DeckGeometry.h
    src/render/NeonMaterial.h
    tests/DeckGeometryTests.cpp
)

add_executable(CyberDeckDeckAnimationTests
    src/render/DeckAnimation.cpp
    src/render/DeckAnimation.h
    src/render/DeckGeometry.h
    src/render/NeonMaterial.h
    tests/DeckAnimationTests.cpp
)

add_executable(CyberDeckDeckLayoutTests
    src/render/DeckGeometry.h
    src/render/DeckLayout.cpp
    src/render/DeckLayout.h
    src/render/NeonMaterial.h
    tests/DeckLayoutTests.cpp
)

add_executable(CyberDeckBookmarkNodeTests
    src/browser/UrlNavigation.cpp
    src/browser/UrlNavigation.h
    src/deck/BookmarkNode.cpp
    src/deck/BookmarkNode.h
    tests/BookmarkNodeTests.cpp
)

add_executable(CyberDeckBookmarkStoreTests
    src/browser/UrlNavigation.cpp
    src/browser/UrlNavigation.h
    src/common/Logger.cpp
    src/common/Logger.h
    src/deck/BookmarkNode.cpp
    src/deck/BookmarkNode.h
    src/deck/BookmarkStore.cpp
    src/deck/BookmarkStore.h
    tests/BookmarkStoreTests.cpp
)

add_executable(CyberDeckFaviconStoreTests
    src/deck/FaviconStore.cpp
    src/deck/FaviconStore.h
    tests/FaviconStoreTests.cpp
)

target_compile_definitions(CyberDeckUrlNavigationTests
    PRIVATE
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

target_compile_definitions(CyberDeckHistoryStoreTests
    PRIVATE
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

target_compile_definitions(CyberDeckSettingsStoreTests
    PRIVATE
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

target_compile_definitions(CyberDeckLoggerTests
    PRIVATE
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

target_compile_definitions(CyberDeckDeckGeometryTests
    PRIVATE
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

target_compile_definitions(CyberDeckDeckAnimationTests
    PRIVATE
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

target_compile_definitions(CyberDeckDeckLayoutTests
    PRIVATE
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

target_compile_definitions(CyberDeckBookmarkNodeTests
    PRIVATE
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

target_compile_definitions(CyberDeckBookmarkStoreTests
    PRIVATE
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

target_compile_definitions(CyberDeckFaviconStoreTests
    PRIVATE
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

target_include_directories(CyberDeckUrlNavigationTests
    PRIVATE
        src
)

target_include_directories(CyberDeckHistoryStoreTests
    PRIVATE
        src
)

target_include_directories(CyberDeckSettingsStoreTests
    PRIVATE
        src
)

target_include_directories(CyberDeckLoggerTests
    PRIVATE
        src
)

target_include_directories(CyberDeckDeckGeometryTests
    PRIVATE
        src
)

target_include_directories(CyberDeckDeckAnimationTests
    PRIVATE
        src
)

target_include_directories(CyberDeckDeckLayoutTests
    PRIVATE
        src
)

target_include_directories(CyberDeckBookmarkNodeTests
    PRIVATE
        src
)

target_include_directories(CyberDeckBookmarkStoreTests
    PRIVATE
        src
)

target_include_directories(CyberDeckFaviconStoreTests
    PRIVATE
        src
)

if(MSVC)
    target_compile_options(CyberDeckBrowser PRIVATE /W4 /permissive- /EHsc)
    target_compile_options(CyberDeckUrlNavigationTests PRIVATE /W4 /permissive- /EHsc)
    target_compile_options(CyberDeckHistoryStoreTests PRIVATE /W4 /permissive- /EHsc)
    target_compile_options(CyberDeckSettingsStoreTests PRIVATE /W4 /permissive- /EHsc)
    target_compile_options(CyberDeckLoggerTests PRIVATE /W4 /permissive- /EHsc)
    target_compile_options(CyberDeckDeckGeometryTests PRIVATE /W4 /permissive- /EHsc)
    target_compile_options(CyberDeckDeckAnimationTests PRIVATE /W4 /permissive- /EHsc)
    target_compile_options(CyberDeckDeckLayoutTests PRIVATE /W4 /permissive- /EHsc)
    target_compile_options(CyberDeckBookmarkNodeTests PRIVATE /W4 /permissive- /EHsc)
    target_compile_options(CyberDeckBookmarkStoreTests PRIVATE /W4 /permissive- /EHsc)
    target_compile_options(CyberDeckFaviconStoreTests PRIVATE /W4 /permissive- /EHsc)
    set_property(
        TARGET CyberDeckBrowser
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
    set_property(
        TARGET CyberDeckUrlNavigationTests
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
    set_property(
        TARGET CyberDeckHistoryStoreTests
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
    set_property(
        TARGET CyberDeckSettingsStoreTests
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
    set_property(
        TARGET CyberDeckLoggerTests
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
    set_property(
        TARGET CyberDeckDeckGeometryTests
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
    set_property(
        TARGET CyberDeckDeckAnimationTests
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
    set_property(
        TARGET CyberDeckDeckLayoutTests
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
    set_property(
        TARGET CyberDeckBookmarkNodeTests
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
    set_property(
        TARGET CyberDeckBookmarkStoreTests
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
    set_property(
        TARGET CyberDeckFaviconStoreTests
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
else()
    target_compile_options(CyberDeckBrowser PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(CyberDeckUrlNavigationTests PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(CyberDeckHistoryStoreTests PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(CyberDeckSettingsStoreTests PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(CyberDeckLoggerTests PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(CyberDeckDeckGeometryTests PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(CyberDeckDeckAnimationTests PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(CyberDeckDeckLayoutTests PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(CyberDeckBookmarkNodeTests PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(CyberDeckBookmarkStoreTests PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(CyberDeckFaviconStoreTests PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(MINGW)
    target_link_options(CyberDeckBrowser PRIVATE -municode)
endif()

if(WIN32)
    target_link_libraries(CyberDeckBrowser PRIVATE dwmapi gdi32 ole32 opengl32 shell32 user32)
else()
    message(FATAL_ERROR "CyberDeck Browser v1 targets Windows 11 only.")
endif()

if(CEF_ROOT)
    cmake_path(ABSOLUTE_PATH CEF_ROOT NORMALIZE OUTPUT_VARIABLE CYBERDECK_CEF_ROOT)
    set(CYBERDECK_CEF_INCLUDE_DIR "${CYBERDECK_CEF_ROOT}/include")
    set(CYBERDECK_CEF_CMAKE_DIR "${CYBERDECK_CEF_ROOT}/cmake")
    set(CYBERDECK_CEF_WRAPPER_DIR "${CYBERDECK_CEF_ROOT}/libcef_dll")

    if(EXISTS "${CYBERDECK_CEF_INCLUDE_DIR}/cef_version.h"
        AND EXISTS "${CYBERDECK_CEF_CMAKE_DIR}/FindCEF.cmake"
        AND EXISTS "${CYBERDECK_CEF_WRAPPER_DIR}/CMakeLists.txt")
        if(MINGW)
            if(CYBERDECK_REQUIRE_CEF)
                message(FATAL_ERROR "Official Windows CEF binary distributions require MSVC-compatible builds. Reconfigure with Visual Studio 2022 or Ninja from an MSVC developer shell.")
            endif()
            message(WARNING "CEF_ROOT is valid, but the current MinGW generator cannot link official Windows CEF binaries. Building placeholder mode.")
        else()
            set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CYBERDECK_CEF_CMAKE_DIR}")
            find_package(CEF REQUIRED)

            ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")
            add_subdirectory("${CEF_LIBCEF_DLL_WRAPPER_PATH}" libcef_dll_wrapper)

            target_include_directories(CyberDeckBrowser SYSTEM PRIVATE "${CYBERDECK_CEF_ROOT}")
            target_compile_definitions(CyberDeckBrowser PRIVATE CYBERDECK_HAS_CEF=1)
            add_dependencies(CyberDeckBrowser libcef_dll_wrapper)
            target_link_libraries(CyberDeckBrowser PRIVATE libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})

            add_custom_command(
                TARGET CyberDeckBrowser
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "$<IF:$<CONFIG:Debug>,${CEF_BINARY_DIR_DEBUG},${CEF_BINARY_DIR_RELEASE}>"
                    "$<TARGET_FILE_DIR:CyberDeckBrowser>"
                COMMENT "Copying CEF runtime binaries"
            )

            if(EXISTS "${CYBERDECK_CEF_ROOT}/Resources")
                add_custom_command(
                    TARGET CyberDeckBrowser
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        "${CYBERDECK_CEF_ROOT}/Resources"
                        "$<TARGET_FILE_DIR:CyberDeckBrowser>"
                    COMMENT "Copying CEF resource files"
                )
            endif()

            message(STATUS "CEF integration enabled from: ${CYBERDECK_CEF_ROOT}")
        endif()
    elseif(CYBERDECK_REQUIRE_CEF)
        message(FATAL_ERROR "CEF_ROOT is set but does not look like a standard CEF binary distribution: ${CYBERDECK_CEF_ROOT}")
    else()
        message(WARNING "CEF_ROOT is set but does not look like a standard CEF binary distribution: ${CYBERDECK_CEF_ROOT}")
    endif()
elseif(CYBERDECK_REQUIRE_CEF)
    message(FATAL_ERROR "CEF_ROOT is required. Provide -DCEF_ROOT=<path-to-cef-binary>.")
else()
    message(STATUS "CEF_ROOT not set. Building scaffold without CEF headers for now.")
endif()
