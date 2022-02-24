#ifndef _STRUCTURE_H___
#define _STRUCTURE_H___

#define CHUNK_SIZE	64

// Ӧ�ó���������ṹ�崫��upload���������
typedef struct{
	WORD  addr;// ��ʵ��ַ
	WORD  len; // ���峤��
	//UCHAR data[1];// ������
}FIRMWARE_UPLOAD, *PFIRMWARE_UPLOAD;

// ȡ��string������
typedef struct _GET_STRING_DESCRIPTOR
{
   UCHAR    Index;
   USHORT   LanguageId;
} GET_STRING_DESCRIPTOR, *PGET_STRING_DESCRIPTOR;

// Appͨ������ṹ������������Set Feature���
typedef struct _SET_FEATURE_CONTROL
{
	USHORT FeatureSelector;
	USHORT Index;
	BOOLEAN bSetOrClear;// TRUE: Set; FALSE: Clear.
} SET_FEATURE_CONTROL, *PSET_FEATURE_CONTROL;

typedef enum{
	ENTERD0,
	EXITD0,
	SURPRISE_REMOVE,
	DRIVER_SYNC_EXIT,
	DRIVER_SYNC_OTHER
}DRIVER_SYNC_ORDER_TYPE;

// ͬ����Ϣ���ݽṹ
typedef struct{
	DRIVER_SYNC_ORDER_TYPE type; // ��Ϣ����
	int info; // ��Ϣ����
}DriverSyncPackt, *PDriverSyncPackt;

// ����������ṹ������ȡ��������İ汾��Ϣ
typedef enum {
	DR_WDF,
	DR_WDM
}DRIVER_TYPE;

typedef enum {
	FW_CY001,
	FW_NOT_CY001
}FIRMWARE_TYPE;

typedef struct{
	DRIVER_TYPE DriverType; // WDF or WDM
	FIRMWARE_TYPE FirmwareType;// USB or Firmware
}DRIVER_VERSION, *PDRIVER_VERSION;

// ������/Vendor�Զ���������ƿ�
typedef struct 
{
	union {
      struct {
        BYTE  recepient:2;
        BYTE  Reserved:3;
        BYTE  Type:2;
        BYTE  bDirInput:1;
      } Request;
      BYTE  type;
    } type;

	UCHAR req;
	USHORT value;
	USHORT index;
	USHORT length;
	char   *buf;// ������length����
}USB_CTL_REQ, *PUSB_CTL_REQ;

#endif