#include "BAT_Driver.h"

void BAT_Init(void)
{
    analogReadResolution(12);                  
    analogSetPinAttenuation(BAT_ADC_PIN, ADC_11db); 
}

float BAT_Get_Volts(void)
{
    int raw = analogRead(BAT_ADC_PIN);
    float volts = (raw / 4095.0f) * 3.3f;
    float adj   = volts / Measurement_offset;
    Serial.printf("🔋 raw=%d → %.3f V → %.3f V (post-divider)\n",
                  raw, volts, adj);
    return adj;
}

float BAT_Get_Percent(void)
{
    float v = BAT_Get_Volts();
    const float Vmin = 3.00f, Vmax = 4.20f;
    float pct = (v - Vmin) / (Vmax - Vmin) * 100.0f;
    pct = pct < 0 ? 0 : pct > 100 ? 100 : pct;
    Serial.printf("🔋 %% = %.1f%%\n", pct);
    return pct;
}
