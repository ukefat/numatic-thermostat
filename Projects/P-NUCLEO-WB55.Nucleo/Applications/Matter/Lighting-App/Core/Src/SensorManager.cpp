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

    temperature = (int16_t)(AFSHT41_ReadTemperature(&tempSensor) * 100);

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
    float P_out = P_mid - (error/PB) * P_half; // calculate pressure output /// TODO: remove volatile

    if(P_out < P_min) P_out = P_min; // clamp outputs
    if(P_out > P_max) P_out = P_max;

	while(UpdateMotorSignal(&motor, &pressureSensor, P_out) == 0)
	{
		float press = (MPRLS_ReadPressure(&pressureSensor) - 12.7f) * 100;
		APP_DBG("Target Pressure: %d, \\(0-0)/ Actual Pressure: %d", (int)(P_out * 100), (int)(press));
		//osDelay(1);
	}
	float final_p = (MPRLS_ReadPressure(&pressureSensor) -12.7f) * 100;
	APP_DBG("====================================================================\n");
	APP_DBG("Current Temperature: %d\n", (int)temperature);
	APP_DBG("Temperature Setpoint: %d\n", (int)setPoint);
	APP_DBG("Temperature Difference: %d\n", (int)error);
	APP_DBG("Target Pressure: %d\n", (int)(P_out * 100));
	APP_DBG("Actual Pressure: %d\n", (int)(final_p));
	APP_DBG("====================================================================\n");
}

void SensorManager::updateTempSetpoint(int16_t setPoint){
	PlatformMgr().LockChipStack();
	    app::Clusters::Thermostat::Attributes::OccupiedHeatingSetpoint::Set(kThermostatEndpoint, setPoint);//, reportState);
	PlatformMgr().UnlockChipStack();
}
