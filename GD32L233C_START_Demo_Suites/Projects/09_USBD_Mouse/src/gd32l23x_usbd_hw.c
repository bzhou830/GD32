#include "usbd_hw.h"

/*!
    \brief      configure the different system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    rcu_usbd_clock_config(RCU_USBDSRC_PLL);

    /* enable USB APB1 clock */
    rcu_periph_clock_enable(RCU_USBD);
}

/*
    \brief      configure the gpio peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
//    exti_interrupt_flag_clear(EXTI_18);
//    exti_init(EXTI_18, EXTI_INTERRUPT, EXTI_TRIG_RISING);
}

/*!
    \brief      configure interrupt priority
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    /* enable the USB low priority interrupt */
    nvic_irq_enable((uint8_t)USBD_LP_IRQn, 2U);
}
