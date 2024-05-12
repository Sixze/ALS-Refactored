@echo off
SETLOCAL

:: 检查git命令是否可用
git --version >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
  echo Git is not installed. Please install Git first.
  exit /b
)

:: 打开Git Bash并执行合并命令
echo Merging from upstream...
start "" "D:\Software\Git\git-bash.exe" -c "git fetch upstream && git merge upstream/main && exec bash"

ENDLOCAL