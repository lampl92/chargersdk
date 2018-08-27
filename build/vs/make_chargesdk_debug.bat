SET source=".\chargesdk\Debug\chargesdk.bin"
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
set /p _ver=<"..\..\src\app\evse_version.h"
set ver=%_ver:~21,10%
echo %ver%
set maj=%ver:~0,2%
set min=%ver:~3,2%
set patch=%ver:~6,4%
set folder3="%maj%.%min%.%patch%"
for /f "delims=" %%t in ('%crc32sum% -r %source%') do set _crc32=%%t
set crc32=%_crc32:~32,8%
md .\Debug\chargesdk\%folder1%\%folder2%\%maj%.%min%.%patch%\upgrade
copy /y %source% .\Debug\chargesdk\%folder1%\%folder2%\%folder3%\upgrade\new_fw_%crc32%