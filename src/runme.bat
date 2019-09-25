@echo off
setlocal
SET CurrentDir="%~dp0"
cd /d "%CurrentDir%"


"bin\Debug\BGchanger.exe" /show /exit
pause

endlocal