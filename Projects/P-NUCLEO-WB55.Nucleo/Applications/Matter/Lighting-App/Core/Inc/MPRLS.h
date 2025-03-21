/*
 * MPRLS.h
 *
 *  Created on: Dec 29, 2024
 *      Author: kobru
 */

#ifndef INC_MPRLS_H_
#define INC_MPRLS_H_

#ifndef MPRLS_H
#define MPRLS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32wbxx_hal.h" // Replace with the appropriate HAL header for your STM32
#include <stm32wbxx_hal_i2c.h>
#define MPRLS_I2C_ADDRESS 0x18  // I2C address of the sensor
#define MPRLS_STATUS_BUSY 0x20      // Bit 5
#define MPRLS_STATUS_FAILED 0x04   // Bit 2
#define MPRLS_STATUS_MATHSAT 0x01  // Bit 0

#define MPRLS_READ_TIMEOUT 20   // Timeout in ms

typedef struct {
    I2C_HandleTypeDef *hi2c;    // Pointer to the I2C handle
    GPIO_TypeDef *eoc_port;     // GPIO port for EOC pin
    uint16_t eoc_pin;           // GPIO pin for EOC
    GPIO_TypeDef *reset_port;   // GPIO port for Reset pin
    uint16_t reset_pin;         // GPIO pin for Reset
    uint16_t psi_min;           // Minimum pressure range
    uint16_t psi_max;           // Maximum pressure range
    float output_min;           // Minimum transfer function curve
    float output_max;           // Maximum transfer function curve
    float conversion_factor;    // Conversion factor for desired units
} MPRLS_HandleTypeDef;

HAL_StatusTypeDef MPRLS_Init(MPRLS_HandleTypeDef *sensor);
float MPRLS_ReadPressure(MPRLS_HandleTypeDef *sensor);
uint8_t MPRLS_ReadStatus(MPRLS_HandleTypeDef *sensor);


void helloWorld();

#ifdef __cplusplus
}
#endif

#endif // MPRLS_H


#endif /* INC_MPRLS_H_ */
