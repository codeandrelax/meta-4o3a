

#ifndef TLC59731_H
#define TLC59731_H

#include <stdint.h>

#define LED_NUMBER (32)

#define ONE_WIRE_ZERO ((uint8_t) 0b10000000)
#define ONE_WIRE_ONE ((uint8_t) 0b10010000)

// 0b1000000010000000100100001001000010010000100000001001000010000000
#define START_OF_SEQUENCE (0x8090809090908080) 
#define END_OF_SEQUENCE (0x00000000)

#define BUFFER_SIZE ((LED_NUMBER + 2) * sizeof(LED_data))

typedef struct __attribute__((packed)) _LED_data {
    uint64_t start_of_sequence;
    uint64_t red_component;
    uint64_t green_component;
    uint64_t blue_component;
    uint32_t end_of_sequence;
} LED_data;

union map{
    uint64_t number;
    uint8_t array[8];
};

extern LED_data LED_buffer[BUFFER_SIZE];

void initialize_buffer(void);
void setLED(uint8_t led_index, uint8_t red_component, uint8_t green_component, uint8_t blue_component);

#endif