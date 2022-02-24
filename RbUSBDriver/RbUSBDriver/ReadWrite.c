#include "public.h"

#pragma alloc_text(PAGE, InterruptRead)
#pragma alloc_text(PAGE, BulkRead)
#pragma alloc_text(PAGE, BulkWrite)
#pragma alloc_text(PAGE, BulkReadComplete)
#pragma alloc_text(PAGE, BulkWriteComplete)

// �ж�Pipe�ص�����������һ���豸�������ж���Ϣ���������ܹ���ȡ����
VOID InterruptRead(WDFUSBPIPE Pipe, WDFMEMORY Buffer, size_t NumBytesTransferred, WDFCONTEXT Context)
{
    UNREFERENCED_PARAMETER(NumBytesTransferred);
	NTSTATUS status;
	size_t size = 0;
	PDEVICE_CONTEXT pContext = (PDEVICE_CONTEXT)Context;
	WDFREQUEST Request = NULL;
	CHAR *pchBuf = NULL;

	KDBG(DPFLTR_INFO_LEVEL, "[InterruptRead]");
	UNREFERENCED_PARAMETER(Pipe);

	// Read���ݻ�������ע�⵽���������������ǹܵ��������ȵı�����
	// ��������ֻ�û������ĵ�һ����Ч�ֽڡ�
	pchBuf = (CHAR*)WdfMemoryGetBuffer(Buffer, &size);
	if(pchBuf == NULL || size == 0)
		return;

	// ��һ���ֽ�Ϊȷ���ֽڣ�һ����0xD4
	//if(pchBuf[0] != 0xD4)return;

	// �Ӷ�������ȡһ��δ�������
	status = WdfIoQueueRetrieveNextRequest(pContext->InterruptManualQueue, &Request);

	if(NT_SUCCESS(status))
	{
		CHAR* pOutputBuffer = NULL;
		status = WdfRequestRetrieveOutputBuffer(Request, 1, &pOutputBuffer, NULL);

		if(NT_SUCCESS(status))
		{
			// �ѽ�����ظ�Ӧ�ó���
			pOutputBuffer[0] = pchBuf[1];
			WdfRequestCompleteWithInformation(Request, status, 1);
		}
		else
		{
			// ���ش���
			WdfRequestComplete(Request, status);
		}

		KDBG(DPFLTR_INFO_LEVEL, "Get and finish an interrupt read request.");
	}else{
		// ���пգ����������豸��ȡ�����ݡ�
		KDBG(DPFLTR_INFO_LEVEL, "Manual interrupt queue is empty!!!");
	}
}

// Bulk�ܵ�д����
VOID BulkWrite(IN WDFQUEUE  Queue, IN WDFREQUEST  Request, IN size_t  Length)
{
	NTSTATUS status = STATUS_SUCCESS;
	WDFMEMORY hMem = NULL;
	WDFDEVICE hDevice = NULL;
	WDFUSBPIPE BulkOutputPipe = NULL;
	UCHAR* lpBuf;

	UNREFERENCED_PARAMETER(Length);

	KDBG(DPFLTR_INFO_LEVEL, "[BulkWrite] size: %d", Length);

	// ��ȡ�ܵ����
	hDevice = WdfIoQueueGetDevice(Queue);
	BulkOutputPipe = GetBulkPipe(FALSE, hDevice);

	if(NULL == BulkOutputPipe)
	{
		KDBG(DPFLTR_ERROR_LEVEL, "BulkOutputPipe = NULL");
		WdfRequestComplete(Request, STATUS_UNSUCCESSFUL);
		return;
	}

	status = WdfRequestRetrieveInputMemory(Request, &hMem);
	if(!NT_SUCCESS(status))
	{
		KDBG(DPFLTR_ERROR_LEVEL, "WdfRequestRetrieveInputMemory failed(status = 0x%0.8x)!!!", status);
		WdfRequestComplete(Request, status);
		return;
	}

	// ��ӡ��offsetֵ��
	// ��д�����ǰ�����ֽ��д���write offset��ֵ
	lpBuf = (UCHAR*)WdfMemoryGetBuffer(hMem, 0);
	KDBG(DPFLTR_TRACE_LEVEL, "write offset: %hd", *(WORD*)lpBuf);

	// �ѵ�ǰ��Request������������ã����͸�USB���ߡ�
	// ��ʽ��Request��������Pipe�������ɺ����ȡ�
	status = WdfUsbTargetPipeFormatRequestForWrite(BulkOutputPipe, Request, hMem, NULL);
	if(!NT_SUCCESS(status))
	{
		KDBG(DPFLTR_ERROR_LEVEL, "WdfUsbTargetPipeFormatRequestForWrite(status 0x%0.8x)!!!", status);
		WdfRequestComplete(Request, status);
		return;
	}

	WdfRequestSetCompletionRoutine(Request, BulkWriteComplete, BulkOutputPipe); 
	if(FALSE == WdfRequestSend(Request, WdfUsbTargetPipeGetIoTarget(BulkOutputPipe), NULL))
	{
		status = WdfRequestGetStatus(Request);
		KDBG(DPFLTR_ERROR_LEVEL, "WdfRequestSend failed with status 0x%0.8x\n", status);		
		WdfRequestComplete(Request, status);
	}
}

// Bulk�ܵ�д��������ɺ���
VOID BulkWriteComplete(IN WDFREQUEST  Request, IN WDFIOTARGET  Target, 
						IN PWDF_REQUEST_COMPLETION_PARAMS  Params, 
						IN WDFCONTEXT  Context)
{
	PWDF_USB_REQUEST_COMPLETION_PARAMS usbCompletionParams;
	NTSTATUS ntStatus;
	ULONG_PTR ulLen;
	LONG* lpBuf;

	UNREFERENCED_PARAMETER(Context);
	UNREFERENCED_PARAMETER(Target);

	KDBG(DPFLTR_INFO_LEVEL, "[BulkWriteComplete]");

	usbCompletionParams = Params->Parameters.Usb.Completion;
	ntStatus = Params->IoStatus.Status;
	ulLen = usbCompletionParams->Parameters.PipeWrite.Length;
	lpBuf = WdfMemoryGetBuffer(usbCompletionParams->Parameters.PipeWrite.Buffer, NULL);

	if(NT_SUCCESS(ntStatus))
		KDBG(DPFLTR_INFO_LEVEL, "%d bytes written to USB device successfully.", ulLen);
	else
		KDBG(DPFLTR_INFO_LEVEL, "Failed to write: 0x%08x!!!", ntStatus);

	// �������
	// Ӧ�ó����յ�֪ͨ��
	WdfRequestCompleteWithInformation(Request, ntStatus, ulLen);
}

// Bulk�ܵ�������
VOID BulkRead(IN WDFQUEUE  Queue, IN WDFREQUEST  Request, IN size_t  Length)
{
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_CONTEXT pContext = NULL;
	WDFUSBPIPE BulkInputPipe;
	WDFMEMORY hMem;
	WDFDEVICE hDevice = WdfIoQueueGetDevice(Queue);

	KDBG(DPFLTR_INFO_LEVEL, "[BulkRead] size: %d", Length);

	// ȡ�ܵ�
	pContext = GetDeviceContext(hDevice);
	BulkInputPipe = GetBulkPipe(TRUE, hDevice);
	if(NULL == BulkInputPipe){
		WdfRequestComplete(Request, STATUS_UNSUCCESSFUL);
		return;
	}

	status = WdfRequestRetrieveOutputMemory(Request, &hMem);
	if(!NT_SUCCESS(status))
	{
		KDBG(DPFLTR_INFO_LEVEL, "WdfRequestRetrieveOutputMemory failed with status 0x%0.8x!!!", status);
		WdfRequestComplete(Request, status);
		return;
	}

	// ��дһ������Ȼ�Ƕ�Request�����������
	// �����������⣬Ҳ�����½�һ��Request�����½��ķ����ڱ����������ط��õý϶ࡣ
	status = WdfUsbTargetPipeFormatRequestForRead(BulkInputPipe, Request, hMem, NULL);
	if(!NT_SUCCESS(status))
	{
		KDBG(DPFLTR_INFO_LEVEL, "WdfUsbTargetPipeFormatRequestForRead failed with status 0x%08x\n", status);
		WdfRequestComplete(Request, status);
		return;
	}

	WdfRequestSetCompletionRoutine(Request, BulkReadComplete, BulkInputPipe);
	if(FALSE == WdfRequestSend(Request, WdfUsbTargetPipeGetIoTarget(pContext->UsbBulkInPipe), NULL))
	{
		status = WdfRequestGetStatus(Request);
		KDBG(DPFLTR_INFO_LEVEL, "WdfRequestSend failed with status 0x%08x\n", status);
		WdfRequestComplete(Request, status);
	}
}

// Bulk�ܵ�����������ɺ���
VOID BulkReadComplete(IN WDFREQUEST  Request, IN WDFIOTARGET  Target,
					   IN PWDF_REQUEST_COMPLETION_PARAMS  Params, 
					   IN WDFCONTEXT  Context)
{
	PWDF_USB_REQUEST_COMPLETION_PARAMS usbCompletionParams;
	NTSTATUS ntStatus;
	ULONG_PTR ulLen;
	LONG* lpBuf;

	UNREFERENCED_PARAMETER(Context);
	UNREFERENCED_PARAMETER(Target);

	KDBG(DPFLTR_INFO_LEVEL, "[BulkReadComplete]");

	usbCompletionParams = Params->Parameters.Usb.Completion;
	ntStatus = Params->IoStatus.Status;
	ulLen = usbCompletionParams->Parameters.PipeRead.Length;
	lpBuf = WdfMemoryGetBuffer(usbCompletionParams->Parameters.PipeRead.Buffer, NULL);

	if(NT_SUCCESS(ntStatus))
		KDBG(DPFLTR_INFO_LEVEL, "%d bytes readen from USB device successfully.", ulLen);
	else
		KDBG(DPFLTR_INFO_LEVEL, "Failed to read: 0x%08x!!!", ntStatus);

	// ��ɲ���
	// Ӧ�ó����յ�֪ͨ��
	WdfRequestCompleteWithInformation(Request, ntStatus, ulLen);
}
