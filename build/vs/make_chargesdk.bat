set path_bootldr=".\bootldr\Release\bootldr.bin"
set path_app=".\chargesdk\Release\chargesdk.bin"
set crc32sum=..\..\tools\crc32sum.exe
set genflist=..\..\tools\genflist\bin\genflist.exe
set frobflist=..\..\tools\frobflist\bin\frobflist.exe

set dd=%date:~8,2%
set mm=%date:~5,2%
set yy=%date:~0,4%
set Tss=%TIME:~6,2%
set Tmm=%TIME:~3,2%
set Thh=%TIME:~0,2%
set Thh=%Thh: =0%
set folder1=%yy%.%mm%.%dd%_%Thh%.%Tmm%.%Tss%

set /p _ver=<"..\..\src\app\evse_version.h"
set ver=%_ver:~21,10%
echo %ver%
set maj=%ver:~0,2%
set min=%ver:~3,2%
set patch=%ver:~6,4%
set folder3=%maj%.%min%.%patch%

for /f "delims=" %%t in ('%crc32sum% -r %path_bootldr%') do set _crc32=%%t
set crc32_bootldr=%_crc32:~30,8%

for /f "delims=" %%t in ('%crc32sum% -r %path_app%') do set _crc32=%%t
set crc32_app=%_crc32:~34,8%

md .\Release\chargesdk\%folder1%\%folder3%\upgrade

copy /y %path_bootldr% .\Release\chargesdk\%folder1%\%folder3%\upgrade\new_boot_%crc32_bootldr%
copy /y %path_bootldr% .\Release\bootldr.bin

copy /y %path_app% .\Release\chargesdk\%folder1%\%folder3%\upgrade\new_fw_%crc32_app%
copy /y %path_app% .\Release\chargesdk.bin

%genflist%  ..\..\..\build\vs\Release\chargesdk\%folder1%\%folder3%\upgrade\new_boot_%crc32_bootldr%
%genflist%  ..\..\..\build\vs\Release\chargesdk\%folder1%\%folder3%\upgrade\new_fw_%crc32_app%

move /y ..\..\tools\genflist\bin\flist.json .\Release\chargesdk\%folder1%\%folder3%
%frobflist% .\Release\chargesdk\%folder1%\%folder3%\flist.json