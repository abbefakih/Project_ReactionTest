#include "gd32vf103.h"
#include "vibrator.h"

void vibrator_init() {
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
    gpio_bit_reset(GPIOB, GPIO_PIN_14);
}

void vibrator_on() {
    gpio_bit_set(GPIOB, GPIO_PIN_14);
}

void vibrator_off() {
    gpio_bit_reset(GPIOB, GPIO_PIN_14);
}