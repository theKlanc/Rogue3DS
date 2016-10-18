@echo off
for %%* in (.) do set ProjectName=%%~n*
echo %ProjectName%
make -f 3dsmakefile.mk
:: D:\devkitPro\devkitARM\arm-none-eabi\bin\strip %ProjectName%.elf
resources\makerom.exe -f cia -o %ProjectName%.cia -elf %ProjectName%.elf -rsf resources\build_cia.rsf -icon resources\icon.bin -banner resources\banner.bin -exefslogo -target t
pause