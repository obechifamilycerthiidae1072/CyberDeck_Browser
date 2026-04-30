#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd -- "$script_dir/.." && pwd)"

build_dir="${BUILD_DIR:-$repo_root/build-linux-cef}"
install_dir="${INSTALL_DIR:-$HOME/.local/opt/cyberdeck-browser}"

if grep -qi microsoft /proc/version 2>/dev/null; then
    export GALLIUM_DRIVER="${GALLIUM_DRIVER:-d3d12}"
    export MESA_D3D12_DEFAULT_ADAPTER_NAME="${MESA_D3D12_DEFAULT_ADAPTER_NAME:-NVIDIA}"
fi

if [[ -x "$build_dir/cyberdeck-browser-cef" ]]; then
    export LD_LIBRARY_PATH="$build_dir:${LD_LIBRARY_PATH:-}"
    exec "$build_dir/cyberdeck-browser-cef" "$@"
fi

if [[ -x "$install_dir/cyberdeck-browser-cef" ]]; then
    export LD_LIBRARY_PATH="$install_dir:${LD_LIBRARY_PATH:-}"
    exec "$install_dir/cyberdeck-browser-cef" "$@"
fi

if [[ -x "$repo_root/build-linux/cyberdeck-browser" ]]; then
    exec "$repo_root/build-linux/cyberdeck-browser" "$@"
fi

if [[ -x "$build_dir/cyberdeck-browser" ]]; then
    exec "$build_dir/cyberdeck-browser" "$@"
fi

echo "No Linux browser binary found." >&2
echo "Build the core launcher with ./scripts/build_linux.sh or install CEF with ./scripts/install_linux.sh --deps." >&2
exit 1
