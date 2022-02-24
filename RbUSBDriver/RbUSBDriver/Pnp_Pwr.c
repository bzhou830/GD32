#include "public.h"
#pragma code_seg("PAGE")

//��������Power�ص�����WDM�е�PnpSetPower���ơ�
NTSTATUS PwrD0Entry(IN WDFDEVICE  Device, IN WDF_POWER_DEVICE_STATE  PreviousState)
{
	PDEVICE_CONTEXT pContext = GetDeviceContext(Device);

	KDBG(DPFLTR_INFO_LEVEL, "[PwrD0Entry] from %s", PowerName(PreviousState));

	if(PreviousState == PowerDeviceD2)
	{
		SetDigitron(Device, pContext->byPre7Seg);
		SetLEDs(Device, pContext->byPreLEDs);
	}

	CompleteSyncRequest(Device, ENTERD0, PreviousState);

	return STATUS_SUCCESS;
}

// �뿪D0״̬
NTSTATUS PwrD0Exit(IN WDFDEVICE  Device, IN WDF_POWER_DEVICE_STATE  TargetState)
{
	PDEVICE_CONTEXT pContext = GetDeviceContext(Device);

	KDBG(DPFLTR_INFO_LEVEL, "[PwrD0Exit] to %s", PowerName(TargetState));

	if(TargetState == PowerDeviceD2)
	{		
		GetDigitron(Device, &pContext->byPre7Seg);
		GetLEDs(Device, &pContext->byPreLEDs);
	}

	CompleteSyncRequest(Device, EXITD0, TargetState);

	// ֹͣ�ж϶�����
	InterruptReadStop(Device);
	ClearSyncQueue(Device);
	return STATUS_SUCCESS;
}

VOID PnpSurpriseRemove(IN WDFDEVICE  Device)
{
	CompleteSyncRequest(Device, SURPRISE_REMOVE, 0);
}

// �����豸�����ĵ�Դ������
NTSTATUS InitPowerManagement(IN WDFDEVICE  Device)
{
	NTSTATUS status = STATUS_SUCCESS;
	WDF_USB_DEVICE_INFORMATION usbInfo;
	PDEVICE_CONTEXT pContext = GetDeviceContext(Device);

	KDBG(DPFLTR_INFO_LEVEL, "[InitPowerManagement]");

	// ��ȡ�豸��Ϣ
	WDF_USB_DEVICE_INFORMATION_INIT(&usbInfo);
	WdfUsbTargetDeviceRetrieveInformation(pContext->UsbDevice, &usbInfo);

	// USB�豸��Ϣ��������ʽ��������Traits�С�
	KDBG( DPFLTR_INFO_LEVEL, "Device self powered: %s", 
		usbInfo.Traits & WDF_USB_DEVICE_TRAIT_SELF_POWERED ? "TRUE" : "FALSE");
	KDBG( DPFLTR_INFO_LEVEL, "Device remote wake capable: %s",
		usbInfo.Traits & WDF_USB_DEVICE_TRAIT_REMOTE_WAKE_CAPABLE ? "TRUE" : "FALSE");
	KDBG( DPFLTR_INFO_LEVEL, "Device high speed: %s",
		usbInfo.Traits & WDF_USB_DEVICE_TRAIT_AT_HIGH_SPEED ? "TRUE" : "FALSE");

	// �����豸�����ߺ�Զ�̻��ѹ���
	if(usbInfo.Traits & WDF_USB_DEVICE_TRAIT_REMOTE_WAKE_CAPABLE)
	{
		WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS idleSettings;
		WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS wakeSettings;

		// �����豸Ϊ��ʱ���ߡ���ʱ����10S���Զ���������״̬��
		WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS_INIT(&idleSettings, IdleUsbSelectiveSuspend);
		idleSettings.IdleTimeout = 10000;
		status = WdfDeviceAssignS0IdleSettings(Device, &idleSettings);
		if(!NT_SUCCESS(status))
		{
			KDBG( DPFLTR_ERROR_LEVEL, "WdfDeviceAssignS0IdleSettings failed with status 0x%0.8x!!!", status);
			return status;
		}

		// ����Ϊ��Զ�̻��ѡ������豸�����������Ѿ���PCϵͳ�������ߺ��豸���Խ�ϵͳ���ѣ��������档
		WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS_INIT(&wakeSettings);
		status = WdfDeviceAssignSxWakeSettings(Device, &wakeSettings);
		if(!NT_SUCCESS(status))
		{
			KDBG( DPFLTR_ERROR_LEVEL, "WdfDeviceAssignSxWakeSettings failed with status 0x%0.8x!!!", status);
			return status;
		}
	}

	return status;
}

char* PowerName(WDF_POWER_DEVICE_STATE PowerState)
{
	char *name;

	switch(PowerState)
	{
	case WdfPowerDeviceInvalid :	
		name = "PowerDeviceUnspecified"; 
		break;
	case WdfPowerDeviceD0:			
		name = "WdfPowerDeviceD0"; 
		break;
	case WdfPowerDeviceD1:			
		name = "WdfPowerDeviceD1"; 
		break;
	case WdfPowerDeviceD2:		
		name = "WdfPowerDeviceD2"; 
		break;
	case WdfPowerDeviceD3:		
		name = "WdfPowerDeviceD3"; 
		break;
	case WdfPowerDeviceD3Final:	
		name = "WdfPowerDeviceD3Final";
		break;
	case WdfPowerDevicePrepareForHibernation:									
		name = "WdfPowerDevicePrepareForHibernation"; 
		break;
	case WdfPowerDeviceMaximum:		
		name = "WdfPowerDeviceMaximum";
		break;
	default:					
		name = "Unknown Power State";
		break;
	}

	return name;
}