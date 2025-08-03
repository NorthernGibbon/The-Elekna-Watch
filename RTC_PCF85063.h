#pragma once

#include "I2C_Driver.h"

#define PCF85063_ADDRESS     0x51
#define YEAR_OFFSET          1970

// Control & status registers
#define RTC_CTRL_1_ADDR      0x00
#define RTC_CTRL_2_ADDR      0x01
#define RTC_OFFSET_ADDR      0x02
#define RTC_RAM_BY_ADDR      0x03

// Time & date registers
#define RTC_SECOND_ADDR      0x04
#define RTC_MINUTE_ADDR      0x05
#define RTC_HOUR_ADDR        0x06
#define RTC_DAY_ADDR         0x07
#define RTC_WDAY_ADDR        0x08
#define RTC_MONTH_ADDR       0x09
#define RTC_YEAR_ADDR        0x0A  // years 0-99; real year = YEAR_OFFSET + RTC_YEAR_ADDR

// Alarm registers
#define RTC_SECOND_ALARM     0x0B
#define RTC_MINUTE_ALARM     0x0C
#define RTC_HOUR_ALARM       0x0D
#define RTC_DAY_ALARM        0x0E
#define RTC_WDAY_ALARM       0x0F

// Timer registers
#define RTC_TIMER_VALUE_ADDR 0x10
#define RTC_TIMER_MODE_ADDR  0x11

// RTC_CTRL_1 bits
#define RTC_CTRL_1_EXT_TEST  0x80
#define RTC_CTRL_1_STOP      0x20  // 0 = runs, 1 = stopped
#define RTC_CTRL_1_SR        0x10  // software reset
#define RTC_CTRL_1_CIE       0x04  // correction interrupt enable
#define RTC_CTRL_1_12_24     0x02  // 0 = 24h, 1 = 12h
#define RTC_CTRL_1_CAP_SEL   0x01  // 0 = 7pF, 1 = 12.5pF

// RTC_CTRL_2 bits
#define RTC_CTRL_2_AIE       0x80  // alarm interrupt enable
#define RTC_CTRL_2_AF        0x40  // alarm flag
#define RTC_CTRL_2_MI        0x20  // minute interrupt enable
#define RTC_CTRL_2_HMI       0x10  // half-minute interrupt enable
#define RTC_CTRL_2_TF        0x08  // timer flag

// Timer_mode bits
#define RTC_TIMER_MODE_TE    0x04  // timer enable
#define RTC_TIMER_MODE_TIE   0x02  // timer interrupt enable
#define RTC_TIMER_MODE_TI_TP 0x01  // pulse mode

// Alarm format bit
#define RTC_ALARM            0x80

// Defaults
#define RTC_CTRL_1_DEFAULT   0x00
#define RTC_CTRL_2_DEFAULT   0x00

// Data structure for date/time
typedef struct {
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  dotw;
    uint8_t  hour;
    uint8_t  minute;
    uint8_t  second;
} datetime_t;

extern datetime_t datetime;

// Initialization & loop
void PCF85063_Init(void);
void RTC_Loop(void);
void PCF85063_Reset(void);

// Set & read time/date
void PCF85063_Set_Time(datetime_t time);
void PCF85063_Set_Date(datetime_t date);
void PCF85063_Set_All(datetime_t time);
void PCF85063_Read_Time(datetime_t *time);

// Alarm functions
void PCF85063_Enable_Alarm(void);
uint8_t PCF85063_Get_Alarm_Flag(void);
void PCF85063_Set_Alarm(datetime_t time);
void PCF85063_Read_Alarm(datetime_t *time);

// New: 1 Hz timer interrupt
// Configure on-chip countdown timer for a 1 Hz pulse on INT pin
void PCF85063_Enable_1Hz_Timer(void);

// Utility
void datetime_to_str(char *datetime_str, datetime_t time);
