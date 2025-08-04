#pragma once
#include <Arduino.h>

#define BAT_ADC_PIN        7
#define Measurement_offset 0.990476f

extern float BAT_analogVolts;

void BAT_Init(void);
float BAT_Get_Volts(void);
float BAT_Get_Percent(void);
