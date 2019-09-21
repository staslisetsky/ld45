@echo off

set ProjectDir="P:\\ls\\ld45"

IF NOT EXIST %ProjectDir%\_build mkdir %ProjectDir%\_build
pushd %ProjectDir%\_build

rem set Variables=-DPLATFORM_WINDOWS=1
set Common=-TP -FC -Zi -nologo -wd4700 -wd4312 -wd4311 -wd4530 -Od

rem Compile App:
cl  /std:c++14 /Fegame ^
    %Common% ^
    %ProjectDir%\src\windows_main.cpp ^
    /link User32.lib Gdi32.lib Opengl32.lib
popd
