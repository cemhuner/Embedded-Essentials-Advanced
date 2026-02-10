
#ifndef MESSAGE_PROTOCOL_H
#define MESSAGE_PROTOCOL_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>

// Protocol characters
#define MSG_SEPARATOR    '#'
#define MSG_END_OF_LINE  '?'
#define MSG_START        '$'  // Optional start marker for better parsing

// Maximum values
#define MAX_CHANNELS     8    // Expandable for future use
#define MAX_MSG_LENGTH   64   // Enough for 8 channels * 4 digits + separators

// Message protocol functions
void MSG_SendADCData(uint16_t *adc_values, uint8_t channel_count);
void MSG_SendSensorData(int *sensor_values, uint8_t data_count);

#endif // MESSAGE_PROTOCOL_H
