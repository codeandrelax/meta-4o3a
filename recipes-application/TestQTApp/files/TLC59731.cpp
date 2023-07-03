
#include <stdint.h>

#include "TLC59731.h"

LED_data LED_buffer[BUFFER_SIZE];

static uint64_t convert_number(uint8_t number){
    union map map;    
    uint8_t bit = 0;
    uint8_t mask = 0x80;
    for(int i = 0; i < 8; i++)
    {
        bit = number & (mask>>i);
        if(bit)
        {
            map.array[i] = ONE_WIRE_ONE;
        }
        else
        {
            map.array[i] = ONE_WIRE_ZERO;
        }
    }

    return map.number;
}

void initialize_buffer(void)
{
    for(uint32_t i = 1; i < BUFFER_SIZE / sizeof(LED_data) - 1; i++)
    {
        LED_buffer[i].start_of_sequence = START_OF_SEQUENCE;
        LED_buffer[i].red_component = convert_number(0);
        LED_buffer[i].green_component = convert_number(0);
        LED_buffer[i].blue_component = convert_number(0);
        LED_buffer[i].end_of_sequence = 0x00000000;
    }
}

void setLED(uint8_t led_index, uint8_t red_component, uint8_t green_component, uint8_t blue_component)
{
    LED_buffer[led_index].red_component = convert_number(red_component);
    LED_buffer[led_index].green_component = convert_number(green_component);
    LED_buffer[led_index].blue_component = convert_number(blue_component);
}