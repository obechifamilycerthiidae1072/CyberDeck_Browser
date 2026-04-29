param(
    [string]$BuildDir = "build-release",
    [string]$Configuration = "Release",
    [string]$PackageDir = "dist\installer-staging",
    [string]$OutputDir = "dist",
    [string]$IsccPath = "",
    [switch]$SkipCompile
)

$ErrorActionPreference = "Stop"

function Resolve-Iscc {
    param([string]$ExplicitPath)

    if ($ExplicitPath.Trim().Length -gt 0) {
        if (!(Test-Path -LiteralPath $ExplicitPath)) {
            throw "ISCC.exe was not found at '$ExplicitPath'."
        }
        return (Resolve-Path -LiteralPath $ExplicitPath).Path
    }

    $command = Get-Command "ISCC.exe" -ErrorAction SilentlyContinue
    if ($null -ne $command) {
        return $command.Source
    }

    $candidates = @(
        "${env:ProgramFiles(x86)}\Inno Setup 6\ISCC.exe",
        "$env:ProgramFiles\Inno Setup 6\ISCC.exe"
    )
    foreach ($candidate in $candidates) {
        if ($candidate -and (Test-Path -LiteralPath $candidate)) {
            return $candidate
        }
    }

    throw "Inno Setup compiler ISCC.exe was not found. Install Inno Setup 6 or pass -IsccPath."
}

function Copy-FilePattern {
    param(
        [string]$SourceDir,
        [string]$Pattern,
        [string]$DestinationDir
    )

    Get-ChildItem -LiteralPath $SourceDir -File -Filter $Pattern -ErrorAction SilentlyContinue |
        ForEach-Object {
            Copy-Item -LiteralPath $_.FullName -Destination $DestinationDir -Force
        }
}

function Copy-DirectoryIfPresent {
    param(
        [string]$SourceDir,
        [string]$Name,
        [string]$DestinationDir
    )

    $source = Join-Path $SourceDir $Name
    if (Test-Path -LiteralPath $source -PathType Container) {
        Copy-Item -LiteralPath $source -Destination (Join-Path $DestinationDir $Name) -Recurse -Force
    }
}

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$buildRoot = Join-Path $repoRoot $BuildDir
$singleConfigExe = Join-Path $buildRoot "CyberDeckBrowser.exe"
$multiConfigExe = Join-Path (Join-Path $buildRoot $Configuration) "CyberDeckBrowser.exe"

if (Test-Path -LiteralPath $multiConfigExe) {
    $runtimeDir = Split-Path -Parent $multiConfigExe
} elseif (Test-Path -LiteralPath $singleConfigExe) {
    $runtimeDir = Split-Path -Parent $singleConfigExe
} else {
    throw "CyberDeckBrowser.exe was not found in '$buildRoot'. Run scripts\build_release.ps1 first."
}

$stageRoot = Join-Path $repoRoot $PackageDir
$appStage = Join-Path $stageRoot "app"
$outputPath = Join-Path $repoRoot $OutputDir
$issPath = Join-Path $repoRoot "installer\CyberDeckBrowser.iss"

if (!(Test-Path -LiteralPath $issPath)) {
    throw "Installer script not found: $issPath"
}

Write-Host "Preparing installer staging directory..."
if (Test-Path -LiteralPath $stageRoot) {
    Remove-Item -LiteralPath $stageRoot -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $appStage | Out-Null
New-Item -ItemType Directory -Force -Path $outputPath | Out-Null

Copy-Item -LiteralPath (Join-Path $runtimeDir "CyberDeckBrowser.exe") -Destination $appStage -Force

foreach ($pattern in @("*.dll", "*.pak", "*.dat", "*.bin", "*.json")) {
    Copy-FilePattern -SourceDir $runtimeDir -Pattern $pattern -DestinationDir $appStage
}

foreach ($helperExe in @("chrome_crashpad_handler.exe")) {
    $helperPath = Join-Path $runtimeDir $helperExe
    if (Test-Path -LiteralPath $helperPath -PathType Leaf) {
        Copy-Item -LiteralPath $helperPath -Destination $appStage -Force
    }
}

foreach ($runtimeFolder in @("locales", "swiftshader", "resources", "angledata", "MEIPreload", "WidevineCdm")) {
    Copy-DirectoryIfPresent -SourceDir $runtimeDir -Name $runtimeFolder -DestinationDir $appStage
}

foreach ($file in @("LICENSE", "THIRD_PARTY_NOTICES.md", "README.md")) {
    $source = Join-Path $repoRoot $file
    if (Test-Path -LiteralPath $source) {
        Copy-Item -LiteralPath $source -Destination $appStage -Force
    }
}

$assets = Join-Path $repoRoot "assets"
if (Test-Path -LiteralPath $assets) {
    Copy-Item -LiteralPath $assets -Destination (Join-Path $appStage "assets") -Recurse -Force
}

$cefMarker = Join-Path $appStage "libcef.dll"
if (!(Test-Path -LiteralPath $cefMarker)) {
    Write-Warning "libcef.dll was not found in the staged app. This package will install the placeholder/non-CEF build unless the build output already contains copied CEF runtime files."
}

if ($SkipCompile) {
    Write-Host "Installer staging complete: $appStage"
    exit 0
}

$iscc = Resolve-Iscc -ExplicitPath $IsccPath

Write-Host "Compiling installer with Inno Setup..."
& $iscc `
    "/DSourceDir=$appStage" `
    "/DOutputDir=$outputPath" `
    $issPath

Write-Host "Installer output directory: $outputPath"
