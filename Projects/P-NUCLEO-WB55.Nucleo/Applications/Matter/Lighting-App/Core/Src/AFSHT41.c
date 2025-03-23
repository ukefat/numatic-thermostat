#include "AFSHT41.h"


HAL_StatusTypeDef AFSHT41_Init(AFSHT41 *sensor) {
#ifdef _LOGGING
	printf("AFSHT41_INIT\r\n");
#endif
	// Delay to allow sensor startup
	HAL_Delay(10);

	return HAL_OK;
}

float AFSHT41_ReadTemperature(AFSHT41 *sensor) {
	uint8_t tx_buffer[] = {0xFD};  // Command to send
	uint8_t rx_buffer[6] = {0};    // Buffer for received data
	HAL_StatusTypeDef status;

	// Send the command
	//TODO transmission timeout
	status = HAL_I2C_Master_Transmit(sensor->hi2c, (AFSHT41_I2C_ADDRESS << 1), tx_buffer, sizeof(tx_buffer), 5000);
	if (status != HAL_OK) {
		return -99;
	}
	// Wait for the sensor to process the command
	HAL_Delay(10); // 10ms delay

	// Read the response
	status = HAL_I2C_Master_Receive(sensor->hi2c, (AFSHT41_I2C_ADDRESS << 1), rx_buffer, sizeof(rx_buffer), AFSHT41_READ_TIMEOUT);
	if (status != HAL_OK) {
		return -99;
	}

	//t_ticks = rx_bytes[0] * 256 + rx_bytes[1]
	uint16_t t_ticks = (rx_buffer[0] << 8) | rx_buffer[1];
//	uint8_t checksum_t = rx_buffer[2];
	//rh_ticks = rx_bytes[3] * 256 + rx_bytres[4]
	uint16_t rh_ticks = (rx_buffer[3] << 8) | rx_buffer[4];
//	uint8_t checksum_rh = rx_buffer[5];

	// Calculate temperature in °C
	float t_degC = -45.0 + (175.0 * t_ticks / 65535.0);
	// Calculate relative humidity in %RH
	float rh_pRH = -6.0 + (125.0 * rh_ticks / 65535.0);

	// Clamp the humidity value to the range [0, 100]
	if (rh_pRH > 100.0) {
		rh_pRH = 100.0;
	}
	if (rh_pRH < 0.0) {
		rh_pRH = 0.0;
	}

	return t_degC;
//	sensor->humidity = rh_pRH;
}
