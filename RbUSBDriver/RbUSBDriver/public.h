#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#pragma warning(disable:4200)  // nameless struct/union
#pragma warning(disable:4201)  // nameless struct/union
#pragma warning(disable:4115)  // named typedef in parenthesis
#pragma warning(disable:4214)  // bit field types other than int

#include <ntddk.h>
#include <usbdi.h>
#include <wdf.h>
#include <Wdfusb.h>
#include <usbdlib.h>
#include <usbbusif.h>

#ifndef _WORD_DEFINED
#define _WORD_DEFINED
typedef unsigned short  WORD;
typedef unsigned int	UINT;
#endif 

#include "structure.h"

#define DRIVER_CY001	"CY001: "
#define POOLTAG			'00YC'

#define  MAX_INSTANCE_NUMBER 8
#define  MAX_INTERFACES 8

// WDF�豸����
typedef struct _DEVICE_CONTEXT {
  WDFUSBDEVICE      UsbDevice;
  WDFUSBINTERFACE   UsbInterface;
  WDFUSBINTERFACE   MulInterfaces[MAX_INTERFACES];

  WDFQUEUE          IoRWQueue;
  WDFQUEUE          IoControlEntryQueue;
  WDFQUEUE          IoControlSerialQueue;
  WDFQUEUE          InterruptManualQueue;
  WDFQUEUE			AppSyncManualQueue;
  
  WDFUSBPIPE		UsbCtlPipe;
  WDFUSBPIPE        UsbIntOutPipe;
  WDFUSBPIPE        UsbIntInPipe;
  WDFUSBPIPE        UsbBulkInPipe;
  WDFUSBPIPE        UsbBulkOutPipe;
  BYTE				byPreLEDs;
  BYTE				byPre7Seg;
  
  ULONG				LastUSBErrorStatusValue; // ���ֵ���ϱ����£��������µĴ���ֵ
  USB_BUS_INTERFACE_USBDI_V1 busInterface;
  BOOLEAN			bIsDeviceHighSpeed;
  BOOLEAN			bIntPipeConfigured;
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

typedef struct _IRP_ENTRY
{
	LIST_ENTRY  entry;
	PIRP		pIrp;
}IRP_ENTRY, *PIRP_ENTRY;

//===============================��������

// ������ȡ������ĺ���ָ�룬
// ͨ��GetDeviceContext�ܴ���һ��ܶ���ȡ��һ��ָ��ṹ��DEVICE_CONTEXT��ָ�롣
// ʹ��GetDeviceContext��ǰ���ǣ�Ŀ������Ѿ�������һ������ΪDEVICE_CONTEXT�Ļ����顣
// �ڱ������У�������PnpAdd������Ϊ�豸��������DEVICE_CONTEXT�����顣
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, GetDeviceContext);

//===============================���ú���
NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject, 
    IN PUNICODE_STRING  RegistryPath
	);

NTSTATUS
PnpAdd(
    IN WDFDRIVER        Driver,
    IN PWDFDEVICE_INIT  DeviceInit
    );

NTSTATUS
PnpPrepareHardware(
    IN WDFDEVICE    Device,
    IN WDFCMRESLIST ResourceList,
    IN WDFCMRESLIST ResourceListTranslated
    );

NTSTATUS
PnpReleaseHardware(
    IN WDFDEVICE    Device,
    IN WDFCMRESLIST ResourceList
    );

NTSTATUS
CreateWDFQueues(
    WDFDEVICE Device,
    PDEVICE_CONTEXT pContext
    );

NTSTATUS
ConfigureUsbDevice(
    WDFDEVICE Device,
    PDEVICE_CONTEXT DeviceContext
    );

NTSTATUS
GetUsbPipes(
    PDEVICE_CONTEXT DeviceContext
    );

//================================��д����
VOID
InterruptRead(
    WDFUSBPIPE  Pipe,
    WDFMEMORY  Buffer,
    size_t  NumBytesTransferred,
    WDFCONTEXT  pContext
    );

VOID
BulkRead(
    IN WDFQUEUE  Queue,
    IN WDFREQUEST  Request,
    IN size_t  Length
    );

VOID
BulkWrite(
    IN WDFQUEUE  Queue,
    IN WDFREQUEST  Request,
    IN size_t  Length
    );

VOID
BulkReadComplete(
    IN WDFREQUEST  Request,
    IN WDFIOTARGET  Target,
    IN PWDF_REQUEST_COMPLETION_PARAMS  Params,
    IN WDFCONTEXT  pContext
    );

VOID
BulkWriteComplete(
    IN WDFREQUEST  Request,
    IN WDFIOTARGET  Target,
    IN PWDF_REQUEST_COMPLETION_PARAMS  Params,
    IN WDFCONTEXT  pContext
    );

VOID
DeviceIoControlParallel(
    IN WDFQUEUE  Queue,
    IN WDFREQUEST  Request,
    IN size_t  OutputBufferLength,
    IN size_t  InputBufferLength,
    IN ULONG  IoControlCode
    );

VOID
DeviceIoControlSerial(
    IN WDFQUEUE  Queue,
    IN WDFREQUEST  Request,
    IN size_t  OutputBufferLength,
    IN size_t  InputBufferLength,
    IN ULONG  IoControlCode
    );

//===================================PNP/Power�ӿ�
NTSTATUS 
PwrD0Entry(
	IN WDFDEVICE  Device, 
	IN WDF_POWER_DEVICE_STATE  PreviousState
	);

NTSTATUS 
PwrD0Exit(
	IN WDFDEVICE  Device, 
	IN WDF_POWER_DEVICE_STATE  TargetState
	);

NTSTATUS 
PwrD0ExitPreInterruptsDisabled(
	IN WDFDEVICE  Device, 
	IN WDF_POWER_DEVICE_STATE  TargetState
	);

VOID 
PnpSurpriseRemove(
	IN WDFDEVICE  Device
	);

NTSTATUS
InitPowerManagement(
	IN WDFDEVICE  Device
	);

char* 
PowerName(
	WDF_POWER_DEVICE_STATE PowerState
	);

//==================================���ýӿڣ�Util.cpp��
NTSTATUS
FirmwareReset(
	IN WDFDEVICE Device, 
	IN UCHAR resetBit
	);

NTSTATUS
FirmwareUpload(
	WDFDEVICE Device, 
	PUCHAR pData, 
	ULONG ulLen,
	WORD offset
	);

NTSTATUS
ReadRAM(
	WDFDEVICE Device,
	WDFREQUEST request,
	ULONG* pLen
	);

ULONG
GetCurrentFrameNumber(
	IN WDFDEVICE Device
	);

NTSTATUS
AbortPipe(
	IN WDFDEVICE Device, 
	IN ULONG nPipeNum
	);

void 
BulkPipeReadWrite_Ctl(
	IN WDFDEVICE Device, 
	IN WDFREQUEST Request
	);

NTSTATUS 
UsbSetOrClearFeature(
	WDFDEVICE Device,
	WDFREQUEST Request
	);

NTSTATUS
GetStringDes(
	USHORT shIndex,
	USHORT shLanID,
	VOID* pBufferOutput, 
	ULONG OutputBufferLength,
	ULONG* pulRetLen,
	PDEVICE_CONTEXT pContext
	);

NTSTATUS
UsbControlRequest(
	IN WDFDEVICE Device,
	IN WDFREQUEST Request
	);

void 
ControlRequestComplete(
    IN WDFREQUEST  Request,
    IN WDFIOTARGET  Target,
    IN PWDF_REQUEST_COMPLETION_PARAMS  Params,
    IN WDFCONTEXT  Context
    );

void
ClearSyncQueue(
	WDFDEVICE Device
	);

void 
CompleteSyncRequest(
	WDFDEVICE Device, 
	DRIVER_SYNC_ORDER_TYPE type,
	int info
	);

NTSTATUS 
InterruptReadStart(
	WDFDEVICE Device
	);

NTSTATUS
InterruptReadStop(
	WDFDEVICE Device);

NTSTATUS 
SetDigitron(
	IN WDFDEVICE Device, 
	IN UCHAR chSet
	);

NTSTATUS
GetDigitron(
	IN WDFDEVICE Device,
	IN UCHAR* pchGet
	);

NTSTATUS 
SetLEDs(
	IN WDFDEVICE Device,
	IN UCHAR chSet
	);

NTSTATUS 
GetLEDs(
	IN WDFDEVICE Device, 
	IN UCHAR* pchGet
	);

WDFUSBPIPE 
GetInterruptPipe(
	BOOLEAN bInput, 
	WDFDEVICE Device
	);

WDFUSBPIPE
GetBulkPipe(
	BOOLEAN bInput,
	WDFDEVICE Device
	);

__inline KDBG(int nLevel, char* msg, ...);
#endif