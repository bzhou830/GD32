#include <initguid.h>
#include "public.h"
#include "ioctl.h"
#include <ntstrsafe.h>

#pragma code_seg("PAGE")

// ���д���
VOID DeviceIoControlParallel(IN WDFQUEUE  Queue,
						IN WDFREQUEST  Request,
						IN size_t  OutputBufferLength,
						IN size_t  InputBufferLength,
						IN ULONG  IoControlCode)
{
	NTSTATUS status = STATUS_SUCCESS;
	ULONG ulRetLen = 0;

	size_t size = 0;
	void* pBufferInput = NULL;
	void* pBufferOutput = NULL;
	WDFDEVICE Device = WdfIoQueueGetDevice(Queue);// ȡ���豸���
	PDEVICE_CONTEXT pContext = GetDeviceContext(Device); // ȡ��WDF�豸����Ļ�����ָ��

	KDBG(DPFLTR_INFO_LEVEL, "[DeviceIoControlParallel] CtlCode:0x%0.8X", IoControlCode);

	// ȡ�����뻺�������ж�����Ч��
	if(InputBufferLength){
		status = WdfRequestRetrieveInputBuffer(Request, InputBufferLength, &pBufferInput, &size);
		if(status != STATUS_SUCCESS || pBufferInput == NULL || size < InputBufferLength){
			WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
			return;
		}
	}

	// ȡ��������������ж�����Ч��
	if(OutputBufferLength){
		status = WdfRequestRetrieveOutputBuffer(Request, OutputBufferLength, &pBufferOutput, &size);
		if(status != STATUS_SUCCESS || pBufferOutput == NULL || size < OutputBufferLength){
			WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
			return;
		}
	}

	//
	// ��������������̡�
	//
	switch(IoControlCode)
	{
		// ȡ�������İ汾��Ϣ
	case IOCTL_GET_DRIVER_VERSION:
		{
			PDRIVER_VERSION pVersion = (PDRIVER_VERSION)pBufferOutput;
			ULONG length;
			char tcsBuffer[120];
			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_GET_DRIVER_VERSION");

			if(OutputBufferLength < sizeof(DRIVER_VERSION)){
				status = STATUS_BUFFER_TOO_SMALL;
				break;
			}

			pVersion->DriverType = DR_WDF;
			pVersion->FirmwareType = FW_NOT_CY001;
			ulRetLen = sizeof(DRIVER_VERSION);// ��ʾ���س���

			// ����String���������ж�Firmware�����Ƿ��Ѿ������ء�
			GetStringDes(2, 0, tcsBuffer, 120, &length, pContext);

			if(length){
				WCHAR* pCyName = L"CY001 V";
				size_t len;
				int nIndex;

				if(length < 8)
					break;

				RtlStringCchLengthW(pCyName, 7, &len);
				for(nIndex = 0; nIndex < len; nIndex++){
					if(pCyName[nIndex] != ((WCHAR*)tcsBuffer)[nIndex])
						break;
				}

				if(nIndex == len)
					pVersion->FirmwareType = FW_CY001; // ��ȫ�����˵���°�Firmware�Ѿ����ص������塣
			}
			break;
		}
		
		// �յ�App���͹�����һ��ͬ��Request������Ӧ�ð������浽ͬ��Queue�У��ȵ���ͬ���¼�������ʱ���ٴ�Queue��ȡ������ɡ�
	case IOCTL_USB_SYNC:
		KDBG(DPFLTR_INFO_LEVEL, "IOCTL_USB_SYNC");
		status = WdfRequestForwardToIoQueue(Request, pContext->AppSyncManualQueue);

		// ֱ�ӷ��أ�������WdfRequestComplete������
		// �����߽�����Ϊ�˶��ȴ������������ڽ�����ĳ��ʱ�̡�
		// �������ν���첽����֮Ҫ���ˡ�
		if(NT_SUCCESS(status))
			return;
		break;

		// ���ͬ�������е���������
	case IOCTL_USB_SYNC_RELEASE:
		KDBG(DPFLTR_INFO_LEVEL, "IOCTL_USB_SYNC");
		ClearSyncQueue(Device);
		break;

		// Ӧ�ó����˳���ȡ�����б�����������
	case IOCTL_APP_EXIT_CANCEL: 
			
		// ȡ��USB�豸������IO��������������ȡ������Pipe��IO������
		//WdfIoTargetStop(WdfUsbTargetDeviceGetIoTarget(pContext->UsbDevice), WdfIoTargetCancelSentIo);
		break;

		// ȡ�õ�ǰ�����ú�.��������Ϊ0,��Ϊ��WDF�����,0����������ǲ���֧�ֵġ�
	case IOCTL_USB_GET_CURRENT_CONFIG:
		{
			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_USB_GET_CURRENT_CONFIG");
			if(InputBufferLength < 4){
				status = STATUS_INVALID_PARAMETER;
				break;
			}

			*(PULONG)pBufferInput = 0;// ֱ�Ӹ�ֵ0��������ѡ��0�����á�Ҳ���Է���URB�����߻�ȡ��ǰ����ѡ�
			ulRetLen = sizeof(ULONG);
			break;
		}

	case IOCTL_USB_ABORTPIPE:
		{
			ULONG pipenum = *((PULONG) pBufferOutput);
			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_USB_ABORTPIPE");

			status = AbortPipe(Device, pipenum);
		}      
		break;

		// ��ȡPipe��Ϣ
	case IOCTL_USB_GET_PIPE_INFO:
		{
			// ������ȡPipe��Ϣ�����Ƶ���������С�
			BYTE byCurSettingIndex = 0;
			BYTE byPipeNum = 0;
			BYTE index;
			USB_INTERFACE_DESCRIPTOR  interfaceDescriptor;
			WDF_USB_PIPE_INFORMATION  pipeInfor;

			WDFUSBINTERFACE Interface = pContext->UsbInterface;// �ӿھ��

			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_USB_GET_PIPE_INFO");

			// ȡ��Pipe��������Pipe�����㻺��������
			byCurSettingIndex = WdfUsbInterfaceGetConfiguredSettingIndex(Interface); 
			WdfUsbInterfaceGetDescriptor(Interface, byCurSettingIndex, &interfaceDescriptor);
			byPipeNum = WdfUsbInterfaceGetNumConfiguredPipes(Interface);		

			if(OutputBufferLength < byPipeNum * sizeof(pipeInfor)){
				status = STATUS_BUFFER_TOO_SMALL; // ����������
			}else{

				ulRetLen = byPipeNum*sizeof(pipeInfor);

				// ������ȡȫ���ܵ���Ϣ����������������С�
				// Ӧ�ó���õ���������ʱ��ҲӦ��ʹ��WDF_USB_PIPE_INFORMATION�ṹ�������������
				for(index = 0; index < byPipeNum; index++)
				{
					WDF_USB_PIPE_INFORMATION_INIT(&pipeInfor);
					WdfUsbInterfaceGetEndpointInformation(Interface, byCurSettingIndex, index, &pipeInfor);
					RtlCopyMemory((PUCHAR)pBufferOutput + index*pipeInfor.Size, &pipeInfor, sizeof(pipeInfor));
				}
			}
		}

		break;

		// ��ȡ�豸������
	case IOCTL_USB_GET_DEVICE_DESCRIPTOR:
		{
			USB_DEVICE_DESCRIPTOR  UsbDeviceDescriptor;
			WdfUsbTargetDeviceGetDeviceDescriptor(pContext->UsbDevice, &UsbDeviceDescriptor);
			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_USB_GET_DEVICE_DESCRIPTOR");

			// �ж����뻺�����ĳ����Ƿ��㹻��
			if(OutputBufferLength < UsbDeviceDescriptor.bLength)
				status = STATUS_BUFFER_TOO_SMALL;
			else{
				RtlCopyMemory(pBufferOutput, &UsbDeviceDescriptor, UsbDeviceDescriptor.bLength);
				ulRetLen = UsbDeviceDescriptor.bLength;
			}

			break;
		}

		// ��ȡ�ַ���������
	case IOCTL_USB_GET_STRING_DESCRIPTOR:
		{
			PGET_STRING_DESCRIPTOR Input = (PGET_STRING_DESCRIPTOR)pBufferInput;
			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_USB_GET_STRING_DESCRIPTOR");
			status = GetStringDes(Input->Index, (UCHAR)(Input->LanguageId), pBufferOutput, (ULONG)OutputBufferLength, &ulRetLen, pContext);
			
			// ���ַ����ȵ���Ϊ�ֽڳ���
			if(NT_SUCCESS(status) && ulRetLen > 0)
				ulRetLen *= (sizeof(WCHAR)/sizeof(char));
			break;
		}

		// ��ȡ����������Ϣ��
	case IOCTL_USB_GET_CONFIGURATION_DESCRIPTOR:
		{
			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_USB_GET_CONFIGURATION_DESCRIPTOR");

			// ���Ȼ�������������ĳ��ȡ�
			status = WdfUsbTargetDeviceRetrieveConfigDescriptor(pContext->UsbDevice, NULL, (PUSHORT)&size);
			if(!NT_SUCCESS(status) && status != STATUS_BUFFER_TOO_SMALL)
				break;

			// ���������������
			if(OutputBufferLength < size)
				break;

			// ��ʽȡ��������������
			status = WdfUsbTargetDeviceRetrieveConfigDescriptor(pContext->UsbDevice, pBufferOutput, (PUSHORT)&size);
			if(!NT_SUCCESS(status))
				break;

			ulRetLen = (ULONG)size;
			break;
		}

		// ���ݿ�ѡֵ���ýӿ�
	case IOCTL_USB_SET_INTERFACE:
		{
			BYTE byAlterSetting = *(BYTE*)pBufferInput;
			WDFUSBINTERFACE usbInterface = pContext->UsbInterface;
			BYTE byCurSetting = WdfUsbInterfaceGetConfiguredSettingIndex(usbInterface); // ��ǰAlternateֵ

			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_USB_SETINTERFACE");

			if(InputBufferLength < 1 || OutputBufferLength < 1)
			{
				status = STATUS_BUFFER_TOO_SMALL;
				break;
			}
			
			// �������Ŀ�ѡֵ�뵱ǰ�Ĳ�ͬ�����������ýӿڣ�
			// ����ֱ�ӷ��ء�
			if(byCurSetting != byAlterSetting)
			{
				WDF_USB_INTERFACE_SELECT_SETTING_PARAMS par;
				WDF_USB_INTERFACE_SELECT_SETTING_PARAMS_INIT_SETTING(&par, byAlterSetting);
				status = WdfUsbInterfaceSelectSetting(usbInterface, NULL, &par);
			}

			*(BYTE*)pBufferOutput = byCurSetting;
			break;
		}

		// �̼�Rest���Զ��������Port Rest�������¡�
	case IOCTL_USB_FIRMWRAE_RESET:
		{
			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_USB_FIRMWRAE_RESET");
			if(InputBufferLength < 1 || pBufferInput == NULL)
				status = STATUS_INVALID_PARAMETER;
			else
				status = FirmwareReset(Device, *(char*)pBufferInput);

			break;
		}

		// ����USB���߶˿�
	case IOCTL_USB_PORT_RESET:
		{
			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_USB_PORT_RESET");			
			WdfUsbTargetDeviceResetPortSynchronously(pContext->UsbDevice);
			break;
		}

		// �ܵ�����
	case IOCTL_USB_PIPE_RESET:
		{
			UCHAR uchPipe;
			WDFUSBPIPE pipe = NULL;

			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_USB_PIPE_RESET");			

			if(InputBufferLength < 1){
				status = STATUS_INVALID_PARAMETER;
				break;
			}

			// ����ID�ҵ���Ӧ��Pipe
			uchPipe = *(UCHAR*)pBufferInput;
			pipe = WdfUsbInterfaceGetConfiguredPipe(pContext->UsbInterface, uchPipe, NULL);
			if(pipe == NULL){ 
				status = STATUS_INVALID_PARAMETER;
				break;
			}

			status = WdfUsbTargetPipeResetSynchronously(pipe, NULL, NULL);
			break;
		}

		// �жϹܵ��������ܵ���ǰ���ڽ��еĲ���
	case IOCTL_USB_PIPE_ABORT:
		{
			UCHAR uchPipe;
			WDFUSBPIPE pipe = NULL;

			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_USB_PIPE_ABORT");

			if(InputBufferLength < 1){
				status = STATUS_INVALID_PARAMETER;
				break;
			}

			// ����ID�ҵ���Ӧ��Pipe
			uchPipe = *(UCHAR*)pBufferInput;
			pipe = WdfUsbInterfaceGetConfiguredPipe(pContext->UsbInterface, uchPipe, NULL);
			if(pipe == NULL){ 
				status = STATUS_INVALID_PARAMETER;
				break;
			}
			
			status = WdfUsbTargetPipeAbortSynchronously(pipe, NULL, NULL);
			break;
		}

		// ȡ������������Ϣ���������ǰ����һ�η��ֵĴ��󱣴����豸����Ļ������С�
		// ����߼���Ȼʵ���ˣ���Ŀǰ�İ汾�У�Ӧ�ó���û����������ӿڡ�
	case IOCTL_USB_GET_LAST_ERROR:
		{
			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_USB_GET_LAST_ERROR");

			if (OutputBufferLength >= sizeof(ULONG))
				*((PULONG)pBufferOutput) = pContext->LastUSBErrorStatusValue;
			else
				status = STATUS_BUFFER_TOO_SMALL;

			ulRetLen = sizeof(ULONG);
			break;
		}

		// Clear feature����
	case IOCTL_USB_SET_CLEAR_FEATURE:
		{
			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_USB_SET_CLEAR_FEATURE");
			status = UsbSetOrClearFeature(Device, Request);
			break;
		}

		// ΪUSB�豸���ع̼����򡣴���ƫ�����������������֧������ƫ������������һ����֧��
		// ��ƫ����������£��̼�������һ��һ�εؼ��أ�
		// ����ƫ������������̼�������Ϊһ����һ���Ա����ء�
	case IOCTL_FIRMWARE_UPLOAD_OFFSET:
		{
			void* pData = pBufferOutput;
			WORD offset = 0;

			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_FIRMWARE_UPLOAD_OFFSET");

			if(InputBufferLength < sizeof(WORD)){
				status = STATUS_INVALID_PARAMETER;
				break;
			}

			offset = *(WORD*)pBufferInput;
			status = FirmwareUpload(WdfIoQueueGetDevice(Queue), pData, (ULONG)OutputBufferLength, offset);
			break;
		}

		// ΪUSB�豸���ع̼�����
	case IOCTL_FIRMWARE_UPLOAD:
		{
			void* pData = pBufferOutput;
			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_FIRMWARE_UPLOAD");
			status = FirmwareUpload(WdfIoQueueGetDevice(Queue), pData, (ULONG)InputBufferLength, 0);
			break;
		}

		// ��ȡ�������豸��RAM���ݡ�RAMҲ�����ڴ档
		// ÿ�δ�ͬһ��ַ��ȡ�����ݿ��ܲ�����ͬ���������й̼������ڲ������У�RAM�����������ݣ�������ʱ���ݣ���
	case IOCTL_FIRMWARE_READ_RAM:
		{
			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_FIRMWARE_READ_RAM");
			status = ReadRAM(WdfIoQueueGetDevice(Queue), Request, &ulRetLen);// inforVal�б����ȡ�ĳ���
			break;
		}

		// ����������
	default:
		{
			// һ��ת����SerialQueue��ȥ��			
			WdfRequestForwardToIoQueue(Request, pContext->IoControlSerialQueue);

			// ����ת��֮���������ֱ�ӷ��أ�ǧ�򲻿ɵ���WdfRequestComplete������
			// ����ᵼ��һ��Request��������εĴ���
			return;
		}
	}

	// �������
	WdfRequestCompleteWithInformation(Request, status, ulRetLen);
}

// �������IO�����Ǿ������л��ġ�һ������һ�������Ծ����ᷢ���������⡣
//
VOID DeviceIoControlSerial(IN WDFQUEUE  Queue,
						 IN WDFREQUEST  Request,
						 IN size_t  OutputBufferLength,
						 IN size_t  InputBufferLength,
						 IN ULONG  IoControlCode)
{
	NTSTATUS ntStatus = STATUS_SUCCESS;
	ULONG ulRetLen = 0;
	SIZE_T size;

	void* pBufferInput = NULL;
	void* pBufferOutput = NULL;
	WDFDEVICE Device = WdfIoQueueGetDevice(Queue);// ȡ���豸���
	PDEVICE_CONTEXT pContext = GetDeviceContext(Device); // ȡ��WDF�豸����Ļ�����ָ��

	KDBG(DPFLTR_INFO_LEVEL, "[DeviceIoControlSerial]");

	// ȡ������/���������
	if(InputBufferLength)WdfRequestRetrieveInputBuffer(Request, InputBufferLength, &pBufferInput, &size);
	if(OutputBufferLength)WdfRequestRetrieveOutputBuffer(Request, OutputBufferLength, &pBufferOutput, &size);

	switch(IoControlCode)
	{
		// ���������
	case IOCTL_USB_SET_DIGITRON:
		{
			CHAR ch = *(CHAR*)pBufferInput;
			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_USB_SET_DIGITRON");
			SetDigitron(Device, ch);
			break;
		}

		// �������
	case IOCTL_USB_GET_DIGITRON:
		{
			UCHAR* pCh = (UCHAR*)pBufferOutput;
			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_USB_GET_DIGITRON");
			GetDigitron(Device, pCh);
			ulRetLen = 1;
			break;
		}

		// ����LED�ƣ���4յ��
	case IOCTL_USB_SET_LEDs:
		{
			CHAR ch = *(CHAR*)pBufferInput;
			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_USB_SET_LEDs");
			SetLEDs(Device, ch);
			break;
		}

		// ��ȡLED�ƣ���4յ���ĵ�ǰ״̬
	case IOCTL_USB_GET_LEDs:
		{
			UCHAR* pCh = (UCHAR*)pBufferOutput;
			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_USB_GET_LEDs");
			GetLEDs(Device, pCh);
			ulRetLen = 1;
			break;
		}

		// �������
		// ��Ϊ��USBЭ��Ԥ�������Vendor�Զ������������(class)���
	case IOCTL_USB_CTL_REQUEST:
		{
			KDBG(DPFLTR_INFO_LEVEL, "IOCTL_USB_CTL_REQUEST");
			ntStatus = UsbControlRequest(Device, Request);
			if(NT_SUCCESS(ntStatus))return;
			break;
		}

		// �����ж϶�
	case IOCTL_START_INT_READ:	
		KDBG(DPFLTR_INFO_LEVEL, "IOCTL_START_INT_READ");	
		ntStatus = InterruptReadStart(Device);
		break;

		// ���Ƴ����Ͷ����������Ǳ������ģ�����Queue���Ŷӣ����Բ�Ҫ����������ǡ�
	case IOCTL_INT_READ_KEYs:
		KDBG(DPFLTR_INFO_LEVEL, "IOCTL_INT_READ_KEYs");
		ntStatus = WdfRequestForwardToIoQueue(Request, pContext->InterruptManualQueue);

		if(NT_SUCCESS(ntStatus))
			return;// �ɹ���ֱ�ӷ���;�첽��ɡ�
		break;

		// ��ֹ�ж϶�
	case IOCTL_STOP_INT_READ:
		KDBG(DPFLTR_INFO_LEVEL, "IOCTL_STOP_INT_READ");
		InterruptReadStop(Device);
		ntStatus = STATUS_SUCCESS;
		break;

	default:
		// ��Ӧ�õ����
		// ���ڲ���ʶ���IO�������������������
		KDBG(DPFLTR_INFO_LEVEL, "Unknown Request: %08x(%d)!!!", IoControlCode, IoControlCode);
		ntStatus = STATUS_INVALID_PARAMETER;
		break;
	}

	WdfRequestCompleteWithInformation(Request, ntStatus, ulRetLen);
	return;
}
