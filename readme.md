# 组件修改记录

+ STM32F4xx_HAL_Driver

复制 STM32F4xx_HAL_Driver\Inc\stm32f4xx_hal_conf_template.h 为stm32f4xx_hal_conf.h

# 使用工程需确认以下工作

## 1. 编译器宏定义定义

### MCU

+ STM32F429xx

### HAL
+ USE_HAL_DRIVER

# Components版本

| Components  | version        | ref                                      |
| ----------- | -------------- | ---------------------------------------- |
| STM32CubeF4 | 1.16.0         | http://www.st.com/en/embedded-software/stm32cubef4.html |
| FreeRTOS    | 10.0.0         | http://www.freertos.org/                 |
| yaffs2      | alpha1-release | https://yaffs.net/                       |
| STemWin     | 5.40           |                                          |
| lua         | 5.3.3          | https://www.lua.org/                     |
| tinysh      | 0.1.0          | https://sourceforge.net/projects/tinysh/ |
| cJSON       | 1.60           | https://github.com/DaveGamble/cJSON      |
| gdsl        | 1.8            | https://directory.fsf.org/wiki/GDSL      |

