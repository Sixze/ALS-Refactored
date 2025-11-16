@echo off

title Build Plugin

set SourceBuildVersion=5.7
set InstalledBuildVersion=5.7

for /f "skip=2 tokens=2*" %%a in ('reg query "HKEY_CURRENT_USER\Software\Epic Games\Unreal Engine\Builds" /v "%SourceBuildVersion%"') do (
    set EngineDirectory=%%b
)

if "%EngineDirectory%" == "" (
    for /f "skip=2 tokens=2*" %%a in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\EpicGames\Unreal Engine\%InstalledBuildVersion%" /v "InstalledDirectory"') do (
        set EngineDirectory=%%b
    )
)

if "%EngineDirectory%" == "" (
    cls

    echo Can't find a path to the engine! Make sure at least one of the following registry entries exists:
    echo:
    echo For a source build:
    echo    Key:   HKEY_CURRENT_USER\Software\Epic Games\Unreal Engine\Builds
    echo    Value: %SourceBuildVersion%
    echo:
    echo For an installed build:
    echo    Key:   HKEY_LOCAL_MACHINE\SOFTWARE\EpicGames\Unreal Engine\%InstalledBuildVersion%
    echo    Value: InstalledDirectory
    echo:

    pause
    exit 0
)

set AutomationToolPath=%EngineDirectory%\Engine\Build\BatchFiles\RunUAT.bat
set PluginPath=%cd%\ALS.uplugin
set OutputPath=%cd%\Build

echo Automation Tool Path: %AutomationToolPath%
echo:

call "%AutomationToolPath%" BuildPlugin -Plugin="%PluginPath%" -Package="%OutputPath%" -Rocket -TargetPlatforms=Win64
echo:
pause
exit 0
