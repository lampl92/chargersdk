# 组件修改记录

+ STM32F4xx_HAL_Driver

复制 STM32F4xx_HAL_Driver\Inc\stm32f4xx_hal_conf_template.h 为stm32f4xx_hal_conf.h

# 使用工程需确认以下工作
约定printf函数只能在CLI中使用
## 1. 编译器宏定义定义

### MCU

+ STM32F429xx

### HAL
+ USE_HAL_DRIVER

# Components版本

| HAL         | version | ref                                      |
| ----------- | ------- | ---------------------------------------- |
| STM32CubeF4 | 1.16.0  | http://www.st.com/en/embedded-software/stm32cubef4.html |
| FreeRTOS    | 9.0.0   |                                          |
| fatfs       | 0.12b   |                                          |
| STemWin     | 5.32    |                                          |
| lua         | 5.3.3   |                                          |
| tinysh      | 0.1.0   |                                          |
|             |         |                                          |
|             |         |                                          |

