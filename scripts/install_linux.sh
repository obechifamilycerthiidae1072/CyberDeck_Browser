#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd -- "$script_dir/.." && pwd)"

CEF_URL_DEFAULT="https://cef-builds.spotifycdn.com/cef_binary_146.0.9%2Bg3ca6a87%2Bchromium-146.0.7680.165_linux64_minimal.tar.bz2"
cef_url="${CEF_URL:-$CEF_URL_DEFAULT}"
cef_root="${CEF_ROOT:-$repo_root/third_party/cef/linux64}"
build_dir="${BUILD_DIR:-$repo_root/build-linux-cef}"
install_dir="${INSTALL_DIR:-$HOME/.local/opt/cyberdeck-browser}"
bin_dir="${BIN_DIR:-$HOME/.local/bin}"

install_deps=0
force_cef=0
skip_tests=0
skip_install=0

usage() {
    cat <<USAGE
Usage: ./scripts/install_linux.sh [options]

Downloads official Linux CEF, builds the separated Linux CEF browser target,
and installs a local launcher.

Options:
  --deps              Install Ubuntu/WSL2 build and runtime packages first.
  --force-cef         Re-download and replace the local CEF SDK.
  --cef-url URL       Use a specific official CEF archive URL.
  --cef-root DIR      Extract/use CEF at DIR.
  --build-dir DIR     Build directory. Default: build-linux-cef.
  --install-dir DIR   Install directory. Default: ~/.local/opt/cyberdeck-browser.
  --bin-dir DIR       Wrapper directory. Default: ~/.local/bin.
  --skip-tests        Build without running ctest.
  --no-install        Build only; do not copy files into the install directory.
  -h, --help          Show this help.

Environment overrides: CEF_URL, CEF_ROOT, BUILD_DIR, INSTALL_DIR, BIN_DIR.
Official CEF downloads page: https://cef-builds.spotifycdn.com/index.html
USAGE
}

to_abs_dir() {
    local path="$1"
    if [[ "$path" == /* ]]; then
        printf '%s\n' "$path"
    else
        printf '%s/%s\n' "$repo_root" "$path"
    fi
}

validate_cef_url() {
    if [[ "$cef_url" != https://* ]]; then
        echo "Refusing non-HTTPS CEF URL: $cef_url" >&2
        exit 1
    fi

    if [[ "$cef_url" != https://cef-builds.spotifycdn.com/* &&
          "${CYBERDECK_ALLOW_CUSTOM_CEF_URL:-0}" != "1" ]]; then
        echo "Refusing custom CEF host: $cef_url" >&2
        echo "Set CYBERDECK_ALLOW_CUSTOM_CEF_URL=1 only if you trust the archive source." >&2
        exit 1
    fi
}

validate_cef_replace_target() {
    case "$cef_root" in
        ""|"/"|"$HOME"|"$HOME/"|"$repo_root"|"$repo_root/"|"$repo_root/third_party"|"$repo_root/third_party/"|"$repo_root/third_party/cef"|"$repo_root/third_party/cef/")
            echo "Refusing unsafe CEF replacement path: $cef_root" >&2
            exit 1
            ;;
    esac

    if [[ "$cef_root" != "$repo_root/third_party/cef/"* &&
          "${CYBERDECK_ALLOW_EXTERNAL_CEF_ROOT:-0}" != "1" ]]; then
        echo "Refusing to replace CEF outside the repository third_party/cef tree:" >&2
        echo "  $cef_root" >&2
        echo "Set CYBERDECK_ALLOW_EXTERNAL_CEF_ROOT=1 only if this path is dedicated to CEF." >&2
        exit 1
    fi
}

validate_cef_archive_listing() {
    local archive_path="$1"
    local saw_entry=0
    local entry

    while IFS= read -r entry; do
        saw_entry=1
        if [[ "$entry" == /* ||
              "$entry" == ../* ||
              "$entry" == */../* ||
              "$entry" == */.. ||
              "$entry" != cef_binary_* ]]; then
            echo "CEF archive contains an unsafe path: $entry" >&2
            exit 1
        fi
    done < <(tar -tjf "$archive_path")

    if [[ "$saw_entry" -eq 0 ]]; then
        echo "CEF archive is empty: $archive_path" >&2
        exit 1
    fi
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --deps)
            install_deps=1
            shift
            ;;
        --force-cef)
            force_cef=1
            shift
            ;;
        --cef-url)
            cef_url="${2:?missing URL after --cef-url}"
            shift 2
            ;;
        --cef-root)
            cef_root="${2:?missing DIR after --cef-root}"
            shift 2
            ;;
        --build-dir)
            build_dir="${2:?missing DIR after --build-dir}"
            shift 2
            ;;
        --install-dir)
            install_dir="${2:?missing DIR after --install-dir}"
            shift 2
            ;;
        --bin-dir)
            bin_dir="${2:?missing DIR after --bin-dir}"
            shift 2
            ;;
        --skip-tests)
            skip_tests=1
            shift
            ;;
        --no-install)
            skip_install=1
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1" >&2
            usage >&2
            exit 2
            ;;
    esac
done

cef_root="$(to_abs_dir "$cef_root")"
build_dir="$(to_abs_dir "$build_dir")"
install_dir="$(to_abs_dir "$install_dir")"
bin_dir="$(to_abs_dir "$bin_dir")"
validate_cef_url

have_cef_distribution() {
    [[ -f "$cef_root/include/cef_version.h" &&
       -f "$cef_root/cmake/FindCEF.cmake" &&
       -f "$cef_root/libcef_dll/CMakeLists.txt" &&
       -f "$cef_root/Release/libcef.so" ]]
}

validate_cef_distribution() {
    if ! have_cef_distribution; then
        echo "CEF SDK is incomplete at: $cef_root" >&2
        echo "Expected include/cef_version.h, cmake/FindCEF.cmake, libcef_dll/CMakeLists.txt, and Release/libcef.so." >&2
        exit 1
    fi
}

install_ubuntu_deps() {
    if ! command -v apt-get >/dev/null 2>&1; then
        echo "--deps currently supports Ubuntu/Debian systems with apt-get." >&2
        exit 1
    fi

    local packages=(
        build-essential
        ca-certificates
        cmake
        curl
        ninja-build
        pkg-config
        tar
        bzip2
        mesa-utils
        libgtk-3-dev
        libnss3-dev
        libx11-dev
        libxcomposite-dev
        libxdamage-dev
        libxrandr-dev
        libxtst-dev
        libxss-dev
        libxkbcommon-dev
        libgbm-dev
        libasound2-dev
        libgl1-mesa-dev
        libegl1-mesa-dev
        libwayland-dev
    )

    sudo apt-get update
    sudo apt-get install -y "${packages[@]}"
}

download_cef() {
    if have_cef_distribution && [[ "$force_cef" -eq 0 ]]; then
        echo "Using existing CEF SDK: $cef_root"
        return
    fi

    if ! command -v curl >/dev/null 2>&1; then
        echo "curl is required to download CEF. Run with --deps or install curl." >&2
        exit 1
    fi

    local downloads_dir="$repo_root/third_party/cef-downloads"
    local archive_name
    archive_name="$(basename "${cef_url%%\?*}")"
    if [[ -z "$archive_name" || "$archive_name" == "." ]]; then
        archive_name="cef-linux64.tar.bz2"
    fi
    local archive_path="$downloads_dir/$archive_name"
    local extract_dir
    extract_dir="$(mktemp -d)"
    local staged_dir="$cef_root.tmp"

    validate_cef_replace_target
    mkdir -p "$downloads_dir" "$(dirname "$cef_root")"
    if [[ "$force_cef" -eq 1 || ! -s "$archive_path" ]]; then
        echo "Downloading official CEF archive:"
        echo "  $cef_url"
        curl --fail --location --retry 3 --output "$archive_path" "$cef_url"
    else
        echo "Using cached CEF archive: $archive_path"
    fi

    validate_cef_archive_listing "$archive_path"

    echo "Extracting CEF SDK to: $cef_root"
    tar -xjf "$archive_path" -C "$extract_dir"

    local extracted_root
    extracted_root="$(find "$extract_dir" -mindepth 1 -maxdepth 1 -type d -name 'cef_binary_*' | sort | head -n 1)"
    if [[ -z "$extracted_root" ]]; then
        echo "CEF archive did not contain a cef_binary_* top-level directory." >&2
        exit 1
    fi

    rm -rf "$staged_dir"
    mv "$extracted_root" "$staged_dir"
    rm -rf "$cef_root"
    mv "$staged_dir" "$cef_root"
    validate_cef_distribution
    rm -rf "$extract_dir" "$staged_dir"
}

configure_and_build() {
    local cmake_args=(
        -S "$repo_root"
        -B "$build_dir"
        -DCMAKE_BUILD_TYPE=Release
        -DCEF_ROOT="$cef_root"
        -DCYBERDECK_REQUIRE_CEF=ON
    )

    if command -v ninja >/dev/null 2>&1; then
        cmake_args+=(-G Ninja)
    fi

    cmake "${cmake_args[@]}"
    cmake --build "$build_dir" --parallel

    if [[ "$skip_tests" -eq 0 ]]; then
        ctest --test-dir "$build_dir" --output-on-failure
    fi
}

copy_if_exists() {
    local source="$1"
    local dest="$2"
    if [[ -e "$source" ]]; then
        cp -a "$source" "$dest"
    fi
}

install_runtime() {
    if [[ "$skip_install" -eq 1 ]]; then
        return
    fi

    if [[ ! -x "$build_dir/cyberdeck-browser-cef" ]]; then
        echo "CEF browser binary was not built: $build_dir/cyberdeck-browser-cef" >&2
        exit 1
    fi

    mkdir -p "$install_dir" "$bin_dir"
    install -m 0755 "$build_dir/cyberdeck-browser-cef" "$install_dir/cyberdeck-browser-cef"
    copy_if_exists "$build_dir/cyberdeck-browser" "$install_dir/"
    copy_if_exists "$build_dir/cyberdeck-wsl2-opengl-probe" "$install_dir/"

    local file
    for file in "$build_dir"/*.so "$build_dir"/*.bin "$build_dir"/*.dat "$build_dir"/*.pak "$build_dir"/chrome-sandbox; do
        [[ -e "$file" ]] && cp -a "$file" "$install_dir/"
    done

    local dir
    for dir in locales swiftshader; do
        [[ -d "$build_dir/$dir" ]] && cp -a "$build_dir/$dir" "$install_dir/"
    done

    local app_dir_literal
    app_dir_literal="$(printf '%q' "$install_dir")"
    cat > "$bin_dir/cyberdeck-browser" <<WRAPPER
#!/usr/bin/env bash
set -euo pipefail
app_dir=$app_dir_literal

if grep -qi microsoft /proc/version 2>/dev/null; then
    export GALLIUM_DRIVER="\${GALLIUM_DRIVER:-d3d12}"
    export MESA_D3D12_DEFAULT_ADAPTER_NAME="\${MESA_D3D12_DEFAULT_ADAPTER_NAME:-NVIDIA}"
fi

export LD_LIBRARY_PATH="\$app_dir:\${LD_LIBRARY_PATH:-}"
exec "\$app_dir/cyberdeck-browser-cef" "\$@"
WRAPPER
    chmod +x "$bin_dir/cyberdeck-browser"
}

cd "$repo_root"

if [[ "$install_deps" -eq 1 ]]; then
    install_ubuntu_deps
fi

download_cef
configure_and_build
install_runtime

echo
echo "Linux CEF browser build complete."
echo "CEF SDK: $cef_root"
echo "Build:   $build_dir"
if [[ "$skip_install" -eq 0 ]]; then
    echo "Install: $install_dir"
    echo "Run:     $bin_dir/cyberdeck-browser \"https://www.example.com\""
else
    echo "Run:     $build_dir/cyberdeck-browser-cef \"https://www.example.com\""
fi
