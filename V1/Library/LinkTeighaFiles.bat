@echo off
set debugSource=%cd%\Teigha\exe\vc17_amd64dlldbg
set debugTarget=%cd%\..\Build\3DMI.Debug64
set releaseSource=%cd%\Teigha\exe\vc17_amd64dll
set releaseTarget=%cd%\..\Build\3DMI.64

for /f "tokens=*" %%i in (TeighaFiles.txt) do (
    del "%debugTarget%\%%i"
    del "%releaseTarget%\%%i"
    mklink "%debugTarget%\%%i" "%debugSource%\%%i"
    mklink "%releaseTarget%\%%i" "%releaseSource%\%%i"
)
