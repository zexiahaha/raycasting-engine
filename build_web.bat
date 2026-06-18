@echo off
if not exist .\build_web mkdir .\build_web
pushd .\build_web

emcc ..\src\main.c ..\vendor\raylib-web\lib\libraylib.web.a -D PLATFORM_WEB -I..\vendor\raylib-web\include -s USE_GLFW=3 -s ASYNCIFY --shell-file ..\shell.html -o index.html

popd