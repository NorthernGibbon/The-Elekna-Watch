#pragma once
#include <stdint.h>

bool     SD_Logger_Init(void);
void     SD_Logger_LoadSteps(uint32_t *out_steps);
void     SD_Logger_SaveSteps(uint32_t steps);
// Returns number of entries filled (oldest first, up to max_count past days)
int      SD_Logger_GetHistory(uint32_t *out, int max_count);
