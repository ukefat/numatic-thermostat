#include "MPRLS.h"
//

HAL_StatusTypeDef MPRLS_Init(MPRLS_HandleTypeDef *sensor) {
#ifdef _LOGGING

	printf("MPRLS_INIT\r\n");
#endif
	if (sensor->reset_port != NULL) {
		HAL_GPIO_WritePin(sensor->reset_port, sensor->reset_pin, GPIO_PIN_RESET);
		HAL_Delay(10);
		HAL_GPIO_WritePin(sensor->reset_port, sensor->reset_pin, GPIO_PIN_SET);
		HAL_Delay(10);
	}

	// Delay to allow sensor startup
	HAL_Delay(10);

	// Check sensor status
	if (MPRLS_ReadStatus(sensor) & MPRLS_STATUS_BUSY) {
		return HAL_ERROR;
	}

	return HAL_OK;
}

uint8_t MPRLS_ReadStatus(MPRLS_HandleTypeDef *sensor) {

#ifdef _LOGGING
	printf("MPRLS_ReadStatus\r\n");
#endif
    uint8_t status;
    if (HAL_I2C_Master_Receive(sensor->hi2c, MPRLS_I2C_ADDRESS << 1, &status, 1, HAL_MAX_DELAY) != HAL_OK) {
        return 0xFF;  // Error
    }
    return status;
}

float MPRLS_ReadPressure(MPRLS_HandleTypeDef *sensor) {
    uint8_t tx_buffer[3] = {0xAA, 0x00, 0x00};
    uint8_t rx_buffer[4];
    uint32_t raw_pressure;
    uint8_t status;

    // Start measurement
    if (HAL_I2C_Master_Transmit(sensor->hi2c, MPRLS_I2C_ADDRESS << 1, tx_buffer, 3, HAL_MAX_DELAY) != HAL_OK) {
        return -99;  // Error
    }

    // Wait for EOC or poll status
    uint32_t start_time = HAL_GetTick();
    if (sensor->eoc_port != NULL) {
        while (!HAL_GPIO_ReadPin(sensor->eoc_port, sensor->eoc_pin)) {
            if (HAL_GetTick() - start_time > MPRLS_READ_TIMEOUT) {
                return -99;  // Timeout
            }
        }
    } else {
        do {
            status = MPRLS_ReadStatus(sensor);
            if (HAL_GetTick() - start_time > MPRLS_READ_TIMEOUT) {
                return -99;  // Timeout
            }
#ifdef _LOGGINGmath
            printf("status %u\n", status);
#endif
        } while (status & MPRLS_STATUS_BUSY);
    }

    // Read data
    if (HAL_I2C_Master_Receive(sensor->hi2c, MPRLS_I2C_ADDRESS << 1, rx_buffer, 4, HAL_MAX_DELAY) != HAL_OK) {
        return -99;  // Error
    }

    // Check status byte
    if (rx_buffer[0] & (MPRLS_STATUS_FAILED | MPRLS_STATUS_MATHSAT)) {
        return -99;  // Error
    }

    //calculation of pressure value according to equation 2 of datasheet
    raw_pressure = (rx_buffer[1] << 16) | (rx_buffer[2] << 8) | rx_buffer[3];
    float pressure = (raw_pressure - sensor->output_min) * (sensor->psi_max - sensor->psi_min);
    pressure /= (sensor->output_max - sensor->output_min);
    pressure += sensor->psi_min;
    return pressure * sensor->conversion_factor;
}
