#include <Arduino.h>
#include "Cdrv8833.h"
#include <esp32-hal-gpio.h>

Cdrv8833::Cdrv8833() {
	// not initialized
	m_in1Pin = -1;
	m_in2Pin = -1;
	// m_power = 0;
	m_offset = 0;  // PWM偏移值
	m_swapDirection = false;
	m_decayMode = drv8833DecaySlow;
}

Cdrv8833::Cdrv8833(uint8_t in1Pin, uint8_t in2Pin, uint8_t channel, float offset, bool swapDirection) {
	init(in1Pin, in2Pin, channel, offset, swapDirection);
}

Cdrv8833::~Cdrv8833() {
	stop();
}

bool Cdrv8833::init(uint8_t in1Pin, uint8_t in2Pin, uint8_t channel, float offset, bool swapDirection) {
	if (channel > 15)
		return false;
	if ((m_in1Pin != -1) && (m_in2Pin != -1))
		stop();
	pinMode(in1Pin, OUTPUT);
	pinMode(in2Pin, OUTPUT);
	m_in1Pin = in1Pin;
	m_in2Pin = in2Pin;
	// m_power = 0;
	m_offset = offset; // 設定偏移值
	m_swapDirection = swapDirection;
	m_channel = channel;
	m_decayMode = drv8833DecaySlow;
	// ledcSetup(channel, PWM_FREQUENCY, PWM_BIT_RESOLUTION);
	return true;
}

bool Cdrv8833::move(float power) {
	if (-1 == m_in1Pin)
		return false;
	if (-1 == m_in2Pin)
		return false;
	if (0 == power) {
		stop();
		return true;
	}
	
	if (power > 0)
		power += m_offset;  // 加上偏移值
	else 
		power -= m_offset;

	if (power > 100)
		power = 100;
	if (power < -100)
		power = -100;
	// m_power = power;

	if (m_swapDirection)
		power = -power;
	float value = (float)((1 << PWM_BIT_RESOLUTION) - 1) * ((float)abs(power))/100.0;
	uint32_t dutyCycle = lround(value);  // 四捨五入並轉成整數值
	
	/*
	if ((value - trunc(value)) < 0.5) 
		dutyCycle = value;
	else
		dutyCycle = value + 1;
    */

	if (drv8833DecaySlow == m_decayMode)
		dutyCycle = ((1 << PWM_BIT_RESOLUTION) - 1) - dutyCycle;

	if (power > 0) { // forward
		if (drv8833DecayFast == m_decayMode) {
			// forward fast decay
			#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
			ledcDetach(m_in2Pin);
			ledcAttachChannel(m_in1Pin, PWM_FREQUENCY, PWM_BIT_RESOLUTION, m_channel); // 接腳, 頻率, 解析度, 通道
			ledcWrite(m_in1Pin, dutyCycle);
			#else
			ledcDetachPin(m_in2Pin);
			ledcAttachPin(m_in1Pin, m_channel);
			#endif

			digitalWrite(m_in2Pin, LOW);
		}
		else {
			// forward slow decay
			#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
			ledcDetach(m_in1Pin);
			ledcAttachChannel(m_in2Pin, PWM_FREQUENCY, PWM_BIT_RESOLUTION, m_channel);
			ledcWrite(m_in2Pin, dutyCycle);
			#else
			ledcDetachPin(m_in1Pin);
			ledcAttachPin(m_in2Pin, m_channel);
			#endif
			
			digitalWrite(m_in1Pin, HIGH);
		}
	} else { // reverse
		if (drv8833DecayFast == m_decayMode) {
			// reverse fast decay
			#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
			ledcDetach(m_in1Pin);
			ledcAttachChannel(m_in2Pin, PWM_FREQUENCY, PWM_BIT_RESOLUTION, m_channel);
			ledcWrite(m_in2Pin, dutyCycle);
			#else
			ledcDetachPin(m_in1Pin);
			ledcAttachPin(m_in2Pin, m_channel);
			#endif
			
			digitalWrite(m_in1Pin, LOW);
		} else {
			// reverse slow decay
			#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
			ledcDetach(m_in2Pin);
			ledcAttachChannel(m_in1Pin, PWM_FREQUENCY, PWM_BIT_RESOLUTION, m_channel);
			ledcWrite(m_in1Pin, dutyCycle);
			#else
			ledcDetachPin(m_in2Pin);
			ledcAttachPin(m_in1Pin, m_channel);
			#endif
			
			digitalWrite(m_in2Pin, HIGH);
		}
	}

	#if ESP_ARDUINO_VERSION <= ESP_ARDUINO_VERSION_VAL(2, 0, 0)
	ledcWrite(m_channel, dutyCycle);
	#endif
	return true;
}

bool Cdrv8833::stop() {
	if (-1 == m_in1Pin)
		return false;
	if (-1 == m_in2Pin)
		return false;

#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
	ledcDetach(m_in1Pin);
	ledcDetach(m_in2Pin);
#else
	ledcDetachPin(m_in1Pin);
	ledcDetachPin(m_in2Pin);
#endif

	digitalWrite(m_in1Pin, LOW);
	digitalWrite(m_in2Pin, LOW);
	// m_power = 0;
	return true;
}

bool Cdrv8833::brake() {
	if (-1 == m_in1Pin)
		return false;
	if (-1 == m_in2Pin)
		return false;

#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
	ledcDetach(m_in1Pin);
	ledcDetach(m_in2Pin);
#else
	ledcDetachPin(m_in1Pin);
	ledcDetachPin(m_in2Pin);
#endif

	digitalWrite(m_in1Pin, HIGH);
	digitalWrite(m_in2Pin, HIGH);
	// m_power = 0;
	return true;
}

void Cdrv8833::setDecayMode(drv8833DecayMode decayMode) {
	stop();
	m_decayMode = decayMode;
}

void Cdrv8833::setFrequency(uint32_t frequency) {
	stop();
	ledcChangeFrequency(m_channel, frequency, PWM_BIT_RESOLUTION);
}

void Cdrv8833::swapDirection(bool swapDirection) {
	stop();
	m_swapDirection = swapDirection;
}

