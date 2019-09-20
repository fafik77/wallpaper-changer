@echo off
setlocal
SET CurrentDir="%~dp0"
cd /d "%CurrentDir%"


"bin\Debug\BGchanger.exe" /show Log.n+
pause

endlocal