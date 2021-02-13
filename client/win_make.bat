@echo off

set build_dir=build
set debug_dir=debug

if "%1" == "debug" (goto :debug)
if "%1" == "clean" (goto :clean)

:build
cmake . -B.\%build_dir% -DCMAKE_BUILD_TYPE=Release
copy mycert.pem .\%build_dir%
goto :eof

:debug
cmake . -B.\%debug_dir% -DCMAKE_BUILD_TYPE=Debug
copy mycert.pem .\%debug_dir%
goto :eof

:clean
rmdir /s /q .\%build_dir%
rmdir /s /q .\%debug_dir%
