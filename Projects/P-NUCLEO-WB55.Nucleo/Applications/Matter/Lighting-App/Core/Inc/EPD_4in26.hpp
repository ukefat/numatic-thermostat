#pragma once

#include "DEV_Config.h"
#include "stm32wbxx_hal.h"
#include "stm32wbxx_hal_gpio.h"
#include <stdint.h>
#include <stdio.h>
#include <cstring>

#define RST_Pin GPIO_PIN_4
#define RST_GPIO_Port GPIOA
#define DC_Pin GPIO_PIN_9
#define DC_GPIO_Port GPIOA
#define BUSY_Pin GPIO_PIN_14
#define BUSY_GPIO_Port GPIOB
#define SPI_CS_Pin GPIO_PIN_15
#define SPI_CS_GPIO_Port GPIOA
#define PWR_Pin GPIO_PIN_6
#define PWR_GPIO_Port GPIOC
#define DIN_Pin GPIO_PIN_7
#define DIN_GPIO_Port GPIOA
#define SCK_Pin GPIO_PIN_5
#define SCK_GPIO_Port GPIOA

//#define RST_Pin GPIO_PIN_5
//#define RST_GPIO_Port GPIOA
//#define DC_Pin GPIO_PIN_9
//#define DC_GPIO_Port GPIOA
//#define BUSY_Pin GPIO_PIN_8
//#define BUSY_GPIO_Port GPIOA
//#define SPI_CS_Pin GPIO_PIN_3
//#define SPI_CS_GPIO_Port GPIOA
//#define PWR_Pin GPIO_PIN_6
//#define PWR_GPIO_Port GPIOA
//#define DIN_Pin GPIO_PIN_7
//#define DIN_GPIO_Port GPIOA
//#define SCK_Pin GPIO_PIN_1
//#define SCK_GPIO_Port GPIOA

class EPD_4in26 {

public:


	EPD_4in26();
	EPD_4in26(GPIO_TypeDef* rstPort, uint16_t rstPin,
		GPIO_TypeDef* dcPort, uint16_t dcPin,
		GPIO_TypeDef* csPort, uint16_t csPin,
		GPIO_TypeDef* busyPort, uint16_t busyPin,
		GPIO_TypeDef* pwrPort, uint16_t pwrPin,
		SPI_HandleTypeDef* spiHandle
	);

    int pinInit();
    void pinExit();

	void EPD_4in26_Reset(void);

	void EPD_4in26_SendCommand(UBYTE Reg);

	void EPD_4in26_SendData(UBYTE Data);

	void EPD_4in26_SendData2(UBYTE *pData, UDOUBLE len);

	void EPD_4in26_ReadBusy(void);

	void EPD_4in26_TurnOnDisplay(void);

	void EPD_4in26_TurnOnDisplay_Fast(void);

	void EPD_4in26_TurnOnDisplay_Part(void);

	void EPD_4in26_TurnOnDisplay_4GRAY(void);

	void EPD_4in26_Lut(void);

	void EPD_4in26_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend);

	void EPD_4in26_SetCursor(UWORD Xstart, UWORD Ystart);

	void EPD_4in26_Init(void);

	void EPD_4in26_Init_Fast(void);

	void EPD_4in26_Init_4GRAY(void);

	void EPD_4in26_Clear(void);

	void EPD_4in26_Display(UBYTE *Image);

	void EPD_4in26_Display_Base(const UBYTE *Image);

	void EPD_4in26_Display_Fast(const UBYTE *Image);

	void EPD_4in26_4GrayDisplay(const UBYTE *Image);

	void EPD_4in26_Sleep(void);

//	void EPD_4in26_Init_Display_Partial(const UBYTE *Image);
	void EPD_4in26_Display_Part(const UBYTE *Image, UWORD x, UWORD y, UWORD w, UWORD l);
	void EPD_4in26_Display_Part2(const UBYTE *Image, UWORD x, UWORD y, UWORD w, UWORD l);

	void EPD_4in26_WriteToBuffer(const UBYTE *Image, UWORD x, UWORD y, UWORD w, UWORD l);

	void setRefreshChanges(UWORD x, UWORD y, UWORD w, UWORD l);


private:
	static const unsigned char LUT_DATA_4Gray[];

    GPIO_TypeDef* rstPort;
    const uint16_t rstPin;
    GPIO_TypeDef* dcPort;
    const uint16_t dcPin;
    GPIO_TypeDef* csPort;
    const uint16_t csPin;
    GPIO_TypeDef* busyPort;
    const uint16_t busyPin;
    GPIO_TypeDef* pwrPort;
	const uint16_t pwrPin;
//    SPI_HandleTypeDef* spiHandle;

    SPI_HandleTypeDef* spi;

};



