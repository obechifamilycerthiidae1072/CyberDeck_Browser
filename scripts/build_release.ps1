param(
    [string]$BuildDir = "build-release",
    [string]$CefRoot = "",
    [switch]$RequireCef,
    [string]$Generator = ""
)

$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$buildPath = Join-Path $repoRoot $BuildDir

$configureArgs = @(
    "-S", $repoRoot,
    "-B", $buildPath,
    "-DCMAKE_BUILD_TYPE=Release"
)

if ($Generator.Trim().Length -gt 0) {
    $configureArgs = @("-G", $Generator) + $configureArgs
}

if ($CefRoot.Trim().Length -gt 0) {
    $configureArgs += "-DCEF_ROOT=$CefRoot"
}

if ($RequireCef) {
    $configureArgs += "-DCYBERDECK_REQUIRE_CEF=ON"
}

Write-Host "Configuring CyberDeck Browser release build..."
cmake @configureArgs

Write-Host "Building CyberDeck Browser release binaries..."
cmake --build $buildPath --config Release --target CyberDeckBrowser

Write-Host "Release build complete: $buildPath"
