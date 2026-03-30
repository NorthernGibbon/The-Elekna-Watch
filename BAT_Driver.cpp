#include "BAT_Driver.h"

float BAT_analogVolts = 0;

struct Vpct { float v; uint8_t pct; };
static const Vpct lut[] = {
  {4.20,100}, {4.10,95}, {4.00,90}, {3.90,80},
  {3.80,70}, {3.70,60}, {3.60,50}, {3.50,40},
  {3.40,30}, {3.30,20}, {3.20,10}, {3.00,0}
};
static const size_t LUT_SZ = sizeof(lut)/sizeof(lut[0]);

void BAT_Init(void)
{
  //set the resolution to 12 bits (0-4095)
  analogReadResolution(12);
}
float BAT_Get_Volts(void)
{
  int Volts = analogReadMilliVolts(BAT_ADC_PIN); // millivolts
  BAT_analogVolts = (float)(Volts * 3.0 / 1000.0) / Measurement_offset;
  return BAT_analogVolts;
}

float BAT_Get_Percent(void) {
  float v = BAT_Get_Volts();
  if (v >= lut[0].v) return 100;
  if (v <= lut[LUT_SZ-1].v) return 0;
  for (size_t i = 0; i < LUT_SZ-1; i++) {
    if (v <= lut[i].v && v >= lut[i+1].v) {
      float dv = lut[i].v - lut[i+1].v;
      float dp = float(lut[i].pct - lut[i+1].pct);
      float frac = (v - lut[i+1].v) / dv;
      return lut[i+1].pct + frac * dp;
    }
  }
  return 0;
}