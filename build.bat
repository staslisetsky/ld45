@echo off

set ProjectDir="P:\\ls\\ld46"

IF NOT EXIST %ProjectDir%\_build mkdir %ProjectDir%\_build
pushd %ProjectDir%\_build

rem set Variables=-DPLATFORM_WINDOWS=1
set Common=-TP -MT -FC -Zi -nologo -wd4700 -wd4312 -wd4311 -wd4530 -Od

rem Font Asset Builder
rem cl /std:c++14 /Fefontgen ^
rem     %Common% ^
rem     /I %ProjectDir%\src\include\ft ^
rem     %ProjectDir%\src\fontgen.cpp ^
rem     /link %ProjectDir%\src\include\ft\freetype265ST.lib

rem Compile App:
cl /TP /std:c++14 /Fegame ^
    /I %ProjectDir%\src\include\openal ^
    %Common% ^
    %ProjectDir%\src\windows_main.cpp ^
    /link -SUBSYSTEM:console User32.lib Gdi32.lib Opengl32.lib %ProjectDir%\src\include\openal\OpenAl32.lib
popd
