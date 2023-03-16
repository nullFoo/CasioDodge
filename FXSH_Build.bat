@echo off
rem Do not edit! This batch file is created by CASIO fx-9860G SDK.


if exist DODGE.G1A  del DODGE.G1A

cd debug
if exist FXADDINror.bin  del FXADDINror.bin
"D:\_Extra Space\CalculatorProjects\Installation\CASIO\fx-9860G SDK\OS\SH\Bin\Hmake.exe" Addin.mak
cd ..
if not exist debug\FXADDINror.bin  goto error

"D:\_Extra Space\CalculatorProjects\Installation\CASIO\fx-9860G SDK\Tools\MakeAddinHeader363.exe" "D:\_Extra Space\CalculatorProjects\Dodge"
if not exist DODGE.G1A  goto error
echo Build has completed.
goto end

:error
echo Build was not successful.

:end

