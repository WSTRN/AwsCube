#pragma once

#include "ButtonEvent.h"
#include "tim.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "gpio.h"
#include "SEGGER_RTT.h"


extern TaskHandle_t EncoderTaskHandle;


void Encoder_Init(void);