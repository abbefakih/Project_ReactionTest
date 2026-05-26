#include "pwm.h"

void T2powerUpInitPWM(uint16_t ch) {
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);
    if (ch & 0x1) gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);

    rcu_periph_clock_enable(RCU_TIMER2);
    timer_deinit(TIMER2);

    // 108MHz / (107+1) = 1 MHz
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 107;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 1173; // 1 MHz / 1173 = 852 Hz
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2, &timer_initpara);

    timer_channel_output_struct_para_init(&timer_ocinitpara);
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    if (ch & 0x1) {
        timer_channel_output_config(TIMER2, TIMER_CH_0, &timer_ocinitpara);
        timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_0, 0); // Tyst från start
        timer_channel_output_mode_config(TIMER2, TIMER_CH_0, TIMER_OC_MODE_PWM0);
        timer_channel_output_shadow_config(TIMER2, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
    }

    timer_auto_reload_shadow_enable(TIMER2);
    timer_enable(TIMER2);
}

void T2setPWMch0(int value) {
    timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_0, value);
}
