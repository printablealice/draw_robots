@echo off
if not exist build mkdir build
set tmp=%cd%
pushd build
cls
cl /nologo -Od -fp:except -Zi -GR- -EHa- -W4 -FC -wd4201 -wd4127 /MD "%tmp%"\main.cpp /link OpenGL32.lib user32.lib gdi32.lib shell32.lib vcruntime.lib msvcrt.lib ..\windows_ext\glfw\lib-vc2017\glfw3.lib
popd
