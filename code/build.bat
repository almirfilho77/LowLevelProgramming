@echo off

mkdir ..\build
pushd ..\build
cl /Zi /FC ..\code\win32_entrypoint.cpp user32.lib Gdi32.lib
popd