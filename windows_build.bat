@echo off
if not exist build mkdir build
set tmp=%cd%
pushd build
cls

cl /nologo /I ext\glfw\include /I ext\glad\include ^
-fp:except ^
-Od ^
-Zi -GR- -EHa- -W4 -FC -wd4201 -wd4505 -wd4127 -wd4723 -wd4238 /MD %tmp%\main.cpp /link ..\windows_ext\glfw\lib-vc2017\glfw3.lib OpenGL32.lib user32.lib gdi32.lib shell32.lib vcruntime.lib msvcrt.lib

popd
