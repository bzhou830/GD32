#include "usbd_transc.h"
#include "standard_hid_core.h"
#include <string.h>

#define USBD_VID                     		 0x28E9U
#define USBD_PID                     		 0x0380U

//重新定义报告描述符长度
#ifdef USB_HID_REPORT_DESC_LEN
#undef USB_HID_REPORT_DESC_LEN
#endif
#define USB_HID_REPORT_DESC_LEN          0x34U

#define USB_HID_CONFIG_DESC_LEN          0x22U

/* Note:it should use the C99 standard when compiling the below codes */
/* USB standard device descriptor */
usb_desc_dev hid_dev_desc = {
    .header =
    {
        .bLength          = USB_DEV_DESC_LEN,
        .bDescriptorType  = USB_DESCTYPE_DEV
    },
#ifdef LPM_ENABLED
    .bcdUSB                = 0x0201U,
#else
    .bcdUSB                = 0x0200U,
#endif /* LPM_ENABLED */
    .bDeviceClass          = 0x00U,
    .bDeviceSubClass       = 0x00U,
    .bDeviceProtocol       = 0x00U,
    .bMaxPacketSize0       = USBD_EP0_MAX_SIZE, //端点0大小
    .idVendor              = USBD_VID,
    .idProduct             = USBD_PID,
    .bcdDevice             = 0x0100U,
    .iManufacturer         = STR_IDX_MFC,
    .iProduct              = STR_IDX_PRODUCT,
    .iSerialNumber         = STR_IDX_SERIAL,
    .bNumberConfigurations = USBD_CFG_MAX_NUM
};

#ifdef LPM_ENABLED

/* BOS descriptor */
uint8_t USBD_BOSDesc[USB_BOS_DESC_LEN] = {
    0x05,
    USB_DESCTYPE_BOS,
    0x0C,
    0x00,
    0x01,  /* 1 device capability descriptor */

    /* device capability*/
    0x07,
    USB_DEVICE_CAPABITY,
    0x02,  /* bDevCapabilityType: USB2.0 EXTENSION */
    0x0E,  /* LPM capability bit set and BESL definition supported */
    0x06,  /* recommended baseline BESL value: 0x6(1000us) */
    0x00,
    0x00
};

#endif /* LPM_ENABLED */

usb_hid_desc_config_set hid_config_desc = {
    .config =
    {
        .header =
        {
            .bLength         = sizeof(usb_desc_config),
            .bDescriptorType = USB_DESCTYPE_CONFIG
        },
        .wTotalLength         = USB_HID_CONFIG_DESC_LEN,
        .bNumInterfaces       = 0x01U,
        .bConfigurationValue  = 0x01U,
        .iConfiguration       = 0x00U,
        .bmAttributes         = 0xA0U,
        .bMaxPower            = 0x32U
    },

    .hid_itf =
    {
        .header =
        {
            .bLength         = sizeof(usb_desc_itf),
            .bDescriptorType = USB_DESCTYPE_ITF
        },
        .bInterfaceNumber     = 0x00U,
        .bAlternateSetting    = 0x00U,
        .bNumEndpoints        = 0x01U,
        .bInterfaceClass      = USB_HID_CLASS,
        .bInterfaceSubClass   = USB_HID_SUBCLASS_BOOT_ITF,
        .bInterfaceProtocol   = USB_HID_PROTOCOL_KEYBOARD,
        .iInterface           = 0x00U
    },

    .hid_vendor =
    {
        .header =
        {
            .bLength         = sizeof(usb_desc_hid),
            .bDescriptorType = USB_DESCTYPE_HID
        },
        .bcdHID               = 0x0111U,
        .bCountryCode         = 0x00U,
        .bNumDescriptors      = 0x01U,
        .bDescriptorType      = USB_DESCTYPE_REPORT,
        .wDescriptorLength    = USB_HID_REPORT_DESC_LEN,
    },

    .hid_epin =
    {
        .header =
        {
            .bLength         = sizeof(usb_desc_ep),
            .bDescriptorType = USB_DESCTYPE_EP
        },
        .bEndpointAddress     = HID_IN_EP,
        .bmAttributes         = USB_EP_ATTR_INT,
        .wMaxPacketSize       = HID_IN_PACKET,
        .bInterval            = 0x0AU
    }
};

/* USB language ID Descriptor */
static usb_desc_LANGID usbd_language_id_desc = {
    .header =
    {
        .bLength         = sizeof(usb_desc_LANGID),
        .bDescriptorType = USB_DESCTYPE_STR
    },
    .wLANGID              = ENG_LANGID
};

/* USB manufacture string */
static usb_desc_str manufacturer_string = {
    .header =
    {
        .bLength         = USB_STRING_LEN(10U),
        .bDescriptorType = USB_DESCTYPE_STR,
    },
    .unicode_string = {'G', 'i', 'g', 'a', 'D', 'e', 'v', 'i', 'c', 'e'}
};

/* USB product string */
static usb_desc_str product_string = {
    .header =
    {
        .bLength         = USB_STRING_LEN(17U),
        .bDescriptorType = USB_DESCTYPE_STR,
    },
    .unicode_string = {'G', 'D', '3', '2', '-', 'U', 'S', 'B', '_', 'K', 'e', 'y', 'b', 'o', 'a', 'r', 'd'}
};

/* USB serial string */
static usb_desc_str serial_string = {
    .header =
    {
        .bLength         = USB_STRING_LEN(12U),
        .bDescriptorType = USB_DESCTYPE_STR,
    }
};

/* USB string descriptor set */
static uint8_t *usbd_hid_strings[] = {
    [STR_IDX_LANGID]  = (uint8_t *) &usbd_language_id_desc,
    [STR_IDX_MFC]     = (uint8_t *) &manufacturer_string,
    [STR_IDX_PRODUCT] = (uint8_t *) &product_string,
    [STR_IDX_SERIAL]  = (uint8_t *) &serial_string
};

usb_desc hid_desc = {
#ifdef LPM_ENABLED
    .bos_desc    = (uint8_t *) &USBD_BOSDesc,
#endif /* LPM_ENABLED */
    .dev_desc    = (uint8_t *) &hid_dev_desc,
    .config_desc = (uint8_t *) &hid_config_desc,
    .strings     = usbd_hid_strings
};

/* local function prototypes ('static') */
static uint8_t hid_init(usb_dev *udev, uint8_t config_index);
static uint8_t hid_deinit(usb_dev *udev, uint8_t config_index);
static uint8_t hid_req_handler(usb_dev *udev, usb_req *req);
static void hid_data_in_handler(usb_dev *udev, uint8_t ep_num);

usb_class hid_class = {
    .init            = hid_init,
    .deinit          = hid_deinit,
    .req_process     = hid_req_handler,
    .data_in         = hid_data_in_handler
};

const uint8_t hid_report_desc[USB_HID_REPORT_DESC_LEN] = {
//每行开始的第一字节为该条目的前缀，前缀的格式为：
 //D7~D4：bTag。D3~D2：bType；D1~D0：bSize。以下分别对每个条目注释。
 
 //这是一个全局（bType为1）条目，选择用途页为普通桌面Generic Desktop Page(0x01)
 //后面跟一字节数据（bSize为1），后面的字节数就不注释了，
 //自己根据bSize来判断。
 0x05, 0x01, // USAGE_PAGE (Generic Desktop)
 
 //这是一个局部（bType为2）条目，说明接下来的应用集合用途用于鼠标
 0x09, 0x02, // USAGE (Mouse)
 
 //这是一个主条目（bType为0）条目，开集合，后面跟的数据0x01表示
 //该集合是一个应用集合。它的性质在前面由用途页和用途定义为
 //普通桌面用的鼠标。
 0xa1, 0x01, // COLLECTION (Application)
 
 //这是一个局部条目。说明用途为指针集合
 0x09, 0x01, //   USAGE (Pointer)
 
 //这是一个主条目，开集合，后面跟的数据0x00表示该集合是一个
 //物理集合，用途由前面的局部条目定义为指针集合。
 0xa1, 0x00, //   COLLECTION (Physical)
 
 //这是一个全局条目，选择用途页为按键（Button Page(0x09)）
 0x05, 0x09, //     USAGE_PAGE (Button)
 
 //这是一个局部条目，说明用途的最小值为1。实际上是鼠标左键。
 0x19, 0x01, //     USAGE_MINIMUM (Button 1)
 
 //这是一个局部条目，说明用途的最大值为3。实际上是鼠标中键。
 0x29, 0x03, //     USAGE_MAXIMUM (Button 3)
 
 //这是一个全局条目，说明返回的数据的逻辑值（就是我们返回的数据域的值啦）
 //最小为0。因为我们这里用Bit来表示一个数据域，因此最小为0，最大为1。
 0x15, 0x00, //     LOGICAL_MINIMUM (0)
 
 //这是一个全局条目，说明逻辑值最大为1。
 0x25, 0x01, //     LOGICAL_MAXIMUM (1)
 
 //这是一个全局条目，说明数据域的数量为三个。
 0x95, 0x03, //     REPORT_COUNT (3)
 
 //这是一个全局条目，说明每个数据域的长度为1个bit。
 0x75, 0x01, //     REPORT_SIZE (1)
 
 //这是一个主条目，说明有3个长度为1bit的数据域（数量和长度
 //由前面的两个全局条目所定义）用来做为输入，
 //属性为：Data,Var,Abs。Data表示这些数据可以变动，Var表示
 //这些数据域是独立的，每个域表示一个意思。Abs表示绝对值。
 //这样定义的结果就是，第一个数据域bit0表示按键1（左键）是否按下，
 //第二个数据域bit1表示按键2（右键）是否按下，第三个数据域bit2表示
 //按键3（中键）是否按下。
 0x81, 0x02, //     INPUT (Data,Var,Abs)
 
 //这是一个全局条目，说明数据域数量为1个
 0x95, 0x01, //     REPORT_COUNT (1)
 
 //这是一个全局条目，说明每个数据域的长度为5bit。
 0x75, 0x05, //     REPORT_SIZE (5)
 
 //这是一个主条目，输入用，由前面两个全局条目可知，长度为5bit，
 //数量为1个。它的属性为常量（即返回的数据一直是0）。
 //这个只是为了凑齐一个字节（前面用了3个bit）而填充的一些数据
 //而已，所以它是没有实际用途的。
 0x81, 0x03, //     INPUT (Cnst,Var,Abs)
 
 //这是一个全局条目，选择用途页为普通桌面Generic Desktop Page(0x01)
 0x05, 0x01, //     USAGE_PAGE (Generic Desktop)
 
 //这是一个局部条目，说明用途为X轴
 0x09, 0x30, //     USAGE (X)
 
 //这是一个局部条目，说明用途为Y轴
 0x09, 0x31, //     USAGE (Y)
 
 //这是一个局部条目，说明用途为滚轮
 0x09, 0x38, //     USAGE (Wheel)
 
 //下面两个为全局条目，说明返回的逻辑最小和最大值。
 //因为鼠标指针移动时，通常是用相对值来表示的，
 //相对值的意思就是，当指针移动时，只发送移动量。
 //往右移动时，X值为正；往下移动时，Y值为正。
 //对于滚轮，当滚轮往上滚时，值为正。
 0x15, 0x81, //     LOGICAL_MINIMUM (-127)
 0x25, 0x7f, //     LOGICAL_MAXIMUM (127)
 
 //这是一个全局条目，说明数据域的长度为8bit。
 0x75, 0x08, //     REPORT_SIZE (8)
 
 //这是一个全局条目，说明数据域的个数为3个。
 0x95, 0x03, //     REPORT_COUNT (3)
 
 //这是一个主条目。它说明这三个8bit的数据域是输入用的，
 //属性为：Data,Var,Rel。Data说明数据是可以变的，Var说明
 //这些数据域是独立的，即第一个8bit表示X轴，第二个8bit表示
 //Y轴，第三个8bit表示滚轮。Rel表示这些值是相对值。
 0x81, 0x06, //     INPUT (Data,Var,Rel)
 
 //下面这两个主条目用来关闭前面的集合用。
 //我们开了两个集合，所以要关两次。bSize为0，所以后面没数据。
 0xc0,       //   END_COLLECTION
 0xc0        // END_COLLECTION
};

/*!
    \brief      register HID interface operation functions
    \param[in]  udev: pointer to USB device instance
    \param[in]  hid_fop: HID operation functions structure
    \param[out] none
    \retval     USB device operation status
*/
uint8_t hid_itfop_register(usb_dev *udev, hid_fop_handler *hid_fop)
{
    if(NULL != hid_fop) {
        udev->user_data = (void *)hid_fop;

        return USBD_OK;
    }

    return USBD_FAIL;
}

/*!
    \brief      send HID report
    \param[in]  udev: pointer to USB device instance
    \param[in]  report: pointer to HID report
    \param[in]  len: data length
    \param[out] none
    \retval     USB device operation status
*/
uint8_t hid_report_send(usb_dev *udev, uint8_t *report, uint16_t len)
{
    standard_hid_handler *hid = (standard_hid_handler *)udev->class_data[USBD_HID_INTERFACE];

    /* check if USB is configured */
    hid->prev_transfer_complete = 0U;

    usbd_ep_send(udev, HID_IN_EP, report, len);

    return USBD_OK;
}

/*!
    \brief      initialize the HID device
    \param[in]  udev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t hid_init(usb_dev *udev, uint8_t config_index)
{
    static standard_hid_handler hid_handler;

    memset((void *)&hid_handler, 0, sizeof(standard_hid_handler));

    /* initialize TX endpoint */
    usbd_ep_init(udev, EP_BUF_SNG, INT_TX_ADDR, &(hid_config_desc.hid_epin));

    udev->ep_transc[EP_ID(HID_IN_EP)][TRANSC_IN] = hid_class.data_in;

    hid_handler.prev_transfer_complete = 1U;

    udev->class_data[USBD_HID_INTERFACE] = (void *)&hid_handler;

    if(NULL != udev->user_data) {
        ((hid_fop_handler *)udev->user_data)->hid_itf_config();
    }

    return USBD_OK;
}

/*!
    \brief      de-initialize the HID device
    \param[in]  udev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t hid_deinit(usb_dev *udev, uint8_t config_index)
{
    /* deinitialize HID endpoints */
    usbd_ep_deinit(udev, HID_IN_EP);

    return USBD_OK;
}

/*!
    \brief      handle the HID class-specific requests
    \param[in]  udev: pointer to USB device instance
    \param[in]  req: device class-specific request
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t hid_req_handler(usb_dev *udev, usb_req *req)
{
    uint8_t status = REQ_NOTSUPP;

    standard_hid_handler *hid = (standard_hid_handler *)udev->class_data[USBD_HID_INTERFACE];

    switch(req->bRequest) {
    case GET_REPORT:
        /* no use for this driver */
        break;

    case GET_IDLE:
        usb_transc_config(&udev->transc_in[0U], (uint8_t *)&hid->idle_state, 1U, 0U);

        status = REQ_SUPP;
        break;

    case GET_PROTOCOL:
        usb_transc_config(&udev->transc_in[0U], (uint8_t *)&hid->protocol, 1U, 0U);

        status = REQ_SUPP;
        break;

    case SET_REPORT:
        /* no use for this driver */
        break;

    case SET_IDLE:
        hid->idle_state = (uint8_t)(req->wValue >> 8);

        status = REQ_SUPP;
        break;

    case SET_PROTOCOL:
        hid->protocol = (uint8_t)(req->wValue);

        status = REQ_SUPP;
        break;

    case USB_GET_DESCRIPTOR:
        if(USB_DESCTYPE_REPORT == (req->wValue >> 8)) {
            usb_transc_config(&udev->transc_in[0U],
                              (uint8_t *)hid_report_desc,
                              USB_MIN(USB_HID_REPORT_DESC_LEN, req->wLength),
                              0U);

            status = REQ_SUPP;
        } else if(USB_DESCTYPE_HID == (req->wValue >> 8U)) {
            usb_transc_config(&udev->transc_in[0U],
                              (uint8_t *)(&(hid_config_desc.hid_vendor)),
                              USB_MIN(9U, req->wLength),
                              0U);
        }
        break;

    default:
        break;
    }

    return status;
}

/*!
    \brief      handle data stage in DATA IN transaction
    \param[in]  udev: pointer to USB device instance
    \param[in]  ep_num: endpoint identifier
    \param[out] none
    \retval     none
*/
static void hid_data_in_handler(usb_dev *udev, uint8_t ep_num)
{
    standard_hid_handler *hid = (standard_hid_handler *)udev->class_data[USBD_HID_INTERFACE];

    if(hid->data[2]) {
        hid->data[2] = 0x00U;

        usbd_ep_send(udev, HID_IN_EP, hid->data, HID_IN_PACKET);
    } else {
        hid->prev_transfer_complete = 1U;
    }
}
