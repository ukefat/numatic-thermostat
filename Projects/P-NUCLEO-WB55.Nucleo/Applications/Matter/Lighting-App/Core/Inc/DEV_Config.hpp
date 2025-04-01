#pragma once

#include "main.h"

#include <stdint.h>
#include <stdio.h>

#ifdef RTOS
#include "FreeRTOS.h"
#include "task.h"
#endif
/**
 * data


/**
 * e-Paper GPIO
**/
//#define EPD_RST_PIN     RST_GPIO_Port, RST_Pin
//#define EPD_DC_PIN      DC_GPIO_Port, DC_Pin
//#define EPD_PWR_PIN     PWR_GPIO_Port, PWR_Pin
//#define EPD_CS_PIN      SPI_CS_GPIO_Port, SPI_CS_Pin
//#define EPD_BUSY_PIN    BUSY_GPIO_Port, BUSY_Pin
//#define EPD_MOSI_PIN    DIN_GPIO_Port, DIN_Pin
//#define EPD_SCLK_PIN    SCK_GPIO_Port, SCK_Pin

/**
 * GPIO read and write
**/
//#define DEV_Digital_Write(_pin, _value) HAL_GPIO_WritePin(_pin, _value == 0? GPIO_PIN_RESET:GPIO_PIN_SET)
//#define DEV_Digital_Read(_pin) HAL_GPIO_ReadPin(_pin)

/**
 * delay x ms
**/
#define DEV_Delay_ms(__xms) vTaskDelay(__xms);

//void DEV_SPI_WriteByte(UBYTE value);
//void DEV_SPI_Write_nByte(UBYTE *value, UDOUBLE len);

int DEV_Module_Init(void);
void DEV_Module_Exit(void);
void DEV_GPIO_Init(void);
void DEV_SPI_Init(void);
void DEV_SPI_SendData(UBYTE Reg);
UBYTE DEV_SPI_ReadData(void);
