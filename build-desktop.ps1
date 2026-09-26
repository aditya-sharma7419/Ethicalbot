$ErrorActionPreference = 'Stop'

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $scriptDir

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Host "CMake is not installed or not on PATH. Install Qt + CMake first, then run:"
    Write-Host "  cmake -S . -B build"
    Write-Host "  cmake --build build"
    exit 1
}

cmake -S . -B build
cmake --build build
Write-Host "Desktop app build complete. Run the executable from the build directory."
