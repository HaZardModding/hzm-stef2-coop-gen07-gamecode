@echo off
setlocal EnableDelayedExpansion

set BLACKLIST=__cleanUp_blacklist.txt
set DRYRUN=0
set LOGFILE=__cleanup_delete.log
if exist %LOGFILE% del /f /q %LOGFILE%

if not exist %BLACKLIST% (
    echo blacklist.txt not found!
    exit /b 1
)

for /f "usebackq tokens=* delims=" %%A in (%BLACKLIST%) do (
    set "LINE=%%A"
    setlocal EnableDelayedExpansion
    set "LINE=!LINE: =!"

    if defined LINE (
        if not "!LINE:~0,1!"=="#" if not "!LINE:~0,1!"=="!" (
            set "PATTERN=!LINE:/=\!"
            if "!PATTERN:~-1!"=="\" set "PATTERN=!PATTERN:~0,-1!"

            for /r %%F in (!PATTERN!) do (
                set "TARGET=%%~fF"
                if exist "!TARGET!\" (
                    if !DRYRUN! == 1 (
                        echo [DRY-RUN] Would delete directory: !TARGET!
                        echo [DRY-RUN] Would delete directory: !TARGET! >> %LOGFILE%
                    ) else (
                        rmdir /s /q "!TARGET!"
                        echo Deleted directory: !TARGET! >> %LOGFILE%
                    )
                ) else if exist "!TARGET!" (
                    if !DRYRUN! == 1 (
                        echo [DRY-RUN] Would delete file: !TARGET!
                        echo [DRY-RUN] Would delete file: !TARGET! >> %LOGFILE%
                    ) else (
                        del /f /q "!TARGET!"
                        echo Deleted file: !TARGET! >> %LOGFILE%
                    )
                )
            )
        )
    )
    endlocal
)

echo Cleanup complete. See %LOGFILE% for details.
pause
