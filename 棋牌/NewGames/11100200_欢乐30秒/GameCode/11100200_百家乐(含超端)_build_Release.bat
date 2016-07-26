@echo off

echo./*
echo.* Check VC++ environment...
echo.*/
echo.

if defined VS110COMNTOOLS (
    set VSVARS="%VS110COMNTOOLS%vsvars32.bat"
    set VC_VER=110
) else if defined VS100COMNTOOLS (
    set VSVARS="%VS100COMNTOOLS%vsvars32.bat"
    set VC_VER=100
)

if not defined VSVARS (
    echo Can't find VC2010 or VC2012 installed!
    goto ERROR
)

echo./*
echo.* Building 11100200_∞Ÿº“¿÷(∫¨≥¨∂À), please wait a while...
echo.*/
echo.

call %VSVARS%
if %VC_VER%==100 (
    msbuild 11100200_BJL_2010.sln /t:Clean
    msbuild 11100200_BJL_2010.sln /p:Configuration="Release" /m
) else if %VC_VER%==110 (
    msbuild 11100200_BJL_2010.sln /t:Clean
    msbuild 11100200_BJL_2010.sln /p:Configuration="Release" /m
) else (
    echo Script error.
    goto ERROR
)




:ERROR
pause

:EOF