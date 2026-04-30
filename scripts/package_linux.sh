#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd -- "$script_dir/.." && pwd)"

build_dir="${BUILD_DIR:-$repo_root/build-linux-cef}"
output_dir="${OUTPUT_DIR:-$repo_root/dist/linux}"
package_name="${PACKAGE_NAME:-cyberdeck-browser}"
package_version="${PACKAGE_VERSION:-0.1.0~rc1-1}"
asset_version="${ASSET_VERSION:-0.1.0-rc1}"
architecture="${ARCHITECTURE:-amd64}"
app_dir_name="cyberdeck-browser"
install_prefix="/opt/$app_dir_name"

usage() {
    cat <<USAGE
Usage: ./scripts/package_linux.sh [options]

Builds Linux binary release artifacts from build-linux-cef:
  - Debian/Ubuntu .deb package
  - Portable Linux x86_64 tarball
  - SHA-256 checksum file

Options:
  --build-dir DIR       CEF build directory. Default: build-linux-cef.
  --output-dir DIR      Artifact output directory. Default: dist/linux.
  --package-version V   Debian package version. Default: 0.1.0~rc1-1.
  --asset-version V     Filename/release asset version. Default: 0.1.0-rc1.
  -h, --help            Show this help.

Environment overrides: BUILD_DIR, OUTPUT_DIR, PACKAGE_VERSION, ASSET_VERSION.
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

copy_if_exists() {
    local source="$1"
    local dest="$2"
    if [[ -e "$source" ]]; then
        cp -a "$source" "$dest"
    fi
}

copy_runtime_files() {
    local target_dir="$1"
    install -m 0755 "$build_dir/cyberdeck-browser-cef" "$target_dir/cyberdeck-browser-cef"
    copy_if_exists "$build_dir/cyberdeck-browser" "$target_dir/"
    copy_if_exists "$build_dir/cyberdeck-wsl2-opengl-probe" "$target_dir/"

    local file
    for file in "$build_dir"/*.so "$build_dir"/*.bin "$build_dir"/*.dat "$build_dir"/*.pak "$build_dir"/chrome-sandbox; do
        [[ -e "$file" ]] && cp -a "$file" "$target_dir/"
    done

    local dir
    for dir in locales swiftshader; do
        [[ -d "$build_dir/$dir" ]] && cp -a "$build_dir/$dir" "$target_dir/"
    done
    return 0
}

write_launcher() {
    local launcher_path="$1"
    local app_dir="$2"
    cat > "$launcher_path" <<LAUNCHER
#!/usr/bin/env bash
set -euo pipefail
app_dir="$app_dir"

if grep -qi microsoft /proc/version 2>/dev/null; then
    export GALLIUM_DRIVER="\${GALLIUM_DRIVER:-d3d12}"
    export MESA_D3D12_DEFAULT_ADAPTER_NAME="\${MESA_D3D12_DEFAULT_ADAPTER_NAME:-NVIDIA}"
fi

export LD_LIBRARY_PATH="\$app_dir:\${LD_LIBRARY_PATH:-}"
exec "\$app_dir/cyberdeck-browser-cef" "\$@"
LAUNCHER
    chmod 0755 "$launcher_path"
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --build-dir)
            build_dir="${2:?missing DIR after --build-dir}"
            shift 2
            ;;
        --output-dir)
            output_dir="${2:?missing DIR after --output-dir}"
            shift 2
            ;;
        --package-version)
            package_version="${2:?missing version after --package-version}"
            shift 2
            ;;
        --asset-version)
            asset_version="${2:?missing version after --asset-version}"
            shift 2
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

build_dir="$(to_abs_dir "$build_dir")"
output_dir="$(to_abs_dir "$output_dir")"

if [[ ! -x "$build_dir/cyberdeck-browser-cef" ]]; then
    echo "Missing Linux CEF binary: $build_dir/cyberdeck-browser-cef" >&2
    exit 1
fi
if [[ ! -f "$build_dir/libcef.so" ]]; then
    echo "Missing bundled CEF runtime: $build_dir/libcef.so" >&2
    exit 1
fi
if ! command -v dpkg-deb >/dev/null 2>&1; then
    echo "dpkg-deb is required to build the .deb package." >&2
    exit 1
fi

rm -rf "$output_dir"
mkdir -p "$output_dir"

deb_root="$output_dir/deb-root"
deb_app_dir="$deb_root$install_prefix"
deb_doc_dir="$deb_root/usr/share/doc/$package_name"
mkdir -p "$deb_app_dir" "$deb_root/usr/bin" "$deb_root/usr/share/applications" "$deb_doc_dir" "$deb_root/DEBIAN"
copy_runtime_files "$deb_app_dir"
write_launcher "$deb_root/usr/bin/cyberdeck-browser" "$install_prefix"

cat > "$deb_root/usr/share/applications/cyberdeck-browser.desktop" <<DESKTOP
[Desktop Entry]
Type=Application
Name=CyberDeck Browser
Comment=Retro terminal-style CEF browser shell with Deck Space Nodes
Exec=cyberdeck-browser %u
Terminal=false
Categories=Network;WebBrowser;
StartupNotify=true
DESKTOP

for doc in README.md README_LINUX.md LICENSE THIRD_PARTY_NOTICES.md; do
    [[ -f "$repo_root/$doc" ]] && cp -a "$repo_root/$doc" "$deb_doc_dir/"
done

installed_size="$(du -ks "$deb_root" | awk '{print $1}')"
cat > "$deb_root/DEBIAN/control" <<CONTROL
Package: $package_name
Version: $package_version
Section: web
Priority: optional
Architecture: $architecture
Maintainer: Harmonic Relic Foundry <release@harmonic-relic-foundry.local>
Installed-Size: $installed_size
Depends: bash, libc6, libstdc++6, libgcc-s1, libx11-6, libglib2.0-0, libnss3, libnspr4, libxcomposite1, libxdamage1, libxrandr2, libgbm1, libxkbcommon0, libasound2 | libasound2t64, libgtk-3-0
Description: CyberDeck Browser
 A separated Linux CEF browser shell with Terminal Mode controls, local JSON
 history/settings, and Deck Space Nodes.
CONTROL

deb_path="$output_dir/CyberDeckBrowser-$asset_version-linux-$architecture.deb"
dpkg-deb --build --root-owner-group "$deb_root" "$deb_path"

portable_root="$output_dir/CyberDeckBrowser-$asset_version-linux-x86_64"
portable_app_dir="$portable_root/app"
portable_doc_dir="$portable_root/docs"
mkdir -p "$portable_app_dir" "$portable_doc_dir"
copy_runtime_files "$portable_app_dir"
write_launcher "$portable_root/cyberdeck-browser" "\$(cd -- \"\$(dirname -- \"\${BASH_SOURCE[0]}\")\" && pwd)/app"
for doc in README.md README_LINUX.md LICENSE THIRD_PARTY_NOTICES.md; do
    [[ -f "$repo_root/$doc" ]] && cp -a "$repo_root/$doc" "$portable_doc_dir/"
done

tar_path="$output_dir/CyberDeckBrowser-$asset_version-linux-x86_64.tar.gz"
tar -C "$output_dir" -czf "$tar_path" "$(basename "$portable_root")"

sha_path="$output_dir/CyberDeckBrowser-$asset_version-linux-sha256.txt"
(
    cd "$output_dir"
    sha256sum "$(basename "$deb_path")" "$(basename "$tar_path")"
) > "$sha_path"

echo "Linux release artifacts:"
echo "  $deb_path"
echo "  $tar_path"
echo "  $sha_path"
