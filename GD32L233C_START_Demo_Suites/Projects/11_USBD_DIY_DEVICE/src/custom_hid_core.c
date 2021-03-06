#include "usbd_transc.h"
#include "custom_hid_core.h"
#include <string.h>

#define USBD_VID                     0x8090U
#define USBD_PID                     0x1234U

/* Note:it should use the C99 standard when compiling the below codes */
/* USB standard device descriptor */
usb_desc_dev custom_dev_desc = {
    .header =
    {
        .bLength          = USB_DEV_DESC_LEN,
        .bDescriptorType  = USB_DESCTYPE_DEV,
    },
    .bcdUSB                = 0x0200U,
    .bDeviceClass          = 0xFFU,  //用户自定义USB设备
    .bDeviceSubClass       = 0x00U,
    .bDeviceProtocol       = 0x00U,
    .bMaxPacketSize0       = USBD_EP0_MAX_SIZE,
    .idVendor              = USBD_VID,
    .idProduct             = USBD_PID,
    .bcdDevice             = 0x0100U,
    .iManufacturer         = STR_IDX_MFC,
    .iProduct              = STR_IDX_PRODUCT,
    .iSerialNumber         = STR_IDX_SERIAL,
    .bNumberConfigurations = USBD_CFG_MAX_NUM,
};

typedef struct {
    usb_desc_config         config;
    usb_desc_itf            diy_itf;
    usb_desc_ep             diy_epin;
    usb_desc_ep             diy_epout;
} usb_diy_desc_config_set;

/* USB device configure descriptor */
usb_diy_desc_config_set custom_config_desc = {
    .config =
    {
        .header =
        {
            .bLength         = sizeof(usb_desc_config),
            .bDescriptorType = USB_DESCTYPE_CONFIG
        },
        .wTotalLength         = DESC_LEN_CONFIG,  //�������������ϵ��ܳ��ȣ���������������������ӿ��������������������˵��������ȡ�
        .bNumInterfaces       = 0x01U,
        .bConfigurationValue  = 0x01U,
        .iConfiguration       = 0x00U,
        .bmAttributes         = 0x80U,
        .bMaxPower            = 0x32U
    },

    //�ӿ�������
    .diy_itf =
    {
        .header =
        {
            .bLength         = sizeof(usb_desc_itf),
            .bDescriptorType = USB_DESCTYPE_ITF
        },
        .bInterfaceNumber     = 0x00U,
        .bAlternateSetting    = 0x00U,
        .bNumEndpoints        = 0x02U,
        .bInterfaceClass      = USB_HID_CLASS,
        .bInterfaceSubClass   = 0x00U,
        .bInterfaceProtocol   = 0x00U,
        .iInterface           = 0x00U
    },

    //����˵�������
    .diy_epin =
    {
        .header =
        {
            .bLength         = sizeof(usb_desc_ep),
            .bDescriptorType = USB_DESCTYPE_EP
        },
        .bEndpointAddress     = CUSTOMHID_IN_EP,
        .bmAttributes         = USB_EP_ATTR_INT,
        .wMaxPacketSize       = CUSTOMHID_IN_PACKET, //�ö˵�����������˵�1��������Ϊ16�ֽڡ�
        .bInterval            = 0x20U //�˵��ѯ��ʱ�䣬0x20ms
    },

    //����˵�������
    .diy_epout =
    {
        .header =
        {
            .bLength         = sizeof(usb_desc_ep),
            .bDescriptorType = USB_DESCTYPE_EP
        },
        .bEndpointAddress     = CUSTOMHID_OUT_EP,
        .bmAttributes         = USB_EP_ATTR_INT,
        .wMaxPacketSize       = CUSTOMHID_OUT_PACKET,
        .bInterval            = 0x20U
    }
};

/* USB language ID descriptor */
static usb_desc_LANGID usbd_language_id_desc = {
    .header =
    {
        .bLength = sizeof(usb_desc_LANGID),
        .bDescriptorType = USB_DESCTYPE_STR
    },
    .wLANGID = ENG_LANGID
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
        .bLength         = USB_STRING_LEN(14U),
        .bDescriptorType = USB_DESCTYPE_STR,
    },
    .unicode_string = {'G', 'D', '3', '2', '-', 'C', 'u', 's', 't', 'o', 'm', 'H', 'I', 'D'}
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

usb_desc custom_hid_desc = {
    .dev_desc    = (uint8_t *) &custom_dev_desc,
    .config_desc = (uint8_t *) &custom_config_desc,
    .strings     = usbd_hid_strings
};

/* local function prototypes ('static') */
static uint8_t custom_hid_init(usb_dev *udev, uint8_t config_index);
static uint8_t custom_hid_deinit(usb_dev *udev, uint8_t config_index);
static uint8_t custom_hid_req_handler(usb_dev *udev, usb_req *req);
static void custom_hid_data_in(usb_dev *udev, uint8_t ep_num);
static void custom_hid_data_out(usb_dev *udev, uint8_t ep_num);

usb_class custom_hid_class = {
    .req_cmd       = 0xFFU,

    .init          = custom_hid_init,
    .deinit        = custom_hid_deinit,
    .req_process   = custom_hid_req_handler,
    .data_in       = custom_hid_data_in,
    .data_out      = custom_hid_data_out
};

const __ALIGNED(2) uint8_t customhid_report_descriptor[DESC_LEN_REPORT] = {
 //ÿ�п�ʼ�ĵ�һ�ֽ�Ϊ����Ŀ��ǰ׺��ǰ׺�ĸ�ʽΪ��
 //D7~D4��bTag��D3~D2��bType��D1~D0��bSize�����·ֱ��ÿ����Ŀע�͡�
 
 //����һ��ȫ�֣�bTypeΪ1����Ŀ������;ҳѡ��Ϊ��ͨ����Generic Desktop Page��
 //�����1�ֽ����ݣ�bSizeΪ1����������ֽ����Ͳ�ע���ˣ��Լ�����bSize���жϡ�
 0x05, 0x01, // USAGE_PAGE (Generic Desktop)
 
 //����һ���ֲ���bTypeΪ2����Ŀ����;ѡ��Ϊ0x00������ͨ����ҳ�У�
 //����;��δ����ģ����ʹ�ø���;�������ϣ���ôϵͳ���������
 //������׼ϵͳ�豸���Ӷ��ͳ���һ���û��Զ����HID�豸��
 0x09, 0x00, // USAGE (0)
 
 //����һ������Ŀ��bTypeΪ0����Ŀ�������ϣ������������0x01��ʾ
 //�ü�����һ��Ӧ�ü��ϡ�����������ǰ������;ҳ����;����Ϊ
 //�û��Զ��塣
 0xa1, 0x01, // COLLECTION (Application)

 //����һ��ȫ����Ŀ��˵���߼�ֵ��СֵΪ0��
 0x15, 0x00, //     LOGICAL_MINIMUM (0)
 
 //����һ��ȫ����Ŀ��˵���߼�ֵ���Ϊ255��
 0x25, 0xff, //     LOGICAL_MAXIMUM (255)
 
 //����һ���ֲ���Ŀ��˵����;����СֵΪ1��
 0x19, 0x01, //     USAGE_MINIMUM (1)
 
 //����һ���ֲ���Ŀ��˵����;�����ֵ8��
 0x29, 0x08, //     USAGE_MAXIMUM (8) 
 
 //����һ��ȫ����Ŀ��˵�������������Ϊ�˸���
 0x95, 0x08, //     REPORT_COUNT (8)
 
 //����һ��ȫ����Ŀ��˵��ÿ��������ĳ���Ϊ8bit����1�ֽڡ�
 0x75, 0x08, //     REPORT_SIZE (8)
 
 //����һ������Ŀ��˵����8������Ϊ8bit����������Ϊ���롣
 0x81, 0x02, //     INPUT (Data,Var,Abs)
 
 //����һ���ֲ���Ŀ��˵����;����СֵΪ1��
 0x19, 0x01, //     USAGE_MINIMUM (1)
 
 //����һ���ֲ���Ŀ��˵����;�����ֵ8��
 0x29, 0x08, //     USAGE_MAXIMUM (8) 
 
 //����һ������Ŀ������������ݣ�8�ֽڣ�ע��ǰ���ȫ����Ŀ����
 0x91, 0x02, //   OUTPUT (Data,Var,Abs)
 
 //�����������Ŀ�����ر�ǰ��ļ��ϡ�bSizeΪ0�����Ժ���û���ݡ�
 0xc0        // END_COLLECTION
};

/*!
    \brief      register HID interface operation functions
    \param[in]  udev: pointer to USB device instance
    \param[in]  hid_fop: HID operation functions structure
    \param[out] none
    \retval     USB device operation status
*/
uint8_t custom_hid_itfop_register(usb_dev *udev, hid_fop_handler *hid_fop)
{
    if(NULL != hid_fop) {
        udev->user_data = hid_fop;

        return USBD_OK;
    }

    return USBD_FAIL;
}

/*!
    \brief      send custom HID report
    \param[in]  udev: pointer to USB device instance
    \param[in]  report: pointer to HID report
    \param[in]  len: data length
    \param[out] none
    \retval     USB device operation status
*/
uint8_t custom_hid_report_send(usb_dev *udev, uint8_t *report, uint16_t len)
{
    usbd_ep_send(udev, CUSTOMHID_IN_EP, report, len);

    return USBD_OK;
}

/*!
    \brief      initialize the HID device
    \param[in]  udev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t custom_hid_init(usb_dev *udev, uint8_t config_index)
{
    static custom_hid_handler hid_handler;

    memset((void *)&hid_handler, 0, sizeof(custom_hid_handler));

    /* initialize the data endpoints */
    usbd_ep_init(udev, EP_BUF_SNG, HID_TX_ADDR, &(custom_hid_config_desc.hid_epin));
    usbd_ep_init(udev, EP_BUF_SNG, HID_RX_ADDR, &(custom_hid_config_desc.hid_epout));

    usbd_ep_recev(udev, CUSTOMHID_OUT_EP, hid_handler.data, 2U);

    udev->ep_transc[EP_ID(CUSTOMHID_IN_EP)][TRANSC_IN] = custom_hid_class.data_in;
    udev->ep_transc[EP_ID(CUSTOMHID_OUT_EP)][TRANSC_OUT] = custom_hid_class.data_out;

    udev->class_data[CUSTOM_HID_INTERFACE] = (void *)&hid_handler;

    if(udev->user_data != NULL) {
        for(uint8_t i = 0U; i < MAX_PERIPH_NUM; i++) {
            if(((hid_fop_handler *)udev->user_data)->periph_config[i] != NULL) {
                ((hid_fop_handler *)udev->user_data)->periph_config[i]();
            }
        }
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
static uint8_t custom_hid_deinit(usb_dev *udev, uint8_t config_index)
{
    /* deinitialize HID endpoints */
    usbd_ep_deinit(udev, CUSTOMHID_IN_EP);
    usbd_ep_deinit(udev, CUSTOMHID_OUT_EP);

    return USBD_OK;
}

/*!
    \brief      handle the HID class-specific requests
    \param[in]  udev: pointer to USB device instance
    \param[in]  req: device class-specific request
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t custom_hid_req_handler(usb_dev *udev, usb_req *req)
{
    uint8_t status = REQ_NOTSUPP;

    custom_hid_handler *hid = (custom_hid_handler *)udev->class_data[CUSTOM_HID_INTERFACE];

    switch(req->bRequest) {
    case USB_GET_DESCRIPTOR:
        if(USB_DESCTYPE_REPORT == (req->wValue >> 8)) {
            usb_transc_config(&udev->transc_in[0],
                              (uint8_t *)customhid_report_descriptor,
                              USB_MIN(DESC_LEN_REPORT, req->wLength),
                              0U);

            status = REQ_SUPP;
        } else if (USB_DESCTYPE_HID == (req->wValue >> 8U)) {
            usb_transc_config(&udev->transc_in[0U], 
                              (uint8_t *)(&(custom_hid_config_desc.hid_vendor)), 
                              USB_MIN(9U, req->wLength), 
                              0U);
        }
        break;

    case GET_REPORT:
        if(2U == req->wLength) {
            usb_transc_config(&udev->transc_in[0], hid->data, 2U, 0U);

            status = REQ_SUPP;
        }
        break;

    case GET_IDLE:
        usb_transc_config(&udev->transc_in[0], (uint8_t *)&hid->idlestate, 1U, 0U);

        status = REQ_SUPP;
        break;

    case GET_PROTOCOL:
        usb_transc_config(&udev->transc_in[0], (uint8_t *)&hid->protocol, 1U, 0U);

        status = REQ_SUPP;
        break;

    case SET_REPORT:
        hid->reportID = (uint8_t)(req->wValue);

        usb_transc_config(&udev->transc_out[0], hid->data, req->wLength, 0U);

        status = REQ_SUPP;
        break;

    case SET_IDLE:
        hid->idlestate = (uint8_t)(req->wValue >> 8);

        status = REQ_SUPP;
        break;

    case SET_PROTOCOL:
        hid->protocol = (uint8_t)(req->wValue);

        status = REQ_SUPP;
        break;

    default:
        break;
    }

    return status;
}

/*!
    \brief      handle custom HID data in transaction
    \param[in]  udev: pointer to USB device instance
    \param[in]  ep_num: endpoint number
    \param[out] none
    \retval     none
*/
static void custom_hid_data_in(usb_dev *udev, uint8_t ep_num)
{
    return;
}

/*!
    \brief      handle custom HID data out transaction
    \param[in]  udev: pointer to USB device instance
    \param[in]  ep_num: endpoint number
    \param[out] none
    \retval     none
*/
static void custom_hid_data_out(usb_dev *udev, uint8_t ep_num)
{
    custom_hid_handler *hid = (custom_hid_handler *)udev->class_data[CUSTOM_HID_INTERFACE];

    if(CUSTOMHID_OUT_EP == ep_num) 
    {
        if(hid->data[0] & 0x01)
            gpio_bit_set(GPIOA, GPIO_PIN_7);
        else
            gpio_bit_reset(GPIOA, GPIO_PIN_7);
        
        if(hid->data[0] & 0x02)
            gpio_bit_set(GPIOA, GPIO_PIN_8);
        else
            gpio_bit_reset(GPIOA, GPIO_PIN_8);
        
        if(hid->data[0] & 0x04)
            gpio_bit_set(GPIOC, GPIO_PIN_6);
        else
            gpio_bit_reset(GPIOC, GPIO_PIN_6);
        
        if(hid->data[0] & 0x08)
            gpio_bit_set(GPIOC, GPIO_PIN_7);
        else
            gpio_bit_reset(GPIOC, GPIO_PIN_7);

        if(hid->data[0] > 0x08)
        {
            /* turn off all LEDs */
            gpio_bit_reset(GPIOA, GPIO_PIN_7 | GPIO_PIN_8);
            gpio_bit_reset(GPIOC, GPIO_PIN_6 | GPIO_PIN_7);
        }
        usbd_ep_recev(udev, CUSTOMHID_OUT_EP, hid->data, 2U);
    }
}
