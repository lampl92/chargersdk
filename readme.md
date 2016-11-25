# 组件修改记录

+ STM32F4xx_HAL_Driver

复制 STM32F4xx_HAL_Driver\Inc\stm32f4xx_hal_conf_template.h 为stm32f4xx_hal_conf.h

# 使用工程需确认以下工作
约定printf函数只能在CLI中使用
## 1. 编译器宏定义定义

### MCU

+ STM32F407xx
+ STM32F429xx

### HAL
+ USE_HAL_DRIVER

## 1. 使用MDK时,不选择使用use MicroLib

# Components版本

|HAL					|	version		|ref|
|---------------		|---------------|---|
|CMSIS					|				|STM32Cube_FW_F4_V1.13.1
|CMSIS_RTOS				|				|STM32Cube_FW_F4_V1.13.1\Middlewares\Third_Party\FreeRTOS\Source\CMSIS_RTOS
|STM32F4xx_HAL_Driver	|				|STM32Cube_FW_F4_V1.13.1


# 组件版本

|component		|	version		|ref|
|---------------|---------------|---|
|FreeRTOS 		|	9.0.0		|
|lwip			|	1.4.1		|
|fatfs			|	0.12b		|[fatfs](http://elm-chan.org/fsw/ff/00index_e.html)
|STemWin		|	5.32		|STM32Cube_FW_F4_V1.13.1
|lua			|	5.3.3		|
|jsmn			|		
|tinysh			|	0.1.0		|
|sqlite			|	3.15.1		|
