#include <stdio.h>
#include "custom_hid_core.h"
#include "usbd_hw.h"

usb_dev usbd_custom_hid;

extern hid_fop_handler fop_handler;

int main(void)
{
    /* system clocks configuration */
    rcu_config();

    custom_hid_itfop_register(&usbd_custom_hid, &fop_handler);

    /* USB device configuration */
    usbd_init(&usbd_custom_hid, &custom_hid_desc, &custom_hid_class);

    /* NVIC configuration */
    nvic_config();

    /* enable USB pull-up */
    usbd_connect(&usbd_custom_hid);

    /* wait for standard USB enumeration is finished */
    while(USBD_CONFIGURED != usbd_custom_hid.cur_status) {
    }
    while(1) {
    }
}
