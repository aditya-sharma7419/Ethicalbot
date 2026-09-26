$ErrorActionPreference = 'Stop'

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $scriptDir

$buildDir = Join-Path $scriptDir 'build'
$exeCandidates = @(
    (Join-Path $buildDir 'AvastSmartCleaner.exe'),
    (Join-Path $buildDir 'Release\AvastSmartCleaner.exe'),
    (Join-Path $buildDir 'Debug\AvastSmartCleaner.exe')
)

$targetExe = $exeCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1

if (-not $targetExe) {
    if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
        Write-Host "Qt + CMake are not installed or not on PATH. Install them, then run this launcher again."
        exit 1
    }

    Write-Host "Building the desktop app..."
    cmake -S . -B build
    cmake --build build --config Release

    $targetExe = $exeCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1
}

if (-not $targetExe) {
    Write-Host "Desktop app build was not found. Check the Qt build output and ensure CMake configured correctly."
    exit 1
}

Write-Host "Launching Secure Shield Cleaner..."
& $targetExe
