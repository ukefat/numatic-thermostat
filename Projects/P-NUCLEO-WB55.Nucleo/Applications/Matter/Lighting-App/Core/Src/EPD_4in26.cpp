
#include <EPD_4in26.hpp>
#include "Debug.h"

#ifdef RTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
extern SemaphoreHandle_t xEPaperBusySemaphoreHandle;
#endif
//#inl

extern SPI_HandleTypeDef hspi1;

#define RST_Pin GPIO_PIN_5
#define RST_GPIO_Port GPIOA
#define DC_Pin GPIO_PIN_9
#define DC_GPIO_Port GPIOA
#define BUSY_Pin GPIO_PIN_8
#define BUSY_GPIO_Port GPIOA
#define SPI_CS_Pin GPIO_PIN_3
#define SPI_CS_GPIO_Port GPIOA
#define PWR_Pin GPIO_PIN_6
#define PWR_GPIO_Port GPIOA
#define DIN_Pin GPIO_PIN_7
#define DIN_GPIO_Port GPIOA
#define SCK_Pin GPIO_PIN_1
#define SCK_GPIO_Port GPIOA

EPD_4in26::EPD_4in26():
	rstPort(RST_GPIO_Port), rstPin(RST_Pin),
	dcPort(DC_GPIO_Port), dcPin(DC_Pin),
	csPort(SPI_CS_GPIO_Port), csPin(SPI_CS_Pin),
	busyPort(BUSY_GPIO_Port), busyPin(BUSY_Pin),
	pwrPort(PWR_GPIO_Port), pwrPin(PWR_Pin),
	spi(&hspi1)
{}

EPD_4in26::EPD_4in26(GPIO_TypeDef* rstPort, uint16_t rstPin,
		GPIO_TypeDef* dcPort, uint16_t dcPin,
		GPIO_TypeDef* csPort, uint16_t csPin,
		GPIO_TypeDef* busyPort, uint16_t busyPin,
		GPIO_TypeDef* pwrPort, uint16_t pwrPin,
		SPI_HandleTypeDef* spiHandle):
	rstPort(rstPort), rstPin(rstPin),
	dcPort(dcPort), dcPin(dcPin),
	csPort(csPort), csPin(csPin),
	busyPort(busyPort), busyPin(busyPin),
	pwrPort(pwrPort), pwrPin(pwrPin),
	spi(spiHandle)
{

}
const unsigned char EPD_4in26::LUT_DATA_4Gray[112] =    //112bytes
{
0x80,	0x48,	0x4A,	0x22,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
0x0A,	0x48,	0x68,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
0x88,	0x48,	0x60,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
0xA8,	0x48,	0x45,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
0x07,	0x1E,	0x1C,	0x02,	0x00,
0x05,	0x01,	0x05,	0x01,	0x02,
0x08,	0x01,	0x01,	0x04,	0x04,
0x00,	0x02,	0x00,	0x02,	0x01,
0x00,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x01,
0x22,	0x22,	0x22,	0x22,	0x22,
0x17,	0x41,	0xA8,	0x32,	0x30,
0x00,	0x00,
};


int EPD_4in26::pinInit(void)
{
	HAL_GPIO_WritePin(dcPort, dcPin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(csPort, csPin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(pwrPort, pwrPin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(rstPort, rstPin, GPIO_PIN_SET);
    return 0;
}

void EPD_4in26::pinExit(void)
{
	HAL_GPIO_WritePin(dcPort, dcPin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(csPort, csPin, GPIO_PIN_RESET);

    //close 5V
	HAL_GPIO_WritePin(pwrPort, pwrPin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(rstPort, rstPin, GPIO_PIN_RESET);
}


/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
void EPD_4in26::EPD_4in26_Reset(void)
{
	HAL_GPIO_WritePin(rstPort, rstPin, GPIO_PIN_SET);
	vTaskDelay(100);
	HAL_GPIO_WritePin(rstPort, rstPin, GPIO_PIN_RESET);
    vTaskDelay(2);
	HAL_GPIO_WritePin(rstPort, rstPin, GPIO_PIN_SET);
    vTaskDelay(100);
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
void EPD_4in26::EPD_4in26_SendCommand(UBYTE Reg)
{

	__HAL_SPI_DISABLE(spi);
	HAL_Delay(1);
	__HAL_SPI_ENABLE(spi);

	// Flush the RX buffer manually before starting transmission
	__HAL_SPI_CLEAR_OVRFLAG(spi);

    HAL_GPIO_WritePin(dcPort, dcPin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(csPort, csPin, GPIO_PIN_RESET);

    vTaskDelay(1);

    HAL_SPI_Transmit(spi, &Reg, 1, HAL_MAX_DELAY);

    vTaskDelay(1);

    HAL_GPIO_WritePin(csPort, csPin, GPIO_PIN_SET);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
void EPD_4in26::EPD_4in26_SendData(UBYTE Data)
{
    HAL_GPIO_WritePin(dcPort, dcPin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(csPort, csPin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(spi, &Data, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(csPort, csPin, GPIO_PIN_SET);
}

void EPD_4in26::EPD_4in26_SendData2(UBYTE *pData, UDOUBLE len)
{
    HAL_GPIO_WritePin(dcPort, dcPin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(csPort, csPin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(spi, pData, len, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(csPort, csPin, GPIO_PIN_SET);
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:
******************************************************************************/

//Debug("e-Paper busy\r\n");
//	while(1)
//	{	 //=1 BUSY
//		if(HAL_GPIO_ReadPin(busyPort, busyPin) == GPIO_PIN_RESET)
//			break;
//		DEV_Delay_ms(20);
//	}
//	DEV_Delay_ms(20);
//    Debug("e-Paper busy release\r\n");
void EPD_4in26::EPD_4in26_ReadBusy(void)
{
		while(1)
		{	 //=1 BUSY
			if(HAL_GPIO_ReadPin(busyPort, busyPin) == GPIO_PIN_RESET)
				break;
			vTaskDelay(20);
		}
		vTaskDelay(20);
#if 0
    Debug("e-Paper busy\r\n");
    if (xSemaphoreTake(xEPaperBusySemaphoreHandle, portMAX_DELAY) == pdTRUE)
    {
        Debug("e-Paper busy release\r\n");
    }
    vTaskDelay(20);
//    Debug("e-Paper busy release\r\n");
#endif
}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
void EPD_4in26::EPD_4in26_TurnOnDisplay(void)
{
	EPD_4in26_SendCommand(0x22); //Display Update Control
	EPD_4in26_SendData(0xF7);
	EPD_4in26_SendCommand(0x20); //Activate Display Update Sequence
	EPD_4in26_ReadBusy();
}

void EPD_4in26::EPD_4in26_TurnOnDisplay_Fast(void)
{
	EPD_4in26_SendCommand(0x22); //Display Update Control
	EPD_4in26_SendData(0xC7);
	EPD_4in26_SendCommand(0x20); //Activate Display Update Sequence
	EPD_4in26_ReadBusy();
}

void EPD_4in26::EPD_4in26_TurnOnDisplay_Part(void)
{
	EPD_4in26_SendCommand(0x22); //Display Update Control
	EPD_4in26_SendData(0xFF);
	EPD_4in26_SendCommand(0x20); //Activate Display Update Sequence
	EPD_4in26_ReadBusy();
}

void EPD_4in26::EPD_4in26_TurnOnDisplay_4GRAY(void)
{
    EPD_4in26_SendCommand(0x22);
    EPD_4in26_SendData(0xC7);
	EPD_4in26_SendCommand(0x20);
    EPD_4in26_ReadBusy();
}

/******************************************************************************
function :	set the look-up tables
parameter:
******************************************************************************/
void EPD_4in26::EPD_4in26_Lut(void)
{
    unsigned int count;
    EPD_4in26_SendCommand(0x32); //vcom
    for(count = 0; count < 105 ; count++) {
        EPD_4in26_SendData(LUT_DATA_4Gray[count]);
    }

    EPD_4in26_SendCommand(0x03); //VGH
	EPD_4in26_SendData(LUT_DATA_4Gray[105]);

	EPD_4in26_SendCommand(0x04); //
	EPD_4in26_SendData(LUT_DATA_4Gray[106]); //VSH1
	EPD_4in26_SendData(LUT_DATA_4Gray[107]); //VSH2
	EPD_4in26_SendData(LUT_DATA_4Gray[108]); //VSL

	EPD_4in26_SendCommand(0x2C);     //VCOM Voltage
	EPD_4in26_SendData(LUT_DATA_4Gray[109]);    //0x1C
}

/******************************************************************************
function :	Setting the display window
parameter:
******************************************************************************/
void EPD_4in26::EPD_4in26_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    EPD_4in26_SendCommand(0x44); // SET_RAM_X_ADDRESS_START_END_POSITION
    EPD_4in26_SendData(Xstart & 0xFF);
    EPD_4in26_SendData((Xstart>>8) & 0x03);
    EPD_4in26_SendData(Xend & 0xFF);
    EPD_4in26_SendData((Xend>>8) & 0x03);

    EPD_4in26_SendCommand(0x45); // SET_RAM_Y_ADDRESS_START_END_POSITION
    EPD_4in26_SendData(Ystart & 0xFF);
    EPD_4in26_SendData((Ystart>>8) & 0x03);
    EPD_4in26_SendData(Yend & 0xFF);
    EPD_4in26_SendData((Yend>>8) & 0x03);
}

/******************************************************************************
function :	Set Cursor
parameter:
******************************************************************************/
void EPD_4in26::EPD_4in26_SetCursor(UWORD Xstart, UWORD Ystart)
{
    EPD_4in26_SendCommand(0x4E); // SET_RAM_X_ADDRESS_COUNTER
    EPD_4in26_SendData(Xstart & 0xFF);
    EPD_4in26_SendData((Xstart>>8) & 0x03);

    EPD_4in26_SendCommand(0x4F); // SET_RAM_Y_ADDRESS_COUNTER
    EPD_4in26_SendData(Ystart & 0xFF);
    EPD_4in26_SendData((Ystart>>8) & 0x03);
}

/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
void EPD_4in26::EPD_4in26_Init(void)
{
	EPD_4in26_Reset();
	DEV_Delay_ms(100);

	EPD_4in26_ReadBusy();
	EPD_4in26_SendCommand(0x12);  //SWRESET
	EPD_4in26_ReadBusy();

	EPD_4in26_SendCommand(0x18); // use the internal temperature sensor
	EPD_4in26_SendData(0x80);

	EPD_4in26_SendCommand(0x0C); //set soft start
	EPD_4in26_SendData(0xAE);
	EPD_4in26_SendData(0xC7);
	EPD_4in26_SendData(0xC3);
	EPD_4in26_SendData(0xC0);
	EPD_4in26_SendData(0x80);

	EPD_4in26_SendCommand(0x01);   //      drive output control
	EPD_4in26_SendData((EPD_4in26_HEIGHT-1)%256); //  Y
	EPD_4in26_SendData((EPD_4in26_HEIGHT-1)/256); //  Y
	EPD_4in26_SendData(0x02);

	EPD_4in26_SendCommand(0x3C);        // Border       Border setting
	EPD_4in26_SendData(0x01);

	EPD_4in26_SendCommand(0x11);        //    data  entry  mode
	EPD_4in26_SendData(0x01);           //       X-mode  x+ y-

	EPD_4in26_SetWindows(0, EPD_4in26_HEIGHT-1, EPD_4in26_WIDTH-1, 0);

	EPD_4in26_SetCursor(0, 0);

	EPD_4in26_ReadBusy();
}

void EPD_4in26::EPD_4in26_Init_Fast(void)
{
	EPD_4in26_Reset();
	DEV_Delay_ms(100);

	EPD_4in26_ReadBusy();
	EPD_4in26_SendCommand(0x12);  //SWRESET
	EPD_4in26_ReadBusy();

	EPD_4in26_SendCommand(0x18); // use the internal temperature sensor
	EPD_4in26_SendData(0x80);

	EPD_4in26_SendCommand(0x0C); //set soft start
	EPD_4in26_SendData(0xAE);
	EPD_4in26_SendData(0xC7);
	EPD_4in26_SendData(0xC3);
	EPD_4in26_SendData(0xC0);
	EPD_4in26_SendData(0x80);

	EPD_4in26_SendCommand(0x01);   //      drive output control
	EPD_4in26_SendData((EPD_4in26_HEIGHT-1)%256); //  Y
	EPD_4in26_SendData((EPD_4in26_HEIGHT-1)/256); //  Y
	EPD_4in26_SendData(0x02);

	EPD_4in26_SendCommand(0x3C);        // Border       Border setting
	EPD_4in26_SendData(0x01);

	EPD_4in26_SendCommand(0x11);        //    data  entry  mode
	EPD_4in26_SendData(0x01);           //       X-mode  x+ y-

	EPD_4in26_SetWindows(0, EPD_4in26_HEIGHT-1, EPD_4in26_WIDTH-1, 0);

	EPD_4in26_SetCursor(0, 0);

	EPD_4in26_ReadBusy();

	//TEMP (1.5s)
	EPD_4in26_SendCommand(0x1A);
    EPD_4in26_SendData(0x5A);

    EPD_4in26_SendCommand(0x22);
    EPD_4in26_SendData(0x91);
    EPD_4in26_SendCommand(0x20);

	EPD_4in26_ReadBusy();
}

void EPD_4in26::EPD_4in26_Init_4GRAY(void)
{
    EPD_4in26_Reset();
	DEV_Delay_ms(100);

	EPD_4in26_ReadBusy();
	EPD_4in26_SendCommand(0x12);  //SWRESET
	EPD_4in26_ReadBusy();

	EPD_4in26_SendCommand(0x18); // use the internal temperature sensor
	EPD_4in26_SendData(0x80);

	EPD_4in26_SendCommand(0x0C); //set soft start
	EPD_4in26_SendData(0xAE);
	EPD_4in26_SendData(0xC7);
	EPD_4in26_SendData(0xC3);
	EPD_4in26_SendData(0xC0);
	EPD_4in26_SendData(0x80);

	EPD_4in26_SendCommand(0x01);   //      drive output control
	EPD_4in26_SendData((EPD_4in26_WIDTH-1)%256); //  Y
	EPD_4in26_SendData((EPD_4in26_WIDTH-1)/256); //  Y
	EPD_4in26_SendData(0x02);

	EPD_4in26_SendCommand(0x3C);        // Border       Border setting
	EPD_4in26_SendData(0x01);

	EPD_4in26_SendCommand(0x11);        //    data  entry  mode
	EPD_4in26_SendData(0x01);           //       X-mode  x+ y-

	EPD_4in26_SetWindows(0, EPD_4in26_HEIGHT-1, EPD_4in26_WIDTH-1, 0);

	EPD_4in26_SetCursor(0, 0);

	EPD_4in26_ReadBusy();

    EPD_4in26_Lut();
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void EPD_4in26::EPD_4in26_Clear(void)
{
	UWORD i;
	UWORD height = EPD_4in26_HEIGHT;
	UWORD width = EPD_4in26_WIDTH/8;
	UBYTE image[EPD_4in26_WIDTH / 8] = {0x00};
    for(i=0; i<width; i++) {
        image[i] = 0xff;
    }

	EPD_4in26_SendCommand(0x24);   //write RAM for black(0)/white (1)
	for(i=0; i<height; i++)
	{
	    EPD_4in26_SendData2(image, width);
	}

	EPD_4in26_SendCommand(0x26);   //write RAM for black(0)/white (1)
	for(i=0; i<height; i++)
	{
		EPD_4in26_SendData2(image, width);
	}
	EPD_4in26_TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void EPD_4in26::EPD_4in26_Display(UBYTE *Image)
{
	UWORD i;
	UWORD height = EPD_4in26_HEIGHT;
	UWORD width = EPD_4in26_WIDTH/8;

	EPD_4in26_SendCommand(0x24);   //write RAM for black(0)/white (1)
	for(i=0; i<height; i++)
	{
        EPD_4in26_SendData2((UBYTE *)(Image+i*width), width);
	}
	EPD_4in26_TurnOnDisplay();
}

void EPD_4in26::EPD_4in26_Display_Base( const  UBYTE *Image)
{
	UWORD i;
	UWORD height = EPD_4in26_HEIGHT;
	UWORD width = EPD_4in26_WIDTH/8;
//
//	EPD_4in26_SendCommand(0x11);        //    data  entry  mode
//	EPD_4in26_SendData(0x01);           //       X-mode  x+ y-

//	EPD_4in26_SetWindows(0, EPD_4in26_HEIGHT-1, EPD_4in26_WIDTH-1, 0);
//
//	EPD_4in26_SetCursor(0, 0);

	EPD_4in26_SendCommand(0x24);   //write RAM for black(0)/white (1)
	for(i=0; i<height; i++)
	{
		EPD_4in26_SendData2((UBYTE *)(Image+i*width), width);
	}

	EPD_4in26_SendCommand(0x26);   //write RAM for black(0)/white (1)
	for(i=0; i<height; i++)
	{
		EPD_4in26_SendData2((UBYTE *)(Image+i*width), width);
	}
	EPD_4in26_TurnOnDisplay();
}

void EPD_4in26::EPD_4in26_Display_Fast(const  UBYTE *Image)
{
	UWORD i;
	UWORD height = EPD_4in26_HEIGHT;
	UWORD width = EPD_4in26_WIDTH/8;

	EPD_4in26_SendCommand(0x24);   //write RAM for black(0)/white (1)
	for(i=0; i<height; i++)
	{
		EPD_4in26_SendData2((UBYTE *)(Image+i*width), width);
	}
	EPD_4in26_TurnOnDisplay_Fast();
}



void EPD_4in26::EPD_4in26_Display_Part(const  UBYTE *Image, UWORD x, UWORD y, UWORD w, UWORD l)
{
	UWORD i;
	UWORD height = l;
	UWORD width =  (w % 8 == 0)? (w / 8 ): (w / 8 + 1);

//	x =200;
//	y =200;
    EPD_4in26_Reset();


	EPD_4in26_SetWindows(x, y, x+w-1, y+l-1);

	EPD_4in26_SetCursor(x, y);

	EPD_4in26_SendCommand(0x24);   //write RAM for black(0)/white (1)
	for(i=0; i<height; i++)
	{
		EPD_4in26_SendData2((UBYTE *)(Image+i*width), width);
	}

	EPD_4in26_TurnOnDisplay_Part();
}

void EPD_4in26::EPD_4in26_Display_Part2(const  UBYTE *Image, UWORD x, UWORD y, UWORD w, UWORD l)
{
	UWORD i;
	UWORD height = l;
	UWORD width =  (w % 8 == 0)? (w / 8 ): (w / 8 + 1);

    EPD_4in26_Reset();

	EPD_4in26_SendCommand(0x18); // use the internal temperature sensor
	EPD_4in26_SendData(0x80);

	EPD_4in26_SendCommand(0x3C);        // Border       Border setting
	EPD_4in26_SendData(0x80);

	EPD_4in26_SetWindows(x, y, x+w-1, y+l-1);

	EPD_4in26_SetCursor(x, y);

	EPD_4in26_SendCommand(0x24);   //write RAM for black(0)/white (1)
	for(i=0; i<height; i++)
	{
		EPD_4in26_SendData2((UBYTE *)(Image+i*width), width);
	}
	EPD_4in26_SendCommand(0x26);   //write RAM for black(0)/white (1)
	for(i=0; i<height; i++)
	{
		EPD_4in26_SendData2((UBYTE *)(Image+i*width), width);
	}

	EPD_4in26_TurnOnDisplay_Part();
}

void EPD_4in26::EPD_4in26_WriteToBuffer(const UBYTE *Image, UWORD x, UWORD y, UWORD w, UWORD l){

	UWORD i;
	UWORD height = l;
	UWORD width =  (w % 8 == 0)? (w / 8 ): (w / 8 + 1);

    EPD_4in26_Reset();

    EPD_4in26_SendCommand(0x18); // use the internal temperature sensor
	EPD_4in26_SendData(0x80);

//	EPD_4in26_SendCommand(0x3C);        // Border       Border setting
//	EPD_4in26_SendData(0x80);

	EPD_4in26_SetWindows(x, EPD_4in26_HEIGHT-y-1, x+w-1, EPD_4in26_HEIGHT -(y+l));

	EPD_4in26_SetCursor(x, y);

	EPD_4in26_SendCommand(0x24);   //write RAM for black(0)/white (1)
	for(i=0; i<height; i++)
	{
		EPD_4in26_SendData2((UBYTE *)(Image+i*width), width);
	}

}


void EPD_4in26::EPD_4in26_4GrayDisplay(const UBYTE *Image)
{
    UDOUBLE i,j,k;
    UBYTE temp1,temp2,temp3;

    EPD_4in26_SendCommand(0x24);
    for(i=0; i<24000; i++) {
        temp3=0;
        for(j=0; j<2; j++) {
            temp1 = Image[i*2+j];
            for(k=0; k<2; k++) {
                temp2 = temp1&0xC0;
                if(temp2 == 0xC0)
                    temp3 |= 0x00;
                else if(temp2 == 0x00)
                    temp3 |= 0x01;
                else if(temp2 == 0x80)
                    temp3 |= 0x01;
                else //0x40
                    temp3 |= 0x00;
                temp3 <<= 1;

                temp1 <<= 2;
                temp2 = temp1&0xC0 ;
                if(temp2 == 0xC0)
                    temp3 |= 0x00;
                else if(temp2 == 0x00)
                    temp3 |= 0x01;
                else if(temp2 == 0x80)
                    temp3 |= 0x01;
                else    //0x40
                    temp3 |= 0x00;
                if(j!=1 || k!=1)
                    temp3 <<= 1;

                temp1 <<= 2;
            }

        }
        EPD_4in26_SendData(temp3);
        // printf("%x",temp3);
    }
     for(i=0; i<24000; i++) {
         EPD_4in26_SendData(0x00);
     }

    EPD_4in26_SendCommand(0x26);   //write RAM for black(0)/white (1)
    for(i=0; i<24000; i++) {
        temp3=0;
        for(j=0; j<2; j++) {
            temp1 = Image[i*2+j];
            for(k=0; k<2; k++) {
                temp2 = temp1&0xC0 ;
                if(temp2 == 0xC0)
                    temp3 |= 0x00;//white
                else if(temp2 == 0x00)
                    temp3 |= 0x01;  //black
                else if(temp2 == 0x80)
                    temp3 |= 0x00;  //gray1
                else //0x40
                    temp3 |= 0x01; //gray2
                temp3 <<= 1;

                temp1 <<= 2;
                temp2 = temp1&0xC0 ;
                if(temp2 == 0xC0)  //white
                    temp3 |= 0x00;
                else if(temp2 == 0x00) //black
                    temp3 |= 0x01;
                else if(temp2 == 0x80)
                    temp3 |= 0x00; //gray1
                else    //0x40
                    temp3 |= 0x01;	//gray2
                if(j!=1 || k!=1)
                    temp3 <<= 1;

                temp1 <<= 2;
            }
        }
        EPD_4in26_SendData(temp3);
        // printf("%x",temp3);
    }
    for(i=0; i<24000; i++) {
         EPD_4in26_SendData(0x00);
     }

    EPD_4in26_TurnOnDisplay_4GRAY();
}

/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
void EPD_4in26::EPD_4in26_Sleep(void)
{
	EPD_4in26_SendCommand(0x10); //enter deep sleep
	EPD_4in26_SendData(0x03);
	DEV_Delay_ms(100);
}
