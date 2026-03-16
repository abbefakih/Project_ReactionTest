

#include "gd32vf103.h"
#include <stdlib.h>
#include <stdio.h>
#include "lcd.h"

#define MTIME           ((volatile uint64_t)0xD1000000)
#define SYSTEM_CLOCK    108000000  // 108 MHz
#define TICKS_PER_MS    (SYSTEM_CLOCK / 4000)  // Approximation

void delay_ms(uint32_t ms) {
    uint64_t start = MTIME;
    while ((MTIME - start) < (ms * TICKS_PER_MS));
}

uint32_t get_random_delay() {
    return (2 + (rand() % 5)) * 1000; // Random delay mellan 2-8 sekunder
}

int main() {
    // Initiera GPIO
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC); // För LCD-styrning

    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);  // LED
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_5); // Knapp
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_15); // LCD

    // Initiera LCD
    Lcd_SetType(LCD_INVERTED);
    Lcd_Init();
    LCD_Clear(BLACK);
    LCD_ShowStr(10, 10, "Reaction Timer", BLUE, BLACK);

    delay_ms(2000);

    while (1) {
        LCD_Clear(BLACK);
        LCD_ShowStr(10, 10, "Get Ready...", YELLOW, BLACK);

        delay_ms(get_random_delay());

        LCD_Clear(BLACK);
        LCD_ShowStr(10, 10, "PRESS!", GREEN, BLACK);

        gpio_bit_set(GPIOB, GPIO_PIN_0);  // Tänd LED
        uint64_t start_time = MTIME;

        uint8_t pressed = 0;
        uint32_t timer = 0;
        while(timer < 500) { // 0.5 sekunders fönster
            if(gpio_input_bit_get(GPIOA, GPIO_PIN_5) == 0) {
                pressed = 1;
                break;
            }
            delay_ms(1);
            timer++;
        }

        gpio_bit_reset(GPIOB, GPIO_PIN_0);  // Släck LED
        if (pressed) {
            uint64_t reaction_ticks = MTIME - start_time;
            uint32_t reaction_time_ms = reaction_ticks / TICKS_PER_MS;

            LCD_Clear(BLACK);
            LCD_ShowStr(10, 10, "Nice!", GREEN, BLACK);

            char msg[32];
            sprintf(msg, "Time: %d ms", reaction_time_ms);
            LCD_ShowStr(10, 40, (u8*)msg, CYAN, BLACK);
        } else {
            LCD_Clear(BLACK);
            LCD_ShowStr(10, 10, "Too Slow!", RED, BLACK);
        }

        delay_ms(3000); // Vänta innan ny runda
    }
}
