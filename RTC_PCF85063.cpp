#include "RTC_PCF85063.h"
#include "I2C_Driver.h"
#include "esp_err.h"
#include <stdio.h>

datetime_t datetime = {0};

static uint8_t decToBcd(int val);
static int bcdToDec(uint8_t val);

const unsigned char MonthStr[12][4] = {
    "Jan", "Feb", "Mar", "Apr",
    "May", "Jun", "Jul", "Aug",
    "Sep", "Oct", "Nov", "Dec"
};

/****************************************************************************
 * Function:    PCF85063_Init
 * Info:        Initialize RTC in 24‑hour run mode, no reset, no correction,
 *              internal capacitance = 12.5 pF.
 ****************************************************************************/
void PCF85063_Init(void)
{
    uint8_t Value = RTC_CTRL_1_DEFAULT | RTC_CTRL_1_CAP_SEL;
    I2C_Write(PCF85063_ADDRESS, RTC_CTRL_1_ADDR, &Value, 1);
    I2C_Read(PCF85063_ADDRESS, RTC_CTRL_1_ADDR, &Value, 1);
    if (Value & RTC_CTRL_1_STOP) {
        printf("PCF85063 failed to initialize. state: 0x%02X\r\n", Value);
    } else {
        printf("PCF85063 running. state: 0x%02X\r\n", Value);
    }
}

/****************************************************************************
 * Function:    PCF85063_Enable_1Hz_Timer
 * Info:        Configure on‑chip countdown timer for 1 Hz pulse on INT pin.
 ****************************************************************************/
void PCF85063_Enable_1Hz_Timer(void)
{
    esp_err_t ret;
    uint8_t tval = 1;  

    ret = I2C_Write(PCF85063_ADDRESS, RTC_TIMER_VALUE_ADDR, &tval, 1);
    if (ret != ESP_OK) {
        printf("PCF85063: Failed to write Timer value (0x%02X)\n", tval);
        return;
    }

    uint8_t tmode = (2 << 3)
                  | RTC_TIMER_MODE_TE
                  | RTC_TIMER_MODE_TIE
                  | RTC_TIMER_MODE_TI_TP;

    ret = I2C_Write(PCF85063_ADDRESS, RTC_TIMER_MODE_ADDR, &tmode, 1);
    if (ret != ESP_OK) {
        printf("PCF85063: Failed to write Timer mode (0x%02X)\n", tmode);
    }
}

/****************************************************************************
 * Function:    RTC_Loop
 * Info:        Simple read loop (unused when using timer interrupt).
 ****************************************************************************/
void RTC_Loop(void)
{
    PCF85063_Read_Time(&datetime);
}

/****************************************************************************
 * Function:    PCF85063_Reset
 * Info:        Issue software reset.
 ****************************************************************************/
void PCF85063_Reset(void)
{
    uint8_t Value = RTC_CTRL_1_DEFAULT | RTC_CTRL_1_CAP_SEL | RTC_CTRL_1_SR;
    esp_err_t ret = I2C_Write(PCF85063_ADDRESS, RTC_CTRL_1_ADDR, &Value, 1);
    if (ret != ESP_OK) {
        printf("PCF85063: Reset failure\r\n");
    }
}

/****************************************************************************
 * Function:    PCF85063_Set_Time
 ****************************************************************************/
void PCF85063_Set_Time(datetime_t time)
{
    uint8_t buf[3] = {
        decToBcd(time.second),
        decToBcd(time.minute),
        decToBcd(time.hour)
    };
    esp_err_t ret = I2C_Write(PCF85063_ADDRESS, RTC_SECOND_ADDR, buf, sizeof(buf));
    if (ret != ESP_OK) {
        printf("PCF85063: Time setting failure\r\n");
    }
}

/****************************************************************************
 * Function:    PCF85063_Set_Date
 ****************************************************************************/
void PCF85063_Set_Date(datetime_t date)
{
    uint8_t buf[4] = {
        decToBcd(date.day),
        decToBcd(date.dotw),
        decToBcd(date.month),
        decToBcd(date.year - YEAR_OFFSET)
    };
    esp_err_t ret = I2C_Write(PCF85063_ADDRESS, RTC_DAY_ADDR, buf, sizeof(buf));
    if (ret != ESP_OK) {
        printf("PCF85063: Date setting failure\r\n");
    }
}

/****************************************************************************
 * Function:    PCF85063_Set_All
 ****************************************************************************/
void PCF85063_Set_All(datetime_t time)
{
    uint8_t buf[7] = {
        decToBcd(time.second),
        decToBcd(time.minute),
        decToBcd(time.hour),
        decToBcd(time.day),
        decToBcd(time.dotw),
        decToBcd(time.month),
        decToBcd(time.year - YEAR_OFFSET)
    };
    esp_err_t ret = I2C_Write(PCF85063_ADDRESS, RTC_SECOND_ADDR, buf, sizeof(buf));
    if (ret != ESP_OK) {
        printf("PCF85063: Failed to set date and time\r\n");
    }
}

/****************************************************************************
 * Function:    PCF85063_Read_Time
 ****************************************************************************/
void PCF85063_Read_Time(datetime_t *time)
{
    uint8_t buf[7] = {0};
    esp_err_t ret = I2C_Read(PCF85063_ADDRESS, RTC_SECOND_ADDR, buf, sizeof(buf));
    if (ret != ESP_OK) {
        printf("PCF85063: Time read failure\r\n");
    } else {
        time->second = bcdToDec(buf[0] & 0x7F);
        time->minute = bcdToDec(buf[1] & 0x7F);
        time->hour   = bcdToDec(buf[2] & 0x3F);
        time->day    = bcdToDec(buf[3] & 0x3F);
        time->dotw   = bcdToDec(buf[4] & 0x07);
        time->month  = bcdToDec(buf[5] & 0x1F);
        time->year   = bcdToDec(buf[6]) + YEAR_OFFSET;
    }
}

/****************************************************************************
 * Function:    PCF85063_Enable_Alarm
 ****************************************************************************/
void PCF85063_Enable_Alarm(void)
{
    uint8_t Value = RTC_CTRL_2_DEFAULT | RTC_CTRL_2_AIE;
    Value &= ~RTC_CTRL_2_AF;
    esp_err_t ret = I2C_Write(PCF85063_ADDRESS, RTC_CTRL_2_ADDR, &Value, 1);
    if (ret != ESP_OK) {
        printf("PCF85063: Failed to enable/clear Alarm flag\r\n");
    }
}

/****************************************************************************
 * Function:    PCF85063_Get_Alarm_Flag
 ****************************************************************************/
uint8_t PCF85063_Get_Alarm_Flag(void)
{
    uint8_t Value = 0;
    esp_err_t ret = I2C_Read(PCF85063_ADDRESS, RTC_CTRL_2_ADDR, &Value, 1);
    if (ret != ESP_OK) {
        printf("PCF85063: Failed to read Alarm flag\r\n");
    } else {
        Value &= (RTC_CTRL_2_AF | RTC_CTRL_2_AIE);
    }
    return Value;
}

/****************************************************************************
 * Function:    PCF85063_Set_Alarm
 ****************************************************************************/
void PCF85063_Set_Alarm(datetime_t time)
{
    uint8_t buf[5] = {
        decToBcd(time.second) & ~RTC_ALARM,
        decToBcd(time.minute) & ~RTC_ALARM,
        decToBcd(time.hour)   & ~RTC_ALARM,
        RTC_ALARM,  // disable day
        RTC_ALARM   // disable weekday
    };
    esp_err_t ret = I2C_Write(PCF85063_ADDRESS, RTC_SECOND_ALARM, buf, sizeof(buf));
    if (ret != ESP_OK) {
        printf("PCF85063: Failed to set Alarm\r\n");
    }
}

/****************************************************************************
 * Function:    PCF85063_Read_Alarm
 ****************************************************************************/
void PCF85063_Read_Alarm(datetime_t *time)
{
    uint8_t buf[5] = {0};
    esp_err_t ret = I2C_Read(PCF85063_ADDRESS, RTC_SECOND_ALARM, buf, sizeof(buf));
    if (ret != ESP_OK) {
        printf("PCF85063: Failed to read Alarm\r\n");
    } else {
        time->second = bcdToDec(buf[0] & 0x7F);
        time->minute = bcdToDec(buf[1] & 0x7F);
        time->hour   = bcdToDec(buf[2] & 0x3F);
        time->day    = bcdToDec(buf[3] & 0x3F);
        time->dotw   = bcdToDec(buf[4] & 0x07);
    }
}

/****************************************************************************
 * Function:    decToBcd
 ****************************************************************************/
static uint8_t decToBcd(int val)
{
    return (uint8_t)(((val / 10) << 4) | (val % 10));
}

/****************************************************************************
 * Function:    bcdToDec
 ****************************************************************************/
static int bcdToDec(uint8_t val)
{
    return (int)(((val >> 4) * 10) + (val & 0x0F));
}

/****************************************************************************
 * Function:    datetime_to_str
 ****************************************************************************/
void datetime_to_str(char *datetime_str, datetime_t time)
{
    sprintf(datetime_str, "%04u-%02u-%02u %u %02u:%02u:%02u", 
            time.year, time.month, time.day, time.dotw, 
            time.hour, time.minute, time.second);
}
