#ifndef EVSE_DEBUG_H
#define EVSE_DEBUG_H

#include "retarget.h"
#include "stm32f4xx.h"

extern uint8_t protolog;
extern uint8_t dePrintTime;

#define printf_protolog(...) do{if(protolog > 0)printf_safe(__VA_ARGS__);}while(0);
#define printf_protodetail(...) do{if(protolog > 1)printf_safe(__VA_ARGS__);}while(0);

#endif