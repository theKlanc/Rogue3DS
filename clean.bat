@echo off
for %%* in (.) do set ProjectName=%%~n*
echo %ProjectName%
make clean
rm -fr %ProjectName%.3ds %ProjectName%.cia %ProjectName%.3dsx %ProjectName%.smdh %ProjectName%.elf
pause