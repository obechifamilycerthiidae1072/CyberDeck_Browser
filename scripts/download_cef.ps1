param(
    [Parameter(Mandatory = $true)]
    [string]$CefUrl,

    [string]$Destination = "third_party"
)

$ErrorActionPreference = "Stop"

$root = Resolve-Path -LiteralPath (Join-Path $PSScriptRoot "..")
$destinationPath = Join-Path $root $Destination
New-Item -ItemType Directory -Force -Path $destinationPath | Out-Null

$archiveName = Split-Path -Leaf ([System.Uri]$CefUrl).AbsolutePath
if (-not $archiveName) {
    throw "Could not determine archive name from URL."
}

$archivePath = Join-Path $destinationPath $archiveName

Write-Host "Downloading CEF from $CefUrl"
Invoke-WebRequest -Uri $CefUrl -OutFile $archivePath

Write-Host "Downloaded to $archivePath"
Write-Host "Extract the archive under third_party, then configure with:"
Write-Host "  cmake -S . -B build -DCEF_ROOT=<path-to-extracted-cef>"
