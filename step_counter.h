#pragma once
#include <stdint.h>

void StepCounter_Init(void);
void StepCounter_StartTask(void);
uint32_t StepCounter_GetSteps(void);
