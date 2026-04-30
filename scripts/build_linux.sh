#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd -- "$script_dir/.." && pwd)"

build_dir="${1:-$repo_root/build-linux}"
if [[ "$build_dir" != /* ]]; then
    build_dir="$repo_root/$build_dir"
fi

cmake -S "$repo_root" -B "$build_dir"
cmake --build "$build_dir" --parallel
ctest --test-dir "$build_dir" --output-on-failure

echo
echo "Core Linux diagnostics binary:"
echo "  $build_dir/cyberdeck-browser"
if [[ -x "$repo_root/build-linux-cef/cyberdeck-browser-cef" ]]; then
    echo "Full Linux CEF browser binary:"
    echo "  $repo_root/build-linux-cef/cyberdeck-browser-cef"
    echo "Recommended launcher:"
    echo "  $repo_root/scripts/run_linux.sh \"https://www.example.com\""
else
    echo "Full Linux CEF browser binary is not built yet."
    echo "Build/install it with:"
    echo "  $repo_root/scripts/install_linux.sh"
fi
