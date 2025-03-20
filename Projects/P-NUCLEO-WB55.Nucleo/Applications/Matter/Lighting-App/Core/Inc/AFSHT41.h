/*
 * AFSHT41.h
 *
 *  Created on: Jan 18, 2025
 *      Author: hasan
 */

#ifndef INC_AFSHT41_H_
#define INC_AFSHT41_H_


#ifndef AFSHT41_H
#define AFSHT41_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32wbxx_hal.h" /* needed for I2C */
#include "stm32wbxx_hal_i2c.h" /* needed for I2C */

/*
 * DEFINES
 */
#define AFSHT41_I2C_ADDRESS 0x44  /* I2C address  (pg. 1) */
#define AFSHT41_READ_TIMEOUT 20   // Timeout in ms

/*
 * SNESOR STRUCT
 */
typedef struct {
    I2C_HandleTypeDef *hi2c;     /* I2C handle */
} AFSHT41;

/*
 * INITIALIZATION
 */
HAL_StatusTypeDef AFSHT41_Init(AFSHT41 *sensor);

/*
 * DATA ACQUISITION
 */
float AFSHT41_ReadTemperature(AFSHT41 *sensor);
#ifdef __cplusplus
}
#endif

#endif // AFSHT41_H

#endif /* INC_AFSHT41_H_ */
