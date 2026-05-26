#include "gd32vf103.h"
#include <stdlib.h>
#include <stdio.h>
#include "lcd.h"
#include "pwm.h"      // För buzzer
#include "vibrator.h"  // För vibrator

#define MTIME          (*(volatile uint64_t*)0xD1000000)
#define SYSTEM_CLOCK   108000000
#define TICKS_PER_MS   (SYSTEM_CLOCK / 4000)
#define MAX_RESULTS    5

// Funktionsprototyper
uint8_t is_button_pressed(void);
void delay_ms(uint32_t ms);
uint32_t get_random_delay(void);
void show_history(void);

typedef struct {
    uint32_t time_ms;
    uint8_t stimulus_type; // 0 = LED, 1 = Buzzer, 2 = Vibrator
} Result;

Result results[MAX_RESULTS];
uint8_t result_count = 0;

void show_history() {
    LCD_Clear(BLACK);
    LCD_ShowStr(10, 5, "Last 5 Results:", YELLOW, BLACK);
    
    char msg[40];
    for (int i = 0; i < result_count; i++) {
        sprintf(msg, "%d: %d ms (%s)", i + 1, 
                results[i].time_ms,
                results[i].stimulus_type == 0 ? "LED" :
                results[i].stimulus_type == 1 ? "Buzzer" : "Vibrator");
        LCD_ShowStr(10, 25 + (i * 19), (u8*)msg, RED, BLACK);
    }
    
    LCD_ShowStr(10, 170, "Press button", GREEN, BLACK);
    while (!is_button_pressed()); // Använd debounce-funktionen
    while (is_button_pressed());
    result_count = 0;
}

void delay_ms(uint32_t ms) {
    uint64_t start = MTIME;
    while ((MTIME - start) < (ms * TICKS_PER_MS));
}

uint32_t get_random_delay() {
    return (2 + (rand() % 5)) * 1000; // 2-6 seconds
}

uint8_t is_button_pressed() {
    static uint32_t last_time = 0;
    static uint8_t last_state = 1;
    uint8_t current_state = gpio_input_bit_get(GPIOA, GPIO_PIN_5);

    // Enkel debounce-check
    if (current_state != last_state) {
        last_time = MTIME; // Återställ debounce-timer
        last_state = current_state;
    }

    if ((MTIME - last_time) > (SYSTEM_CLOCK / 1000)) {
        return current_state == 0; // Knappen tryckt
    }
    return 0; // Knapp inte tryckt
}

int main() {
    // Initiera GPIO
    rcu_periph_clock_enable(RCU_GPIOB);  // För LED och vibrator
    rcu_periph_clock_enable(RCU_GPIOA);  // För knapp och buzzer
    rcu_periph_clock_enable(RCU_GPIOC);  // För LCD

    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);  // LED
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_5); // Knapp
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_15); // LCD

    // Initiera LCD
    Lcd_Init();
    LCD_Clear(BLACK);
    LCD_ShowStr(10, 10, "Reaction Timer", RED, BLACK);
    delay_ms(1000);

    // Initiera buzzer (PA6 / TIMER2 CH0)
    T2powerUpInitPWM(0x1);

    // Initiera vibrator (sköts nu endast i vibrator.c)
    vibrator_init();

    while (1) {
        LCD_Clear(BLACK);
        LCD_ShowStr(10, 10, "Get Ready... ", YELLOW, BLACK);
        delay_ms(get_random_delay());

        // Välj slumpmässig stimulus: 0 = LED, 1 = Buzzer, 2 = Vibrator
        int mode = rand() % 3;

        LCD_Clear(BLACK);
        char press_msg[20];
        sprintf(press_msg, "PRESS %s", mode == 0 ? "LED" : (mode == 1 ? "Buzzer" : "Vibrator"));
        LCD_ShowStr(10, 10, press_msg, GREEN, BLACK);
        uint64_t start_time = MTIME;

        // Aktivera stimulus
        if (mode == 0) {
            gpio_bit_set(GPIOB, GPIO_PIN_0);  // LED on
        } else if (mode == 1) {
            T2setPWMch0(586);  // Buzzer on (852 Hz)
        } else if (mode == 2) {
            vibrator_on();  // Vibrator on
        }

        // Vänta på knapptryckning, max 0.5 sek
        uint8_t pressed = 0;
        for (int timer = 0; timer < 500; timer++) {
            if (is_button_pressed()) {
                pressed = 1;
                break;
            }
            delay_ms(1);
        }

        // Deaktivera stimulus
        gpio_bit_reset(GPIOB, GPIO_PIN_0);  // LED off
        T2setPWMch0(0);  // Buzzer off
        vibrator_off();  // Vibrator off

        if (pressed) {
            uint64_t reaction_ticks = MTIME - start_time;
            uint32_t reaction_time_ms = reaction_ticks / TICKS_PER_MS;
            
            // Spara resultat
            results[result_count].time_ms = reaction_time_ms;
            results[result_count].stimulus_type = mode;
            result_count++;
        
            LCD_Clear(BLACK);
            LCD_ShowStr(10, 10, "Nice! ", GREEN, BLACK);
        
            char msg[40];
            sprintf(msg, "Time: %d ms (%s)", reaction_time_ms, mode == 0 ? "LED" : (mode == 1 ? "Buzzer" : "Vibrator"));
            LCD_ShowStr(5, 35, (u8*)msg, CYAN, BLACK);
            
            if (result_count >= MAX_RESULTS) {
                delay_ms(2000);
                show_history();
            }
        } else {
            LCD_Clear(BLACK);
            LCD_ShowStr(10, 10, "Too Slow! ", RED, BLACK);
        }
        
        delay_ms(1000);
    }
}