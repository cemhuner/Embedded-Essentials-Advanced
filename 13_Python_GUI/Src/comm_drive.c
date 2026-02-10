
#include "comm_drive.h"
#include "uart.h"  // Your UART driver

/**
 * Sends ADC channel data using the defined protocol
 * Format: $#value1#value2#...#valueN?
 *
 * @param adc_values: Array of ADC values to send
 * @param channel_count: Number of channels to send (1-8)
 */
void MSG_SendADCData(uint16_t *adc_values, uint8_t channel_count) {
    char message[MAX_MSG_LENGTH];
    char temp_str[8];  // Temporary string for number conversion
    int msg_index = 0;

    // Validate input
    if (channel_count == 0 || channel_count > MAX_CHANNELS || adc_values == NULL) {
        return;
    }

    // Start marker (optional but helpful for parsing)
    message[msg_index++] = MSG_START;

    // Add each ADC value with separator
    for (uint8_t i = 0; i < channel_count; i++) {
        message[msg_index++] = MSG_SEPARATOR;

        // Convert number to string manually (avoiding sprintf for simplicity)
        uint16_t value = adc_values[i];
        int temp_index = 0;

        // Handle the case when value is 0
        if (value == 0) {
            temp_str[temp_index++] = '0';
        } else {
            // Convert number to string (reverse order first)
            while (value > 0) {
                temp_str[temp_index++] = (value % 10) + '0';
                value /= 10;
            }

            // Reverse the string to get correct order
            for (int j = 0; j < temp_index / 2; j++) {
                char temp = temp_str[j];
                temp_str[j] = temp_str[temp_index - 1 - j];
                temp_str[temp_index - 1 - j] = temp;
            }
        }

        // Copy converted string to message
        for (int j = 0; j < temp_index; j++) {
            if (msg_index < MAX_MSG_LENGTH - 2) {  // Leave space for EOL and null terminator
                message[msg_index++] = temp_str[j];
            }
        }
    }

    // End of line marker
    message[msg_index++] = MSG_END_OF_LINE;
    message[msg_index] = '\0';  // Null terminator

    // Send message via UART using your _write function
    printf("%s", message);
}

/**
 * Generic function for sending sensor data (I2C, SPI, etc.)
 * Format: $TYPE#value1#value2#...#valueN?
 *
 * @param sensor_values: Array of sensor values
 * @param data_count: Number of data points
 * @param data_type: Type identifier ("ADC", "I2C", "SPI", etc.)
 */
void MSG_SendSensorData(int *sensor_values, uint8_t data_count) {
    char message[MAX_MSG_LENGTH];
    char temp_str[8];
    int msg_index = 0;

    // Validate input
    if (data_count == 0 || data_count > MAX_CHANNELS || sensor_values == NULL) {
        return;
    }

    // Start marker
    message[msg_index++] = MSG_START;

    // Add each sensor value with separator
    for (uint8_t i = 0; i < data_count; i++) {
        message[msg_index++] = MSG_SEPARATOR;

        // Convert number to string
        int value = sensor_values[i];
        int temp_index = 0;

        if (value == 0) {
            temp_str[temp_index++] = '0';
        } else {
            while (value > 0) {
                temp_str[temp_index++] = (value % 10) + '0';
                value /= 10;
            }

            // Reverse string
            for (int j = 0; j < temp_index / 2; j++) {
                char temp = temp_str[j];
                temp_str[j] = temp_str[temp_index - 1 - j];
                temp_str[temp_index - 1 - j] = temp;
            }
        }

        // Copy to message
        for (int j = 0; j < temp_index; j++) {
            if (msg_index < MAX_MSG_LENGTH - 2) {
                message[msg_index++] = temp_str[j];
            }
        }
    }

    // End marker
    message[msg_index++] = MSG_END_OF_LINE;
    message[msg_index] = '\0';

    // Send message
    printf("%s", message);
}

