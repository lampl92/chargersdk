cd .\bootldr
call make_clean.bat
call make.bat
cd ..
SET source=".\bootldr\Release\bootldr.bin"
set crc32sum=..\..\tools\crc32sum.exe
set dd=%date:~8,2%
set mm=%date:~5,2%
set yy=%date:~0,4%
set Tss=%TIME:~6,2%
set Tmm=%TIME:~3,2%
set Thh=%TIME:~0,2%
set Thh=%Thh: =0%
set folder1="%yy%.%mm%.%dd%"
set folder2="%Thh%.%Tmm%.%Tss%"
for /f "delims=" %%t in ('%crc32sum% -r %source%') do set _crc32=%%t
set crc32=%_crc32:~30,8%

md .\Release\bootldr\%folder1%\
copy /y %source% .\Release\bootldr\%folder1%\bootldr.bin.%crc32%
pause