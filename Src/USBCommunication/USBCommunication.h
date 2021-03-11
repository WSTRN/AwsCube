#pragma once


#include "gpio.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

static TaskHandle_t USBTaskHandle = NULL;
void USBTask(void const * argument);

void usb_communication_init();