#include "standard_hid_core.h"

typedef enum {
    CHAR_A = 1,
    CHAR_B,
    CHAR_C
} key_char;

/* local function prototypes ('static') */
static void key_config(void);
static uint8_t key_state(void);
static void hid_key_data_send(usb_dev *udev);

hid_fop_handler fop_handler = {
    .hid_itf_config = key_config,
    .hid_itf_data_process = hid_key_data_send
};

/*!
    \brief      configure the keys
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void key_config(void)
{
	  rcu_periph_clock_enable(RCU_GPIOA);
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);
    //nvic_irq_enable(WAKEUP_KEY_EXTI_IRQn, 0U);
}

/*!
    \brief      to get usb keyboard state
    \param[in]  none
    \param[out] none
    \retval     the char
*/
static uint8_t key_state(void)
{
    if(SET == gpio_input_bit_get(GPIOA, GPIO_PIN_0)) 
		{
				return CHAR_B;
    }
    return 0U;
}

static void hid_key_data_send(usb_dev *udev)
{
    standard_hid_handler *hid = (standard_hid_handler *)udev->class_data[0];

    if(hid->prev_transfer_complete) {
        switch(key_state()) {
        case CHAR_A:
            hid->data[2] = 0x04U;
            break;
        case CHAR_B:
					  hid->data[2] = -5;// 鼠标上移5个单位
            break;
        default:
            break;
        }

        if(0U != hid->data[2]) {
            hid_report_send(udev, hid->data, 4);
        }
    }
}
