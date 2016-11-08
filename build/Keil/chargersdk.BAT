SET PATH=C:\Keil_v5\ARM\ARMCC\bin\;C:\ProgramData\Oracle\Java\javapath;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;C:\windows\system32;C:\windows;C:\windows\System32\Wbem;C:\windows\System32\WindowsPowerShell\v1.0\;C:\Program Files (x86)\Common Files\Autodesk Shared\;D:\Program Files (x86)\Altium Designer 6\System;C:\Users\DT-553\AppData\Local\Pandoc\
SET CPU_TYPE=STM32F429ZG
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=chargersdk
SET CPU_CLOCK=0x017D7840
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\output\main.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\output\stm32f4xx_it.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\output\bsp.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmAsm" --Via ".\output\startup_stm32f429xx._ia"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\output\system_stm32f4xx.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\output\stm32f4xx_hal_gpio.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\output\stm32f4xx_hal_rcc.__i"
