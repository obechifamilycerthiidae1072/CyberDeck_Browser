#!/usr/bin/env bash
set -euo pipefail

build_dir="${1:-build-wsl2}"

run_probe() {
    local label="$1"
    shift

    echo
    echo "== $label =="
    if "$@"; then
        return 0
    fi

    echo "warning: $label failed; retrying once"
    sleep 1
    "$@" || echo "warning: $label failed after retry"
}

echo "== WSL2 environment =="
uname -a
if ! grep -qi "microsoft.*wsl2" /proc/version; then
    echo "warning: /proc/version does not look like WSL2"
fi
echo "DISPLAY=${DISPLAY:-}"
echo "WAYLAND_DISPLAY=${WAYLAND_DISPLAY:-}"

if command -v glxinfo >/dev/null 2>&1; then
    run_probe "OpenGL default renderer" /bin/bash -lc "glxinfo -B"
else
    echo
    echo "== OpenGL default renderer =="
    echo "glxinfo not found; install mesa-utils to inspect WSLg OpenGL"
fi

if command -v glxinfo >/dev/null 2>&1; then
    run_probe "OpenGL WSL2 NVIDIA renderer" /bin/bash -lc "GALLIUM_DRIVER=d3d12 MESA_D3D12_DEFAULT_ADAPTER_NAME=NVIDIA glxinfo -B"
fi

if command -v nvidia-smi >/dev/null 2>&1; then
    run_probe "NVIDIA bridge" /bin/bash -lc "nvidia-smi"
else
    echo
    echo "== NVIDIA bridge =="
    echo "nvidia-smi not found"
fi

export GALLIUM_DRIVER=d3d12
export MESA_D3D12_DEFAULT_ADAPTER_NAME=NVIDIA

echo
echo "== Build and tests =="
cmake -S . -B "$build_dir"
cmake --build "$build_dir" --parallel
ctest --test-dir "$build_dir" --output-on-failure

echo
echo "== Linux launcher smoke test =="
"$build_dir/cyberdeck-browser" "example.com"

echo
echo "== Linux OpenGL smoke test =="
"$build_dir/cyberdeck-wsl2-opengl-probe" --require-nvidia || echo "warning: Linux OpenGL smoke test failed in this shell; run it directly from your WSL terminal with GALLIUM_DRIVER=d3d12 MESA_D3D12_DEFAULT_ADAPTER_NAME=NVIDIA"
