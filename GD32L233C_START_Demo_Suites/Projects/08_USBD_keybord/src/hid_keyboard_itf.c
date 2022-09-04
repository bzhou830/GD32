#include "standard_hid_core.h"

typedef enum {
    CHAR_A = 1,
    CHAR_B,
    CHAR_C
} key_char;


#define KEY_A               0x04        //A 
#define KEY_B               0x05        //B 
#define KEY_C               0x06        //C 
#define KEY_D               0x07        //D 
#define KEY_E               0x08        //E 
#define KEY_F               0x09        //F 
#define KEY_G               0x0A        //G 
#define KEY_H               0x0B        //H 
#define KEY_I               0x0C        //I 
#define KEY_J               0x0D        //J 
#define KEY_K               0x0E        //K 
#define KEY_L               0x0F        //L 
#define KEY_M               0x10        //M 
#define KEY_N               0x11        //N 
#define KEY_O               0x12        //O 
#define KEY_P               0x13        //P
#define KEY_Q               0x14        //Q
#define KEY_R               0x15        //R
#define KEY_S               0x16        //S
#define KEY_T               0x17        //T
#define KEY_U               0x18        //U
#define KEY_V               0x19        //V
#define KEY_W               0x1A        //W
#define KEY_X               0x1B        //X
#define KEY_Y               0x1C        //Y
#define KEY_Z               0x1D        //Z 
#define KEY_1               0x1E         //1 !
#define KEY_2               0x1F         //2 @
#define KEY_3               0x20         //3
#define KEY_4               0x21         //4
#define KEY_5               0x22         //5
#define KEY_6               0x23         //6
#define KEY_7               0x24         //7 
#define KEY_8               0x25         //8
#define KEY_9               0x26         //9
#define KEY_0               0x27         //0 

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

void sleep(uint16_t val)
{
    for(uint16_t i=0;i<100;i++)
    {
        for(uint16_t j=0;j<val;j++)
        {
            __NOP();
        }
    }
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
        sleep(200);
        if(SET == gpio_input_bit_get(GPIOA, GPIO_PIN_0)) 
            return CHAR_B;
    }
    return 0U;
}

static void sendchar(usb_dev *udev, uint8_t c)
{
    standard_hid_handler *hid = (standard_hid_handler *)udev->class_data[0];
    hid->data[2] = c;
    hid_report_send(udev, hid->data, HID_IN_PACKET);
    sleep(1500);
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
            hid->data[2] = 0x05U;
            break;
        default:
            break;
        }

        if(0U != hid->data[2]) {
            //hid_report_send(udev, hid->data, HID_IN_PACKET);
            sendchar(udev, KEY_H);
            sendchar(udev, KEY_E);
            sendchar(udev, KEY_L);
            sendchar(udev, KEY_L);
            sendchar(udev, KEY_O);
            
            sendchar(udev, KEY_W);
            sendchar(udev, KEY_O);
            sendchar(udev, KEY_R);
            sendchar(udev, KEY_L);
            sendchar(udev, KEY_D);
        }
    }
}
