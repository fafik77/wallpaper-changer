@echo off
setlocal
SET CurrentDir="%~dp0"
cd /d "%CurrentDir%"


"BGchanger -- Cmd.lnk" /show ImgOrder.n+

pause
endlocal