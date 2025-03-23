#include "motor.h"
#include <stdlib.h>

// Initialize the Motor Struct
void Motor_Init(Motor_HandleTypeDef *motor, TIM_HandleTypeDef *htim_encoder, TIM_HandleTypeDef *htim_pwm) {
	motor->htim_encoder = htim_encoder;  // Store timer reference for encoder
	motor->htim_pwm = htim_pwm;  // Store timer reference for PWM output


    if (HAL_TIM_Encoder_Start(htim_encoder, TIM_CHANNEL_ALL) != HAL_OK) // Start the encoder timer
	{
	  Error_Handler();
	}
    if (HAL_TIM_PWM_Start(htim_pwm, TIM_CHANNEL_1) != HAL_OK) //Start the PWM timer
    {
  	  Error_Handler();
    }

    // RESET all pins
    __HAL_TIM_SET_COMPARE(motor->htim_pwm, motor->pwm_channel, 0);
	HAL_GPIO_WritePin(motor->stby_port, motor->stby_pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(motor->in1_port, motor->in1_pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(motor->in2_port, motor->in2_pin, GPIO_PIN_RESET);
}

// Get Encoder Value
int16_t Encoder_GetCounter(Motor_HandleTypeDef *motor) {
	return __HAL_TIM_GET_COUNTER(motor->htim_encoder);
}

// Reset Encoder Value
void Encoder_ResetCounter(Motor_HandleTypeDef *motor) {
    __HAL_TIM_SET_COUNTER(motor->htim_encoder, 0);
}

// Get Encoder Direction (+1 = Forward, -1 = Backward)
int8_t Encoder_GetDirection(Motor_HandleTypeDef *motor) {
    return __HAL_TIM_IS_TIM_COUNTING_DOWN(motor->htim_encoder) ? -1 : 1;
}

/*
 * TODO:
 * change SCALE VALUE
 */

int UpdateMotorSignal(Motor_HandleTypeDef *motor, int16_t target_position) {
    static int previous_error = 0;
    int16_t encoder_count = __HAL_TIM_GET_COUNTER(motor->htim_encoder);
    int16_t error = target_position - encoder_count;
    int errorMagnitude = error > 0 ? error : -error;
    int derivative = error - previous_error;
    int output;

    // **PID Coefficients**
    float Kp = 0.5;
    float Kd = 0.3;

    // **Calculate PWM Output**
    output = (Kp * errorMagnitude) + (Kd * derivative);
//    output = (output / 10) * 1600; // Scale for PWM

    // **Clamp PWM Output**
    if (output > 1600) output = 1600;
    if (output < 320) output = 320;


    // **Motor Direction Control**
    if (error < 0 && errorMagnitude > 5) {
        HAL_GPIO_WritePin(motor->stby_port, motor->stby_pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(motor->in1_port, motor->in1_pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(motor->in2_port, motor->in2_pin, GPIO_PIN_RESET);
    } else if (error > 0 && errorMagnitude > 5) {
        HAL_GPIO_WritePin(motor->stby_port, motor->stby_pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(motor->in1_port, motor->in1_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(motor->in2_port, motor->in2_pin, GPIO_PIN_SET);
    } else {
        // Stop motor
        __HAL_TIM_SET_COMPARE(motor->htim_pwm, motor->pwm_channel, 0);
        HAL_GPIO_WritePin(motor->stby_port, motor->stby_pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(motor->in1_port, motor->in1_pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(motor->in2_port, motor->in2_pin, GPIO_PIN_SET);
        return 1;  // Exit without modifying previous error
    }

    // **Apply PWM**
    __HAL_TIM_SET_COMPARE(motor->htim_pwm, motor->pwm_channel, output);
    previous_error = error;
    return 0;
}
