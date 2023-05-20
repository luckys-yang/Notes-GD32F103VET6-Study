@echo off

set UV=C:\Keil_V5\UV4\UV4.exe
set UV_PRO_FILE="uvprojx"
set UV_PRO_DIR=%cd%

for /f "usebackq delims=" %%j in (`dir /b "%UV_PRO_DIR%"\*."%UV_PRO_FILE%"`) do (
if exist %%j (
set UV_PRO_FILE_PATH="%UV_PRO_DIR%\%%j"))

echo %UV%
echo %UV_PRO_DIR%
echo %UV_PRO_FILE_PATH%

echo Init building ...
echo .>build_log.txt
%UV% -j0 -r %UV_PRO_FILE_PATH% -o %cd%\build_log.txt
type build_log.txt
echo Done.
pause