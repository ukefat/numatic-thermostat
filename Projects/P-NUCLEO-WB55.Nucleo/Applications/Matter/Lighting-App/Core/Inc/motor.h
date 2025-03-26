/*
 * AFSHT41.h
 *
 *  Created on: Feb 08, 2025
 *      Author: hasan
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#ifndef MOTOR_H
#define MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32wbxx_hal.h"
#include "MPRLS.h"

/*
 * MOTOR STRUCT
 */
typedef struct {
    TIM_HandleTypeDef *htim_encoder;    /* encoder timer handle */
    TIM_HandleTypeDef *htim_pwm;     	/* PWM output timer handle */
    uint32_t pwm_channel;			/* PWM Output Channel */
    GPIO_TypeDef *in1_port;     // GPIO port for IN1 pin
    uint16_t in1_pin;           // GPIO pin for IN1
    GPIO_TypeDef *in2_port;     // GPIO port for IN2 pin
	uint16_t in2_pin;           // GPIO pin for IN2
	GPIO_TypeDef *stby_port;     // GPIO port for STBY pin
	uint16_t stby_pin;           // GPIO pin for IN1
} Motor_HandleTypeDef;

/*
 * INITIALIZATION
 */
void Motor_Init(Motor_HandleTypeDef *motor, TIM_HandleTypeDef *htim_encoder, TIM_HandleTypeDef *htim_pwm);

/*
 * FUNCTIONS
 */
int16_t Encoder_GetCounter(Motor_HandleTypeDef * motor);
void Encoder_ResetCounter(Motor_HandleTypeDef *motor);
int8_t Encoder_GetDirection(Motor_HandleTypeDef *motor);
int UpdateMotorSignal(Motor_HandleTypeDef *motor, MPRLS_HandleTypeDef *pSensor, float target_pressure);

#ifdef __cplusplus
}
#endif

#endif // MOTOR_H

#endif /* INC_MOTOR_H_ */

