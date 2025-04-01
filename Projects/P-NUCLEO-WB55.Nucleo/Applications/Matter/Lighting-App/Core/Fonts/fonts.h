/*
 * fonts.h
 *
 *  Created on: Mar 21, 2025
 *      Author: myles
 */

#ifndef FONTS_FONTS_H_
#define FONTS_FONTS_H_

#include "Century30.h"
#include "Century40.h"
#include "Century60.h"
#include "Century90limited.h"


#define MAX_HEIGHT_FONT         41
#define MAX_WIDTH_FONT          32
#define OFFSET_BITMAP

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

//ASCII
typedef struct _tFont
{
  const uint8_t *table;
  uint16_t Width;
  uint16_t Height;

} sFONT;


//GB2312
typedef struct                                          // ������ģ���ݽṹ
{
  unsigned char index[2];                               // ������������
  const char matrix[MAX_HEIGHT_FONT*MAX_WIDTH_FONT/8];  // ����������
}CH_CN;


typedef struct
{
  const CH_CN *table;
  uint16_t size;
  uint16_t ASCII_Width;
  uint16_t Width;
  uint16_t Height;

}cFONT;

extern sFONT Font24;
extern sFONT Font20;
extern sFONT Font16;
//extern sFONT Font12;
//extern sFONT Font8;

//extern cFONT Font12CN;
//extern cFONT Font24CN;
#ifdef __cplusplus
}
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

#endif /* FONTS_FONTS_H_ */
