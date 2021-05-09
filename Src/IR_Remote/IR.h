#pragma once

#include "tim.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "gpio.h"
#include "SEGGER_RTT.h"
#include "gpio.h"


extern TaskHandle_t IR_RemoteTaskHandle;


void IR_Remote_Init(void);
