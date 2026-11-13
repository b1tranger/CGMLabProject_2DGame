# =========================================================================
# The Wenderer: Complete Web Build & Itch.io Packaging Script
# Compiles C++ to WebAssembly + WebGL 2.0 and packages itch.io release
# =========================================================================

$ErrorActionPreference = "Stop"

$OutputDir = "d:\GitHub\UITS\CGMLabProject_2DGame\TheWenderer_Web"
$CacheDir = "C:\emsdk\cache"

Write-Host "=========================================================================" -ForegroundColor Cyan
Write-Host "Compiling The Wenderer for Web (WebAssembly + WebGL 2.0)..." -ForegroundColor Cyan
Write-Host "=========================================================================" -ForegroundColor Cyan

# 1. Ensure Directories Exist
if (!(Test-Path $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
}
if (!(Test-Path $CacheDir)) {
    New-Item -ItemType Directory -Path $CacheDir -Force | Out-Null
}

# 2. Configure Emscripten Environment
$env:EM_CACHE = $CacheDir
$env:PATH = "C:\emsdk;C:\emsdk\upstream\emscripten;C:\emsdk\node\24.19.0_64bit;C:\emsdk\python\3.13.3_64bit;$env:PATH"
$env:EMSDK = "C:/emsdk"
$env:EMSDK_NODE = "C:\emsdk\node\24.19.0_64bit\node.exe"
$env:EMSDK_PYTHON = "C:\emsdk\python\3.13.3_64bit\python.exe"

$python = "C:\emsdk\python\3.13.3_64bit\python.exe"
$empp = "C:\emsdk\upstream\emscripten\em++.py"

$sourceFiles = @(
    "src/main.cpp",
    "src/Renderer.cpp",
    "src/World.cpp",
    "src/UIManager.cpp",
    "src/LadderScene.cpp",
    "src/SkyFallScene.cpp",
    "src/Audio.cpp",
    "src/VideoPlayer.cpp"
)

$targetHtml = "$OutputDir/index.html"

# Remove old build artifacts in output directory (except .zip if any)
Remove-Item -Path "$OutputDir/index.*" -Force -ErrorAction SilentlyContinue

Write-Host "Invoking em++ with optimizations (-O3, WebGL 2.0, GLFW 3)..." -ForegroundColor Yellow

& $python $empp `
    $sourceFiles `
    -Iinclude `
    -std=c++17 `
    -O3 `
    -s USE_GLFW=3 `
    -s MIN_WEBGL_VERSION=2 `
    -s MAX_WEBGL_VERSION=2 `
    -s ALLOW_MEMORY_GROWTH=1 `
    -s INITIAL_MEMORY=67108864 `
    --preload-file assets `
    --shell-file shell_minimal.html `
    -o $targetHtml

if ($LASTEXITCODE -ne 0) {
    Write-Error "Emscripten compilation failed with exit code $LASTEXITCODE."
    exit $LASTEXITCODE
}

Write-Host "[SUCCESS] WebAssembly and WebGL 2.0 build generated!" -ForegroundColor Green

# 3. Create itch.io Distribution ZIP
$zipPath = "$OutputDir/TheWenderer_Web_itch.zip"
if (Test-Path $zipPath) {
    Remove-Item $zipPath -Force
}

Write-Host "Creating itch.io distribution ZIP: $zipPath..." -ForegroundColor Yellow

$itemsToZip = Get-ChildItem -Path $OutputDir -Exclude "*.zip"
Compress-Archive -Path $itemsToZip.FullName -DestinationPath $zipPath -Force

Write-Host "=========================================================================" -ForegroundColor Green
Write-Host "BUILD COMPLETE!" -ForegroundColor Green
Write-Host "Web build directory: $OutputDir" -ForegroundColor White
Write-Host "itch.io ZIP package: $zipPath" -ForegroundColor White
Write-Host "=========================================================================" -ForegroundColor Green
