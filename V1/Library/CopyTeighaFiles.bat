@echo off
set debugSource=.\Teigha\exe\vc17_amd64dlldbg
set debugTarget=..\Build\3DMI.Debug64
set releaseSource=.\Teigha\exe\vc17_amd64dll
set releaseTarget=..\Build\3DMI.64

for /f "tokens=*" %%i in (TeighaFiles.txt) do (
    del "%debugTarget%\%%i"
    del "%releaseTarget%\%%i"
    xcopy /s/e "%debugSource%\%%i" "%debugTarget%"
    xcopy /s/e "%releaseSource%\%%i" "%releaseTarget%"
)
