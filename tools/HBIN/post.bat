del dest.bin
SET source=".\Release"
fsutil  file createnew dest.bin 0x40000
cat 2.bin >>dest.bin
hbin 1.bin dest.bin
pause