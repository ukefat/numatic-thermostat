/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *    Copyright (c) 2019 Google LLC.
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/**********************************************************
 * Includes
 *********************************************************/

#include "app_common.h"
#include "SensorManager.h"
//#include "AppConfig.h"
#include "AppEvent.h"
#include "AppTask.h"
#include "dbg_trace.h"


#if defined(SL_MATTER_USE_SI70XX_SENSOR) && SL_MATTER_USE_SI70XX_SENSOR
#include "Si70xxSensor.h"
#endif // defined(SL_MATTER_USE_SI70XX_SENSOR) && SL_MATTER_USE_SI70XX_SENSOR
/**********************************************************
 * Defines and Constants
 *********************************************************/

using namespace chip;
using namespace chip::app;
using namespace ::chip::DeviceLayer;

constexpr EndpointId kThermostatEndpoint = 1;
constexpr uint16_t kSensorTImerPeriodMs  = 10000; // 30s timer period
constexpr uint16_t kMinTemperatureDelta  = 50;    // 0.5 degree Celcius

float P_max = 15.f;
float P_min = 3.f;
const float P_mid = (P_min + P_max) / 2.f; // 9
const float P_half = (P_max - P_min) / 2.f; // 6
int8_t PB = 5;
int16_t target_position = 0;
float output_scale = 100;
/**********************************************************
 * Variable declarations
 *********************************************************/
SensorManager SensorManager::sSensorManager;

#if !(defined(SL_MATTER_USE_SI70XX_SENSOR) && (SL_MATTER_USE_SI70XX_SENSOR))
constexpr uint16_t kSimulatedReadingFrequency = (60000 / kSensorTImerPeriodMs); // Change Simulated number at each minutes
static int16_t mSimulatedTemp[]               = { 2300, 2400, 2800, 2550, 2200, 2125, 2100, 2600, 1800, 2700 };
#endif // !(defined(SL_MATTER_USE_SI70XX_SENSOR) && (SL_MATTER_USE_SI70XX_SENSOR))


AFSHT41 SensorManager::tempSensor = {
	.hi2c = &hi2c1
};

MPRLS_HandleTypeDef SensorManager::pressureSensor = {
	.hi2c = &hi2c3,              // Replace with your I2C handle
	.eoc_port = NULL,            // Not in use, set to NULL
	.eoc_pin = 0,                // Not in use, set to 0
	.reset_port = NULL,          // Not in use, set to NULL
	.reset_pin = 0,              // Not in use, set to 0
	.psi_min = 0,                // Minimum PSI range
	.psi_max = 25,               // Maximum PSI range
	.output_min = 0.1 * 0xFFFFFF, // Minimum transfer function value
	.output_max = 0.9 * 0xFFFFFF, // Maximum transfer function value
	.conversion_factor = 1 // Conversion factor for PSI to hPa // the value calculated will be in PSI change this value to the conversion PSI vs the value you want to read
}; ///

Motor_HandleTypeDef SensorManager::motor = {
//	    .htim_encoder=    /* encoder timer handle */
//	    .htim_pwm=     	/* PWM output timer handle */
	.pwm_channel=TIM_CHANNEL_1,
	.in1_port= GPIOC,
	.in1_pin= GPIO_PIN_12,
	.in2_port= GPIOC,
	.in2_pin= GPIO_PIN_13,
	.stby_port= GPIOC,
	.stby_pin= GPIO_PIN_10
};

CHIP_ERROR SensorManager::Init()
{
    // Create cmsisos sw timer for temp sensor timer.
    mSensorTimer = osTimerNew(SensorTimerEventHandler, osTimerPeriodic, nullptr, nullptr);

    AFSHT41_Init(&tempSensor);

    MPRLS_Init(&pressureSensor);

    Motor_Init(&motor, &htim2, &htim1);

    if (mSensorTimer == NULL)
    {
    	APP_DBG("mSensorTimer timer create failed");
        return APP_ERROR_CREATE_TIMER_FAILED;
    }

//#if defined(SL_MATTER_USE_SI70XX_SENSOR) && SL_MATTER_USE_SI70XX_SENSOR
//    if (SL_STATUS_OK != Si70xxSensor::Init())
//    {
//    	APP_DBG("Failed to Init Sensor");
//        return CHIP_ERROR_INTERNAL;
//    }
//#endif // defined(SL_MATTER_USE_SI70XX_SENSOR) && SL_MATTER_USE_SI70XX_SENSOR

    // Update Temp immediatly at bootup
    SensorTimerEventHandler(nullptr);
    // Trigger periodic update
    osTimerStart(mSensorTimer, pdMS_TO_TICKS(kSensorTImerPeriodMs));
    return CHIP_NO_ERROR;
}

void SensorManager::SensorTimerEventHandler(void * arg)
{
    AppEvent event;
    event.Type    = AppEvent::kEventType_Timer;
    event.Handler = TemperatureUpdateEventHandler;

    AppTask::GetAppTask().PostEvent(&event);
}

void SensorManager::TemperatureUpdateEventHandler(AppEvent * aEvent)
{
    volatile int16_t temperature            = 0; //TODO: remove volatile
    static int16_t lastTemperature = 0;

//#if defined(SL_MATTER_USE_SI70XX_SENSOR) && SL_MATTER_USE_SI70XX_SENSOR
//    int32_t tempSum   = 0;
//    uint16_t humidity = 0;
//
//    for (uint8_t i = 0; i < 100; i++)
//    {
//        if (SL_STATUS_OK != Si70xxSensor::GetSensorData(humidity, temperature))
//        {
//            APP_DBG("Failed to read Temperature !!!");
//        }
//        tempSum += temperature;
//    }
//    temperature = static_cast<int16_t>(tempSum / 100);
//#else
//    static uint8_t nbOfRepetition = 0;
//    static uint8_t simulatedIndex = 0;
//    if (simulatedIndex >= ArraySize(mSimulatedTemp))
//    {
//        simulatedIndex = 0;
//    }
//    temperature = mSimulatedTemp[simulatedIndex];
//
//    nbOfRepetition++;
//    if (nbOfRepetition >= kSimulatedReadingFrequency)
//    {
//        simulatedIndex++;
//        nbOfRepetition = 0;
//    }
//#endif // defined(SL_MATTER_USE_SI70XX_SENSOR) && SL_MATTER_USE_SI70XX_SENSOR


    temperature = (int16_t)(AFSHT41_ReadTemperature(&tempSensor) * 100);
//    while(1){
//		float pressure  = MPRLS_ReadPressure(&pressureSensor);
//		APP_DBG("Pressure is : %d", (int)pressure);
//
//    }
    float pressure  = MPRLS_ReadPressure(&pressureSensor);
    //MarkAttributeDirty reportState = MarkAttributeDirty::kNo;
    if ((temperature >= (lastTemperature + kMinTemperatureDelta)) || temperature <= (lastTemperature - kMinTemperatureDelta))
    {

    }

    lastTemperature = temperature;
    int16_t setPoint = 0;
    PlatformMgr().LockChipStack();
    // The SensorMagager shouldn't be aware of the Endpoint ID TODO Fix this.
    // TODO Per Spec we should also apply the Offset stored in the same cluster before saving the temp
    app::Clusters::Thermostat::Attributes::LocalTemperature::Set(kThermostatEndpoint, temperature);//, reportState);
    app::Clusters::Thermostat::Attributes::OccupiedHeatingSetpoint::Get(kThermostatEndpoint, &setPoint);//, reportState);
    PlatformMgr().UnlockChipStack();

    float error = (setPoint - temperature)/100; // get error
    volatile float P_out = P_mid + (error/PB) * P_half; // calculate pressure output /// TODO: remove volatile

    if(P_out < P_min) P_out = P_min; // clamp outputs
    if(P_out > P_max) P_out = P_max;

    float output_percent = ((P_out - P_min) / (P_max - P_min)) * output_scale; // convert to output percent 0-100%

    target_position = (int16_t)((output_percent * 19100) / 100); // scale from 0 = 0% and 19100 = 100%
	APP_DBG("====================================================================\n");
	APP_DBG("Sensor Pressure: %d", (int)pressure);
	APP_DBG("Temperature Difference: %d\n", (int)error);
    APP_DBG("Target Pressure: %d\n", (int)P_out);
    APP_DBG("Target Position: %d\n", target_position);
    //int16_t counter = __HAL_TIM_GET_COUNTER(&htim2);
	while(UpdateMotorSignal(&motor, target_position) == 0)
	{
		APP_DBG("==========================================================error: %d", __HAL_TIM_GET_COUNTER(&htim2) - target_position);
		//osDelay(1);
	}
	volatile int16_t counter = __HAL_TIM_GET_COUNTER(&htim2); //TODO: remove volatile
	APP_DBG("Final Motor Error: %d\n", __HAL_TIM_GET_COUNTER(&htim2) - target_position);
	APP_DBG("Final Encoder Position: %d\n", __HAL_TIM_GET_COUNTER(&htim2));
	APP_DBG("====================================================================\n");
}

