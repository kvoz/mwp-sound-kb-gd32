#include "main.h"

/* Uses GD32F10x library v2.2.2 */

#include "gd32f10x.h"
#include "systick.h"
#include "timers.h"
#include "i2c.h"

#include "sound.h"
#include "keyboard.h"

void system_sound_init(void);
void system_swtimers_init(void);
void system_kb_init(void);

int main(void) {
  systick_config();

  /* Hardware initialization. */
	system_swtimers_init();
	system_sound_init();
	system_kb_init();

  /* Registration of callback functions. */
  keyboard_reg_func(keyboard_col_sel_callback,
                         keyboard_col_unsel_callback,
                         keyboard_row_read_callback);

  sound_reg_func(max9768_write_callback,
                 sound_turn_on_callback,
                 sound_turn_off_callback,
                 sound_set_tone_callback);

  /* FSM initializations. */
  sound_init();
  keyboard_init();

	while(1) {
	  sound_fsm_call();
	  keyboard_fsm_call();
	}
}

void system_sound_init(void) {
  /* -----------------------------------------------------------------------
    Configuration of I2C communication driver (volume control)
    ----------------------------------------------------------------------- */

  /* enable GPIO clock */
  rcu_periph_clock_enable(RCU_GPIOB);
  rcu_periph_clock_disable(RCU_AF);
  /* enable I2C1 clock */
  rcu_periph_clock_enable(RCU_I2C1);

  /* I2C1 GPIO ports */
  /* connect PB10 to I2C1_SCL */
  /* connect PB11 to I2C1_SDA */
  gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_11 | GPIO_PIN_10);

	/* I2C clock configure */
	i2c_clock_config(I2C1, 100000, I2C_DTCY_2);
	/* I2C address configure:
	 * set to I2C mode communication address format 7 bit,
	 * own address set to zero, dual address mode disabled */
	i2c_mode_addr_config(I2C1, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0);
	/* enable I2C1 */
	i2c_enable(I2C1);
	/* enable acknowledge sending */
	i2c_ack_config(I2C1, I2C_ACK_ENABLE);

  /* -----------------------------------------------------------------------
    Configuration for sound output

    TIMER0 configuration: generate 1 PWM signal:
    TIMER0CLK = SystemCoreClock / 108 = 1 MHz
    TIMER0 channel3 for output
    ----------------------------------------------------------------------- */
  rcu_periph_clock_enable(RCU_GPIOA);
  rcu_periph_clock_enable(RCU_AF);

  /*Configure PA11(TIMER0 CH3) as alternate function*/
  gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);

  timer_oc_parameter_struct timer_ocintpara;
  timer_parameter_struct timer_initpara;

  rcu_periph_clock_enable(RCU_TIMER0);

  timer_deinit(TIMER0);

  /* TIMER0 configuration */
  timer_initpara.prescaler         = 107;
  timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
  timer_initpara.counterdirection  = TIMER_COUNTER_UP;
  timer_initpara.period            = 0;
  timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
  timer_initpara.repetitioncounter = 0;
  timer_init(TIMER0, &timer_initpara);

  /* CH3 configuration in PWM mode1 */
  timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
  timer_ocintpara.outputstate  = TIMER_CCX_DISABLE;
  timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
  timer_channel_output_config(TIMER0, TIMER_CH_3, &timer_ocintpara);

  /* CH3 configuration in PWM mode1 (duty cycle) */
  timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, 50);
  timer_channel_output_mode_config(TIMER0, TIMER_CH_3, TIMER_OC_MODE_PWM0);
  timer_channel_output_shadow_config(TIMER0, TIMER_CH_3, TIMER_OC_SHADOW_DISABLE);

  /* auto-reload preload enable */
  timer_auto_reload_shadow_enable(TIMER0);
  /* auto-reload preload enable */
  timer_enable(TIMER0);
}

void system_kb_init(void) {
  rcu_periph_clock_enable(RCU_GPIOA);
  rcu_periph_clock_enable(RCU_GPIOB);
  rcu_periph_clock_enable(RCU_GPIOC);
  rcu_periph_clock_enable(RCU_AF);

  gpio_init(GPIOC, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9);
  gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_8);

  gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
  gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_6);

  gpio_bit_write(GPIOB, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, SET);
  gpio_bit_write(GPIOC, GPIO_PIN_6, SET);
}

void system_swtimers_init(void) {
  /* ----------------------------------------------------------------------------
     TIMER3 Configuration:
     TIMER3CLK = SystemCoreClock/108 = 1MHz, the period is 1ms.
     ---------------------------------------------------------------------------- */
  timer_parameter_struct timer_initpara;

  rcu_periph_clock_enable(RCU_TIMER3);

  timer_deinit(TIMER3);
  /* initialize TIMER init parameter struct */
  timer_struct_para_init(&timer_initpara);
  /* TIMER1 configuration */
  timer_initpara.prescaler         = 107;
  timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
  timer_initpara.counterdirection  = TIMER_COUNTER_UP;
  timer_initpara.period            = 999;
  timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
  timer_init(TIMER3, &timer_initpara);

  timer_interrupt_flag_clear(TIMER3, TIMER_INT_FLAG_UP);
  timer_interrupt_enable(TIMER3, TIMER_INT_UP);
  timer_enable(TIMER3);

  nvic_irq_enable(TIMER3_IRQn, 0, 0);
}

void max9768_write_callback(uint8_t slave_addr, uint8_t data) {
  I2CPacket packet;
  packet.buffer = &data;
  packet.buffer_size = 1;
  packet.slave_addr = slave_addr;
  i2c_write(I2C1, &packet);
}

void sound_set_tone_callback(uint16_t tone) {
  uint16_t new_period;
  uint32_t tim_freq = 1000000;

  /* set timer period */
  new_period = (uint16_t) (tim_freq / tone);
  timer_autoreload_value_config(TIMER0, new_period);

  /* update pulse for current ARR (duty cycle 50%)
   * note: comment lines below for old school sound (more heavy) */
//  uint16_t new_pulse = (uint16_t) (new_period / 2);
//    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, new_pulse);
}

void sound_turn_on_callback(void) {
  timer_autoreload_value_config(TIMER0, 0);
  /* Enable timer channel output. */
  timer_channel_output_state_config(TIMER0, TIMER_CH_3, TIMER_CCX_ENABLE);
  /* Enable primary output from timer, for some timers (TIM0 or TIM7). */
  timer_primary_output_config(TIMER0, ENABLE);
  /* Enable timer counter. */
  timer_enable(TIMER0);
}

void sound_turn_off_callback(void) {
  /* Disable timer channel output. */
  timer_channel_output_state_config(TIMER0, TIMER_CH_3, TIMER_CCX_DISABLE);
  /* Disable primary output from timer, for some timers (TIM0 or TIM7) */
  timer_primary_output_config(TIMER0, DISABLE);
  /* Disable timer counter. */
  timer_disable(TIMER0);
}

/* Row GPIO configuration: input, pull-up */
const uint32_t kbRowPorts[] = {GPIOC, GPIOC, GPIOC, GPIOA};
const uint16_t kbRowPins[] = {GPIO_PIN_7, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_8};
/* Column GPIO configuration: simple output */
const uint32_t kbColPorts[] = {GPIOB, GPIOB, GPIOB, GPIOB, GPIOC};
const uint16_t kbColPins[] = {GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15, GPIO_PIN_6};

void keyboard_col_sel_callback(uint8_t col) {
  gpio_bit_write(kbColPorts[col], kbColPins[col], RESET);
}

void keyboard_col_unsel_callback(uint8_t col) {
  gpio_bit_write(kbColPorts[col], kbColPins[col], SET);
}

uint8_t keyboard_row_read_callback(uint8_t row) {
  return gpio_input_bit_get(kbRowPorts[row], kbRowPins[row]);
}

void TIMER3_Callback() {
  __disable_irq();
  {
    swtimer_process();
  }
  __enable_irq();
}
