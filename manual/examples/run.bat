@ECHO off
IF "(env) " neq "%PROMPT:~0,6%" ECHO Please run setup.bat first && EXIT /b

python examples.py
