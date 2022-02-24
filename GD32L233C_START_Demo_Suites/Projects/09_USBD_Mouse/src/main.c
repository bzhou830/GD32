#include "standard_hid_core.h"
#include "usbd_hw.h"

usb_dev usb_hid;
extern hid_fop_handler fop_handler;

/*!
    \brief      main routine
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* system clocks configuration */
    rcu_config();
	
	  //gpio_config();

	  hid_itfop_register(&usb_hid, &fop_handler);
	
    /* USB device configuration */
    usbd_init(&usb_hid, &hid_desc, &hid_class);

    /* NVIC configuration */
    nvic_config();

    /* enable USB pull-up */
    usbd_connect(&usb_hid);

    while(USBD_CONFIGURED != usb_hid.cur_status){
        /* wait for standard USB enumeration is finished */
    }

    while(1) {
        fop_handler.hid_itf_data_process(&usb_hid);
    }
}
