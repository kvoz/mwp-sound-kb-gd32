#include "systick.h"
#include "gd32f10x.h"

volatile static float systick_count_1us = 0;
volatile static float systick_count_1ms = 0;

void systick_config(void) {
  /* systick clock source is from HCLK */
  systick_clksource_set(SYSTICK_CLKSOURCE_HCLK);
  systick_count_1us = (float)SystemCoreClock / 1000000;
  systick_count_1ms = (float)systick_count_1us * 1000;
}

void systick_delay_1us(uint32_t delay) {
  uint32_t ctl;

  /* reload the count value */
  SysTick->LOAD = (uint32_t)(delay * systick_count_1us);
  /* clear the current count value */
  SysTick->VAL = 0x0000U;
  /* enable the systick timer */
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
  /* wait for the COUNTFLAG flag set */
  do{
    ctl = SysTick->CTRL;
  }while((ctl&SysTick_CTRL_ENABLE_Msk)&&!(ctl & SysTick_CTRL_COUNTFLAG_Msk));
  /* disable the systick timer */
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  /* clear the current count value */
  SysTick->VAL = 0x0000U;
}

void systick_delay_1ms(uint32_t delay) {
  uint32_t ctl;

  /* reload the count value */
  SysTick->LOAD = (uint32_t)(delay * systick_count_1ms);
  /* clear the current count value */
  SysTick->VAL = 0x0000U;
  /* enable the systick timer */
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
  /* wait for the COUNTFLAG flag set */
  do{
    ctl = SysTick->CTRL;
  }while((ctl&SysTick_CTRL_ENABLE_Msk)&&!(ctl & SysTick_CTRL_COUNTFLAG_Msk));
  /* disable the systick timer */
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  /* clear the current count value */
  SysTick->VAL = 0x0000U;
}

