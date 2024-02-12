echo off
:start
cls
plink -serial COM3 -sercfg 115200,8
timeout /t 1
goto start