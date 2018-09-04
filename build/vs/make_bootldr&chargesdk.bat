call make_bootldr.bat
call make_chargesdk.bat
SET source=.\Release
del %source%\bootldr^&chargesdk.bin
copy ..\..\tools\HBIN\blank256.bin %source%\bootldr^&chargesdk.bin
..\..\tools\HBIN\cat %source%\chargesdk.bin>>%source%\bootldr^&chargesdk.bin
..\..\tools\HBIN\hbin %source%\bootldr.bin %source%\bootldr^&chargesdk.bin
del %source%\bootldr.bin
del %source%\chargesdk.bin