C:\SYSGCC\ARM-EABI\BIN\rgw-make clean
C:\SYSGCC\ARM-EABI\BIN\rgw-make -j5
SET source=".\Release\chargesdk.bin"
set crc32sum=..\..\..\tools\crc32sum.exe
set dd=%date:~8,2%
set mm=%date:~5,2%
set yy=%date:~0,4%
set Tss=%TIME:~6,2%
set Tmm=%TIME:~3,2%
set Thh=%TIME:~0,2%
set Thh=%Thh: =0%
set folder1="%yy%.%mm%.%dd%"
set folder2="%Thh%.%Tmm%.%Tss%"
for /f "delims=" %%t in ('..\..\..\tools\crc32sum.exe -r .\Release\chargesdk.bin') do set _crc32=%%t
set crc32=%_crc32:~24,8%
md ..\Release\chargesdk\%folder1%\%folder2%
copy /y %source% ..\Release\chargesdk\%folder1%\%folder2%\chargesdk.bin.new
copy /y %source% ..\Release\chargesdk\%folder1%\%folder2%\01%crc32%
pause