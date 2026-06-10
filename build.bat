@echo off

if not exist .\build mkdir .\build
pushd .\build
cl -Zi -MD /I..\vendor\raylib\include ..\src\main.c /link ..\vendor\raylib\lib\raylib.lib user32.lib gdi32.lib opengl32.lib shell32.lib winmm.lib /out:.\raycaster.exe
popd