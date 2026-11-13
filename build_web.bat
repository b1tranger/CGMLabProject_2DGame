@echo off
REM =========================================================================
REM The Wenderer: WebAssembly + WebGL 2 Compilation Script via Emscripten
REM Delegates to build_web.ps1 for environment configuration and itch packaging
REM =========================================================================

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build_web.ps1"

