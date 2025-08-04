#include "BAT_Driver.h"

void BAT_Init(void)
{
    analogReadResolution(12);
}

float BAT_Get_Volts(void)
{
    int raw = analogRead(BAT_ADC_PIN);
    float volts = (raw / 4095.0f) * 3.3f;
    return volts / Measurement_offset;
}

float BAT_Get_Percent(void)
{
    float v = BAT_Get_Volts();
    const float Vmin = 3.00f, Vmax = 4.20f;
    float pct = (v - Vmin) / (Vmax - Vmin) * 100.0f;
    if (pct < 0)   pct = 0;
    if (pct > 100) pct = 100;
    return pct;
}
