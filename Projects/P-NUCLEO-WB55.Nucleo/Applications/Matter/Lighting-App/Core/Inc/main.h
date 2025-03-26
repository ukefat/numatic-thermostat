/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
//#include <EPD_test.hpp>
#include "stm32wbxx_hal.h"
#include "FreeRTOS.h"
#include "Debug.h"
#include "semphr.h"



/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#ifdef  USE_STM32WBXX_USB_DONGLE
#include "stm32wbxx_usb_dongle.h"
#endif
#ifdef  USE_STM32WBXX_NUCLEO
#include "stm32wbxx_nucleo.h"
#endif
#ifdef  USE_X_NUCLEO_EPD
#include "x_nucleo_epd.h"
#endif
#ifdef USE_STM32WB5M_DK
#include "stm32wb5mm_dk.h"

#endif

#define MAX_SETPOINTS_PER_DAY 4
#define DAYS_IN_WEEK 7
#define RTOS 1
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern RTC_HandleTypeDef hrtc; /**< RTC handler declaration */
extern RNG_HandleTypeDef hrng;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

//ziggy
#ifdef __cplusplus
extern "C" {
#endif
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
#ifdef __cplusplus
}
#endif

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

#define Button_1_Pin GPIO_PIN_0
#define Button_1_GPIO_Port GPIOD
#define Button_2_Pin GPIO_PIN_1
#define Button_2_GPIO_Port GPIOD
#define Button_3_Pin GPIO_PIN_2
#define Button_3_GPIO_Port GPIOC
#define Button_4_Pin GPIO_PIN_3
#define Button_4_GPIO_Port GPIOC
#define Button_5_Pin GPIO_PIN_4
#define Button_5_GPIO_Port GPIOC

#define Button_1_EXTI_IRQn EXTI0_IRQn
#define Button_2_EXTI_IRQn EXTI1_IRQn
#define Button_3_EXTI_IRQn EXTI2_IRQn

#define Button_4_EXTI_IRQn EXTI3_IRQn

#define Button_5_EXTI_IRQn EXTI4_IRQn


/* USER CODE BEGIN Private defines */
#define EPD_4in26_WIDTH 800
#define EPD_4in26_HEIGHT 480

#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

void lukeisfuckingretardisthehoelessmostincooherenceioadnawdnowa(char * buffer, int size, float val);

/* USER CODE END Private defines */


#endif /* __MAIN_H */

