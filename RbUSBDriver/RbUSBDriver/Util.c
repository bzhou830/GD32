#include "public.h"
#include <ntstrsafe.h>

#define CY001_LOAD_REQUEST    0xA0
#define ANCHOR_LOAD_EXTERNAL  0xA3
#define MAX_INTERNAL_ADDRESS  0x4000
#define INTERNAL_RAM(address) ((address <= MAX_INTERNAL_ADDRESS) ? 1 : 0)

#define MCU_Ctl_REG    0x7F92
#define MCU_RESET_REG  0xE600

// ȡ���жϹܵ���
// ��bIn������ȡinput����Output�ܵ���
WDFUSBPIPE GetInterruptPipe(BOOLEAN bInPipe, WDFDEVICE Device)
{
	DEVICE_CONTEXT* pContext = NULL;
	WDFUSBINTERFACE Interface = NULL;
	WDFUSBPIPE	pipe = NULL;
	BYTE byNumPipes;
	BYTE by;

	WDF_USB_PIPE_INFORMATION pipeInfo;

	KDBG(DPFLTR_INFO_LEVEL, "[GetInterruptPipe]");

	ASSERT(Device);
	pContext = GetDeviceContext(Device);
	ASSERT(pContext);
	Interface = pContext->UsbInterface;
	ASSERT(Interface);

	// ���豸�������л�ȡ
	if(bInPipe)
		pipe = pContext->UsbIntInPipe;
	else
		pipe = pContext->UsbIntOutPipe;

	if(NULL == pipe)
	{
		byNumPipes = WdfUsbInterfaceGetNumConfiguredPipes(Interface);// �ܵ�����
		WDF_USB_PIPE_INFORMATION_INIT(&pipeInfo);

		// ö�ٹܵ������жϹܵ�����
		for(by = 0; by < byNumPipes; by++)
		{
			pipe = WdfUsbInterfaceGetConfiguredPipe(Interface, by, &pipeInfo);
			if(pipe)
			{
				if(pipeInfo.PipeType == WdfUsbPipeTypeInterrupt &&
					bInPipe == WdfUsbTargetPipeIsInEndpoint(pipe))
				{
					if(bInPipe)
						pContext->UsbIntInPipe = pipe;
					else
						pContext->UsbIntOutPipe = pipe;

					break;
				}

				pipe = NULL;
			}
		}
	}

	return pipe;
}

// ȡ���豸�������ܵ�������bIn������ȡinput����Output�ܵ���
WDFUSBPIPE GetBulkPipe(BOOLEAN bIn, WDFDEVICE Device)
{
	DEVICE_CONTEXT* pContext = NULL;
	WDFUSBINTERFACE Interface = NULL;
	WDFUSBPIPE	pipe = NULL;
	BYTE byNumPipes, by;

	WDF_USB_PIPE_INFORMATION pipeInfo;

	KDBG(DPFLTR_INFO_LEVEL, "[GetBulkPipe]");

	ASSERT(Device);
	pContext = GetDeviceContext(Device);
	ASSERT(pContext);
	Interface = pContext->UsbInterface;
	ASSERT(Interface);

	// ��ȡ�豸�������б���ľ��
	if(bIn)
		pipe = pContext->UsbBulkInPipe;
	else
		pipe = pContext->UsbBulkOutPipe;

	if(!pipe)
	{
		// �����Ч��˵����δ��ȡ��Ӧ�ڴ˽�һ��������
		byNumPipes = WdfUsbInterfaceGetNumConfiguredPipes(Interface);
		WDF_USB_PIPE_INFORMATION_INIT(&pipeInfo);

		for(by = 0; by < byNumPipes; by++)
		{
			pipe = WdfUsbInterfaceGetConfiguredPipe(Interface, by, &pipeInfo);
			if(pipe){
				if(pipeInfo.PipeType == WdfUsbPipeTypeInterrupt
					&& bIn == WdfUsbTargetPipeIsInEndpoint(pipe))
				{
					if(bIn)
						pContext->UsbBulkInPipe = pipe;
					else
						pContext->UsbBulkOutPipe = pipe;

					break; // �ҵ�������
				}

				pipe = NULL; // û���ҵ�
			}
		}
	}

	return pipe;
}

// �����ж϶�������Ϊ�����ǿ�����һ��������������
// ����Ӧ������Ȥ�ο���ص�WDM���룬Ϊ��ʵ��������ܣ�WDM��ʵ���൱�������������׳���
NTSTATUS InterruptReadStart(WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	WDF_USB_CONTINUOUS_READER_CONFIG interruptConfig;
	PDEVICE_CONTEXT pContext = NULL;
	WDFUSBPIPE	pipeInt = NULL;

	KDBG(DPFLTR_INFO_LEVEL, "[InterruptReadStart]");

	ASSERT(Device);
	pipeInt = GetInterruptPipe(TRUE, Device);
	pContext = GetDeviceContext(Device);
	
	if(NULL == pipeInt)
		status = STATUS_UNSUCCESSFUL;
	else{

		//WDF_IO_TARGET_STATE state;
		WDF_USB_PIPE_INFORMATION pipeInfo;
		WDF_USB_PIPE_INFORMATION_INIT(&pipeInfo);
		WdfUsbTargetPipeGetInformation(pipeInt, &pipeInfo);

		// ��ȡpipe IOTarget�ĵ�ǰ״̬
		//WdfIoTargetGetState(WdfUsbTargetPipeGetIoTarget(pipeInt), &state);

		// Ҫ�жϱ�־λ��
		// �ж�Pipeֻ��Ҫ����һ��continue���þͿ����ˡ���������ܵ���ֹ�����������ض������á�
		if(pContext->bIntPipeConfigured == FALSE)
		{
			WDF_USB_CONTINUOUS_READER_CONFIG_INIT(&interruptConfig, 
				InterruptRead,				// �ص�����ע�ᡣ���յ�һ�ζ������Ϣ�󣬴˺��������á�
				pContext,					// �ص���������
				pipeInfo.MaximumPacketSize	// ���豸��ȡ���ݵĳ���
				);

			status = WdfUsbTargetPipeConfigContinuousReader(pipeInt, &interruptConfig);
			if(NT_SUCCESS(status))pContext->bIntPipeConfigured = TRUE;
			else KDBG(DPFLTR_INFO_LEVEL, "Error! Status: %08x", status);
		}

		// ����Pipe�������ǵ�һ��������Ҳ�����Ǻ���������
		if(NT_SUCCESS(status))
			status = WdfIoTargetStart(WdfUsbTargetPipeGetIoTarget(pipeInt));		
	}

	if(!NT_SUCCESS(status))
		KDBG(DPFLTR_INFO_LEVEL, "WdfUsbTargetPipeConfigContinuousReader failed with status 0x%08x", status);

	return status;
}

// ֹͣ�ж϶�����
NTSTATUS InterruptReadStop(WDFDEVICE Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	WDFUSBPIPE	pipeInt = NULL;
	WDFREQUEST Request = NULL;
	PDEVICE_CONTEXT pContext = GetDeviceContext(Device);

	KDBG(DPFLTR_INFO_LEVEL, "[InterruptReadStop]");

	ASSERT(Device);
	pipeInt = GetInterruptPipe(TRUE, Device);

	if(pipeInt)
		// �������δ��ɵ�IO��������Cancel����
		WdfIoTargetStop(WdfUsbTargetPipeGetIoTarget(pipeInt), WdfIoTargetCancelSentIo);

	// ������ֶ������е�����δ���Request��
	// ���Queue����δ����״̬���᷵��STATUS_WDF_PAUSED��
	// �������������ᰤ��ȡ����Entry��ֱ������STATUS_NO_MORE_ENTRIES��	
	do{
		status = WdfIoQueueRetrieveNextRequest(pContext->InterruptManualQueue, &Request);

		if(NT_SUCCESS(status))
		{
			WdfRequestComplete(Request, STATUS_SUCCESS);
		}
	}while(status != STATUS_NO_MORE_ENTRIES && status != STATUS_WDF_PAUSED);

	return STATUS_SUCCESS;
}

void ClearSyncQueue(WDFDEVICE Device)
{
	NTSTATUS status;
	WDFREQUEST Request = NULL;
	PDEVICE_CONTEXT pContext = GetDeviceContext(Device);

	KDBG(DPFLTR_INFO_LEVEL, "[ClearSyncQueue]");

	// ���ͬ�������е�����ͬ��Request���˲����߼������溯����ͬ��
	do{
		status = WdfIoQueueRetrieveNextRequest(pContext->AppSyncManualQueue, &Request);

		if(NT_SUCCESS(status))
			WdfRequestComplete(Request, STATUS_SUCCESS);

	}while(status != STATUS_NO_MORE_ENTRIES && status != STATUS_WDF_PAUSED);
}

// ��ͬ��������ȡ��һ����ЧRequest��
NTSTATUS GetOneSyncRequest(WDFDEVICE Device, WDFREQUEST* pRequest)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PDEVICE_CONTEXT pContext = GetDeviceContext(Device);
	ASSERT(pRequest);
	*pRequest = NULL;

	if(pContext->AppSyncManualQueue)
		status = WdfIoQueueRetrieveNextRequest(pContext->AppSyncManualQueue, pRequest);

	return status;
}

// ���һ��ͬ��Request�����������Ϣ������Request��
void CompleteSyncRequest(WDFDEVICE Device, DRIVER_SYNC_ORDER_TYPE type, int info)
{
	WDFREQUEST Request;
	if(NT_SUCCESS(GetOneSyncRequest(Device, &Request)))
	{
		PDriverSyncPackt pData = NULL;

		if(!NT_SUCCESS(WdfRequestRetrieveOutputBuffer(Request, sizeof(DriverSyncPackt), &(void*)pData, NULL)))
			WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
		else{

			// ���Output�ṹ����
			pData->type = type;
			pData->info = info;
			WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, sizeof(DriverSyncPackt));
		}
	}
}

NTSTATUS SetDigitron(IN WDFDEVICE Device, IN UCHAR chSet)
{
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_CONTEXT pContext = GetDeviceContext(Device);
	WDF_USB_CONTROL_SETUP_PACKET controlPacket;
	WDF_MEMORY_DESCRIPTOR hMemDes;

	KDBG(DPFLTR_INFO_LEVEL, "[SetDigitron] %d", chSet);
	WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&hMemDes, &chSet, sizeof(UCHAR));

	WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR(
		&controlPacket,
		BmRequestHostToDevice,
		BmRequestToDevice,
		0xD2, // Vendor����
		0,
		0);

	status = WdfUsbTargetDeviceSendControlTransferSynchronously(
		pContext->UsbDevice,
		NULL,
		NULL,
		&controlPacket,
		&hMemDes,
		NULL);

	return status;
}

NTSTATUS GetDigitron(IN WDFDEVICE Device, OUT UCHAR* pchGet)
{
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	PDEVICE_CONTEXT pContext = GetDeviceContext(Device);
	WDF_USB_CONTROL_SETUP_PACKET controlPacket;
	WDFMEMORY hMem = NULL;
	WDFREQUEST newRequest = NULL;

	ASSERT(pchGet);
	KDBG(DPFLTR_INFO_LEVEL, "[GetDigitron]");

	// �����ڴ�������
	ntStatus = WdfMemoryCreatePreallocated(WDF_NO_OBJECT_ATTRIBUTES, pchGet, sizeof(UCHAR), &hMem);
	if(!NT_SUCCESS(ntStatus))
		return ntStatus;

	// ��ʼ����������
	WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR(
		&controlPacket,
		BmRequestDeviceToHost,// input����
		BmRequestToDevice,
		0xD4,// Vendor ����D4
		0, 0);

	// �����µ�WDF REQUEST����
	ntStatus = WdfRequestCreate(NULL, NULL, &newRequest);
	if(!NT_SUCCESS(ntStatus))
		return ntStatus;

	WdfUsbTargetDeviceFormatRequestForControlTransfer(pContext->UsbDevice, newRequest, &controlPacket, hMem, NULL);	

	if(NT_SUCCESS(ntStatus))
	{
		// ͬ������
		WDF_REQUEST_SEND_OPTIONS opt;
		WDF_REQUEST_SEND_OPTIONS_INIT(&opt, WDF_REQUEST_SEND_OPTION_SYNCHRONOUS);
		if(WdfRequestSend(newRequest, WdfDeviceGetIoTarget(Device), &opt))
		{
			WDF_REQUEST_COMPLETION_PARAMS par;
			WDF_REQUEST_COMPLETION_PARAMS_INIT(&par);
			WdfRequestGetCompletionParams(newRequest, &par);

			// �ж϶�ȡ�����ַ����ȡ�
			if(sizeof(UCHAR) != par.Parameters.Usb.Completion->Parameters.DeviceControlTransfer.Length)
				ntStatus = STATUS_UNSUCCESSFUL;
		}else
			ntStatus = STATUS_UNSUCCESSFUL;
	}

	// ͨ��WdfXxxCreate�����Ķ��󣬱���ɾ��
	WdfObjectDelete(newRequest);

	return ntStatus;
}

NTSTATUS SetLEDs(IN WDFDEVICE Device, IN UCHAR chSet)
{
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_CONTEXT pContext = GetDeviceContext(Device);
	WDF_USB_CONTROL_SETUP_PACKET controlPacket;
	WDF_MEMORY_DESCRIPTOR hMemDes;

	KDBG(DPFLTR_INFO_LEVEL, "[SetLEDs] %c", chSet);
	WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&hMemDes, &chSet, sizeof(UCHAR));

	WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR(
		&controlPacket,
		BmRequestHostToDevice,
		BmRequestToDevice,
		0xD1, // Vendor����
		0, 0);

	status = WdfUsbTargetDeviceSendControlTransferSynchronously(
		pContext->UsbDevice,
		NULL,
		NULL,
		&controlPacket,
		&hMemDes,
		NULL);

	return status;
}

NTSTATUS GetLEDs(IN WDFDEVICE Device, OUT UCHAR* pchGet)
{
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	PDEVICE_CONTEXT pContext = GetDeviceContext(Device);
	WDF_USB_CONTROL_SETUP_PACKET controlPacket;
	WDFMEMORY hMem = NULL;
	WDFREQUEST newRequest = NULL;

	KDBG(DPFLTR_INFO_LEVEL, "[GetLEDs]");
	ASSERT(pchGet);

	// �����ڴ�������
	ntStatus = WdfMemoryCreatePreallocated(WDF_NO_OBJECT_ATTRIBUTES, pchGet, sizeof(UCHAR), &hMem);
	if(!NT_SUCCESS(ntStatus))
		return ntStatus;

	// ��ʼ����������
	WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR(
		&controlPacket,
		BmRequestDeviceToHost,// input����
		BmRequestToDevice,
		0xD3,// Vendor ����D3
		0, 0);

	// ����WDF REQUEST����
	ntStatus = WdfRequestCreate(NULL, NULL, &newRequest);
	if(!NT_SUCCESS(ntStatus))
		return ntStatus;

	WdfUsbTargetDeviceFormatRequestForControlTransfer(pContext->UsbDevice, newRequest, &controlPacket, hMem, NULL);	

	if(NT_SUCCESS(ntStatus))
	{
		// ͬ������
		WDF_REQUEST_SEND_OPTIONS opt;
		WDF_REQUEST_SEND_OPTIONS_INIT(&opt, WDF_REQUEST_SEND_OPTION_SYNCHRONOUS);
		if(WdfRequestSend(newRequest, WdfDeviceGetIoTarget(Device), &opt))
		{
			WDF_REQUEST_COMPLETION_PARAMS par;
			WDF_REQUEST_COMPLETION_PARAMS_INIT(&par);
			WdfRequestGetCompletionParams(newRequest, &par);

			// �ж϶�ȡ�����ַ����ȡ�
			if(sizeof(UCHAR) != par.Parameters.Usb.Completion->Parameters.DeviceControlTransfer.Length)
				ntStatus = STATUS_UNSUCCESSFUL;
		}else
			ntStatus = STATUS_UNSUCCESSFUL;
	}

	// ͨ��WdfXxxCreate�����Ķ��󣬱���ɾ��
	WdfObjectDelete(newRequest);

	return ntStatus;
}

NTSTATUS GetStringDes(USHORT shIndex, USHORT shLanID, VOID* pBufferOutput, ULONG OutputBufferLength, ULONG* pulRetLen, PDEVICE_CONTEXT pContext)
{
	NTSTATUS status;

	USHORT  numCharacters;

	KDBG(DPFLTR_INFO_LEVEL, "[GetStringDes] index:%d", shIndex);
	ASSERT(pulRetLen);
	ASSERT(pContext);
	*pulRetLen = 0;

	// ����String��������һ���䳤�ַ����飬������ȡ���䳤��
	status = WdfUsbTargetDeviceQueryString(
		pContext->UsbDevice,
		NULL,
		NULL,
		NULL, // ������ַ���
		&numCharacters,
		(UCHAR)shIndex,
		shLanID
		);
	if(!NT_SUCCESS(status))
		return status;

	// �ж��������ĳ���
	if(OutputBufferLength < numCharacters){
		status = STATUS_BUFFER_TOO_SMALL;
		return status;
	}

	// �ٴ���ʽ��ȡ��String������
	status = WdfUsbTargetDeviceQueryString(pContext->UsbDevice,
		NULL,
		NULL,
		(PUSHORT)pBufferOutput,// Unicode�ַ���
		&numCharacters,
		(UCHAR)shIndex,
		shLanID
		);

	// ��ɲ���
	if(NT_SUCCESS(status)){
		((PUSHORT)pBufferOutput)[numCharacters] = L'\0';// �ֶ����ַ���ĩβ���NULL
		*pulRetLen = numCharacters+1;
	}
	return status;
}

NTSTATUS FirmwareReset(IN WDFDEVICE Device, IN UCHAR resetBit)
{
	NTSTATUS status = STATUS_SUCCESS;
	WDF_USB_CONTROL_SETUP_PACKET controlPacket;
	WDF_MEMORY_DESCRIPTOR memDescriptor;
	PDEVICE_CONTEXT Context = GetDeviceContext(Device);

	KDBG(DPFLTR_INFO_LEVEL, "[FirmwareReset]");

	WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&memDescriptor, &resetBit, 1);

	// д��ַMCU_RESET_REG
	WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR(
		&controlPacket,
		BmRequestHostToDevice,
		BmRequestToDevice,
		CY001_LOAD_REQUEST,// Vendor����
		MCU_RESET_REG,	   // ָ����ַ
		0);

	status = WdfUsbTargetDeviceSendControlTransferSynchronously(
		Context->UsbDevice, 
		NULL, NULL,	
		&controlPacket,
		&memDescriptor,
		NULL);

	if(!NT_SUCCESS(status))
		KDBG(DPFLTR_ERROR_LEVEL, "FirmwareReset failed: 0x%X!!!", status);

	return status;
}

// ��һ�ζ����ƵĹ̼�����д�뿪����ָ����ַ����
//
NTSTATUS FirmwareUpload(WDFDEVICE Device, PUCHAR pData, ULONG ulLen, WORD offset)
{
	NTSTATUS ntStatus = STATUS_SUCCESS;
	WDF_USB_CONTROL_SETUP_PACKET controlPacket;
	ULONG chunkCount = 0;
	ULONG ulWritten;
	WDF_MEMORY_DESCRIPTOR memDescriptor;
	PDEVICE_CONTEXT Context = GetDeviceContext(Device);
	ULONG i;

	chunkCount = ((ulLen + CHUNK_SIZE - 1) / CHUNK_SIZE);

	// Ϊ��ȫ��������ع����У�������ݱ��ָ����64�ֽ�Ϊ��λ��С����з��͡�
	// ����Դ����д��ݣ����ܻᷢ�����ݶ�ʧ�������
	//
	for (i = 0; i < chunkCount; i++)
	{
		// �����ڴ�������
		WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&memDescriptor, pData, (i < chunkCount-1)?
			CHUNK_SIZE : 
			(ulLen - (chunkCount-1) * CHUNK_SIZE));// ����������һ���飬��CHUNK_SIZE�ֽڣ�����Ҫ����β�ͳ��ȡ�

		// ��ʼ����������
		WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR(
			&controlPacket,
			BmRequestHostToDevice,
			BmRequestToDevice,
			CY001_LOAD_REQUEST,		// Vendor ����A3
			(USHORT)(offset + i*CHUNK_SIZE),  // д����ʼ��ַ
			0);

		ntStatus = WdfUsbTargetDeviceSendControlTransferSynchronously(
			Context->UsbDevice, 
			NULL, NULL, 
			&controlPacket,
			&memDescriptor, 
			&ulWritten);

		if (!NT_SUCCESS(ntStatus)){
			KDBG( DPFLTR_ERROR_LEVEL, "FirmwareUpload Failed :0x%0.8x!!!", ntStatus);
			break;
		}else			
			KDBG( DPFLTR_INFO_LEVEL, "%d bytes are written.", ulWritten);

		pData += CHUNK_SIZE;
	}

	return ntStatus;
}

// �ӿ������ڴ�ĵ�ָ����ַ����ȡ��ǰ����
//
NTSTATUS ReadRAM(WDFDEVICE Device, WDFREQUEST Request, ULONG* pLen)
{
	NTSTATUS ntStatus;
	WDF_USB_CONTROL_SETUP_PACKET controlPacket;    
	WDFMEMORY   hMem = NULL;
	PDEVICE_CONTEXT Context = GetDeviceContext(Device);
	PFIRMWARE_UPLOAD pUpLoad = NULL;
	WDFREQUEST newRequest;
	void* pData = NULL;
	size_t size;

	KDBG(DPFLTR_INFO_LEVEL, "[ReadRAM]");

	ASSERT(pLen);
	*pLen = 0;

	if(!NT_SUCCESS(WdfRequestRetrieveInputBuffer(Request, sizeof(FIRMWARE_UPLOAD), &(void*)pUpLoad, NULL)) ||
		!NT_SUCCESS(WdfRequestRetrieveOutputBuffer(Request, 1, &pData, &size)))
	{		
		KDBG( DPFLTR_ERROR_LEVEL, "Failed to retrieve memory handle\n");
		return STATUS_INVALID_PARAMETER;
	}

	// �����ڴ�������
	ntStatus = WdfMemoryCreatePreallocated(WDF_NO_OBJECT_ATTRIBUTES, pData, min(size, pUpLoad->len), &hMem);
	if(!NT_SUCCESS(ntStatus))
		return ntStatus;

	// ��ʼ����������
	WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR(	
		&controlPacket,
		BmRequestDeviceToHost,// input����
		BmRequestToDevice,
		CY001_LOAD_REQUEST,// Vendor ����A0
		pUpLoad->addr,// ��ַ
		0);

	// ��������ʼ��WDF REQUEST����
	ntStatus = WdfRequestCreate(NULL, NULL, &newRequest);
	if(!NT_SUCCESS(ntStatus))
		return ntStatus;

	WdfUsbTargetDeviceFormatRequestForControlTransfer(Context->UsbDevice,
		newRequest, &controlPacket, 
		hMem, NULL);	

	if(NT_SUCCESS(ntStatus))
	{
		WDF_REQUEST_SEND_OPTIONS opt;
		WDF_REQUEST_SEND_OPTIONS_INIT(&opt, WDF_REQUEST_SEND_OPTION_SYNCHRONOUS);
		if(WdfRequestSend(newRequest, WdfDeviceGetIoTarget(Device), &opt))
		{
			WDF_REQUEST_COMPLETION_PARAMS par;
			WDF_REQUEST_COMPLETION_PARAMS_INIT(&par);
			WdfRequestGetCompletionParams(newRequest, &par);

			// ȡ�ö�ȡ�����ַ����ȡ�
			*pLen = par.Parameters.Usb.Completion->Parameters.DeviceControlTransfer.Length;
		}
	}

	// ͨ��WdfXxxCreate�����Ķ��󣬱���ɾ��
	WdfObjectDelete(newRequest);

	return ntStatus;
}

// USB���ƶ˿������Щ�������USBԤ��������Vendor�Զ���������������ඨ������ȡ�
// 
NTSTATUS UsbControlRequest(IN WDFDEVICE Device, IN WDFREQUEST Request)
{
	NTSTATUS status;
	WDFREQUEST RequestNew = NULL;
	WDFMEMORY memHandle = NULL;
	WDF_USB_CONTROL_SETUP_PACKET controlPacket;
	WDF_REQUEST_SEND_OPTIONS  opt;

	PUSB_CTL_REQ pRequestControl;
	char* pOutputBuf;
	WDF_USB_BMREQUEST_DIRECTION dir;
	WDF_USB_BMREQUEST_RECIPIENT recipient;

	KDBG(DPFLTR_INFO_LEVEL, "[UsbControlRequest]");
	
	__try
	{
		// �������Ϊһ��USB_CTL_REQ���͵Ľṹ��
		status = WdfRequestRetrieveInputBuffer(Request, sizeof(USB_CTL_REQ)-1, &(void*)pRequestControl, NULL);
		if(!NT_SUCCESS(status))
			__leave;

		// ���������ΪUSB_CTL_REQ�е�buf��Ա��������̳���Ϊ1.
		status = WdfRequestRetrieveOutputBuffer(Request, max(1, pRequestControl->length), &pOutputBuf, NULL);
		if(!NT_SUCCESS(status))
			__leave;

		// �ж���������������
		if(pRequestControl->type.Request.bDirInput) 
			dir = BmRequestDeviceToHost;
		else
			dir = BmRequestHostToDevice;

		// USB�豸�еĽ��ܷ����������豸�����ӿڡ��˵㣬������֮���δ֪�ߡ�
		switch(pRequestControl->type.Request.recepient){
			case 0: 
				recipient = BMREQUEST_TO_DEVICE;
				break;
			case 1:
				recipient = BMREQUEST_TO_INTERFACE;
				break;
			case 2:
				recipient = BMREQUEST_TO_ENDPOINT;
				break;
			case 3:
			default:
				recipient = BMREQUEST_TO_OTHER;
		}

		// ��������ص����ݱ�������buf��Աָ���С�
		if(pRequestControl->length)
		{
			status = WdfMemoryCreatePreallocated(NULL, pOutputBuf, pRequestControl->length, &memHandle);
			if(!NT_SUCCESS(status))
				__leave;
		}

		KDBG(DPFLTR_INFO_LEVEL, "%s: recepient:%d type:%d",
			pRequestControl->type.Request.bDirInput?"In Req":"Out Req", 
			pRequestControl->type.Request.recepient, 
			pRequestControl->type.Request.Type);

		// ��ʼ��Setup�ṹ�塣WDF�ṩ��5�г�ʼ���꣬������ֱ���չʾ��
		if(pRequestControl->type.Request.Type == 0x1) // Class ����
		{			
			KDBG(DPFLTR_INFO_LEVEL, "Class Request");
			WDF_USB_CONTROL_SETUP_PACKET_INIT_CLASS (
				&controlPacket,
				dir, recipient,
				pRequestControl->req,
				pRequestControl->value,
				pRequestControl->index
				);
		}
		else if(pRequestControl->type.Request.Type == 0x2) // Vendor ����
		{
			if(pRequestControl->req == 0xA0 || (pRequestControl->req >= 0xD1 && pRequestControl->req <= 0xD5))
			{
				KDBG(DPFLTR_INFO_LEVEL, "Known Vendor Request.");// ��ʶ���Vendor ����
			}
			else
			{
				KDBG(DPFLTR_INFO_LEVEL, "Unknown Vendor Request. DANGER!!!");// ����ʶ��Σ��!!!!
			}

			WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR (
				&controlPacket,
				dir,
				recipient,
				pRequestControl->req,
				pRequestControl->value,
				pRequestControl->index
				);
		}
		else if(pRequestControl->type.Request.Type == 0x0) // ��׼����
		{
			KDBG(DPFLTR_INFO_LEVEL, "Standard Request");

			if(pRequestControl->req == 1 || pRequestControl->req == 3) // Feature����
			{
				KDBG(DPFLTR_INFO_LEVEL, "Feature Request");
				WDF_USB_CONTROL_SETUP_PACKET_INIT_FEATURE(
					&controlPacket,
					recipient,
					pRequestControl->value,
					pRequestControl->index,				
					pRequestControl->req == 1 ? FALSE: // clear
												TRUE   // set
				);
			}
			else if(pRequestControl->req == 0)							// Status����
			{			
				KDBG(DPFLTR_INFO_LEVEL, "Status Request");
				WDF_USB_CONTROL_SETUP_PACKET_INIT_GET_STATUS(
					&controlPacket,
					recipient,
					pRequestControl->index
				);
			}
			else														// ����
			{				
				WDF_USB_CONTROL_SETUP_PACKET_INIT (
					&controlPacket,
					dir, recipient,
					pRequestControl->req,
					pRequestControl->value,
					pRequestControl->index
					);
			}
		}

		// ����һ���µ�Request����
		status = WdfRequestCreate(NULL, WdfDeviceGetIoTarget(Device), &RequestNew);
		if(!NT_SUCCESS(status))
			__leave;

		// ���Usb�豸����controlPacket�ṹ��ʽ���´�����Request����
		status = WdfUsbTargetDeviceFormatRequestForControlTransfer(
			GetDeviceContext(Device)->UsbDevice,
			RequestNew, &controlPacket, 
			memHandle, NULL
			);

		if (!NT_SUCCESS(status)){
			KDBG( DPFLTR_ERROR_LEVEL, "WdfUsbTargetDeviceFormatRequestForControlTransfer failed");
			__leave;
		}

		// �첽����
		// ����Timeout��־���Է�ֹ�û��ڷ����˲���ʶ�����������������ڵȴ�������
		// �ȴ�����2�룬���󽫱�ȡ����
		WDF_REQUEST_SEND_OPTIONS_INIT(&opt, 0);
		WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(&opt, WDF_REL_TIMEOUT_IN_SEC(2));
		WdfRequestSetCompletionRoutine(RequestNew, ControlRequestComplete, Request);
		if(WdfRequestSend(RequestNew, WdfDeviceGetIoTarget(Device), &opt) == FALSE) 
		{
			status = WdfRequestGetStatus(RequestNew);
			KDBG(DPFLTR_ERROR_LEVEL, "WdfRequestSend failed");
			__leave;
		}		
	}
	__finally{
	}
	
	if(!NT_SUCCESS(status))
		KDBG(DPFLTR_ERROR_LEVEL, "status: %08X", status);
	
	return status;
}

void ControlRequestComplete(
    IN WDFREQUEST  Request,
    IN WDFIOTARGET  Target,
    IN PWDF_REQUEST_COMPLETION_PARAMS  Params,
    IN WDFCONTEXT  Context
    )
{
    UNREFERENCED_PARAMETER(Target);
	ULONG len = 0;
	WDFREQUEST OriginalReqeust = (WDFREQUEST)Context;
	NTSTATUS status = WdfRequestGetStatus(Request);
	
	KDBG(DPFLTR_INFO_LEVEL, "[ControlRequestComplete] status: %08X", status);

	if(status == STATUS_IO_TIMEOUT)
		KDBG(DPFLTR_ERROR_LEVEL, "the control request is time out, should be checked.");
	else
		len = Params->Parameters.Usb.Completion->Parameters.DeviceControlTransfer.Length;

	WdfObjectDelete(Request);
	WdfRequestCompleteWithInformation(OriginalReqeust, status, len);
}

// ͨ���ܵ�Item�����Եõ��ܵ����������Abort��������ֹPipe�ϵ���������
NTSTATUS AbortPipe(IN WDFDEVICE Device, IN ULONG nPipeNum)
{
	NTSTATUS status;
	PDEVICE_CONTEXT Context = GetDeviceContext(Device);
	WDFUSBINTERFACE Interface = Context->UsbInterface;
	WDFUSBPIPE pipe = WdfUsbInterfaceGetConfiguredPipe(Interface, (UCHAR)nPipeNum, NULL);

	KDBG(DPFLTR_INFO_LEVEL, "[AbortPipe]");

	if(pipe == NULL)
		return STATUS_INVALID_PARAMETER;// ����nPipeNum̫����

	status = WdfUsbTargetPipeAbortSynchronously(pipe, NULL, NULL);
	if(!NT_SUCCESS(status))
		KDBG( DPFLTR_ERROR_LEVEL, "AbortPipe Failed: 0x%0.8X", status);

	return status;
}

NTSTATUS UsbSetOrClearFeature(WDFDEVICE Device, WDFREQUEST Request)
{
	NTSTATUS status;
	WDFREQUEST Request_New = NULL;
	WDF_USB_CONTROL_SETUP_PACKET controlPacket;
	PSET_FEATURE_CONTROL pFeaturePacket;

	KDBG(DPFLTR_INFO_LEVEL, "[UsbSetOrClearFeature]");

	status = WdfRequestRetrieveInputBuffer(Request, sizeof(SET_FEATURE_CONTROL), &(void*)pFeaturePacket, NULL);
	if(!NT_SUCCESS(status))return status;

	WDF_USB_CONTROL_SETUP_PACKET_INIT_FEATURE(&controlPacket, 
		BmRequestToDevice,
		pFeaturePacket->FeatureSelector,
		pFeaturePacket->Index,
		pFeaturePacket->bSetOrClear
		);

	status = WdfRequestCreate(NULL, NULL, &Request_New);
	if(!NT_SUCCESS(status)){
		KDBG( DPFLTR_ERROR_LEVEL, "WdfRequestCreate Failed: 0x%0.8X", status);
		return status;
	}

	WdfUsbTargetDeviceFormatRequestForControlTransfer(
		GetDeviceContext(Device)->UsbDevice,
		Request_New, 
		&controlPacket, 
		NULL, NULL);

	if(FALSE == WdfRequestSend(Request_New, WdfDeviceGetIoTarget(Device), NULL))
		status = WdfRequestGetStatus(Request_New);
	WdfObjectDelete(Request_New);

	return status;
}

#if (DBG)
__inline KDBG(int nLevel, char* msg, ...)
{
	static char csmsg[1024];

	va_list argp;
	va_start(argp, msg);
	vsprintf(csmsg, msg, argp);
	va_end(argp);

	KdPrintEx((DPFLTR_DEFAULT_ID, nLevel, "CY001:"));// ��ʽ��ͷ
	KdPrintEx((DPFLTR_DEFAULT_ID, nLevel, csmsg));	 // Log body
	KdPrintEx((DPFLTR_DEFAULT_ID, nLevel, "\n"));	 // ����
}
#else
__inline KDBG(int nLevel, char* msg, ...)
{
}
#endif