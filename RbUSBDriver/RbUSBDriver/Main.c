#include "public.h"

#pragma alloc_text(INIT, DriverEntry)

NTSTATUS DriverEntry(IN PDRIVER_OBJECT  DriverObject, IN PUNICODE_STRING  RegistryPath)
{
  WDF_DRIVER_CONFIG config;
  NTSTATUS status = STATUS_SUCCESS;

  // �������__DATE__, __TIME__��Checked��������Ч
  // ��ʾ����ʱ���ʱ���
  KDBG(DPFLTR_INFO_LEVEL, "[***RbUSBDriver.sys DriverEntry ***]");
  KDBG(DPFLTR_INFO_LEVEL, "Compiled time: %s (%s)", __DATE__, __TIME__);

  // ����WDF������������������������Զ�ò�����Ҳ������¶������ȷʵ�����š�
  // ���ǿ��԰�������һֻ�����ŵ��ְɡ�
  // DriverEntry��������ֳ�ʼ�������������е�С�˿�����һģһ������ֱ���޶��¡�
  // ���߿��Ծݴ˲ο�����Scasi���ļ����ˡ�NDIS��StreamPort�����͵�С�˿�����������������ġ�
  WDF_DRIVER_CONFIG_INIT(&config, PnpAdd);

  status = WdfDriverCreate(DriverObject, // WDF��������
                      RegistryPath,
                      WDF_NO_OBJECT_ATTRIBUTES,
                      &config, // ���ò���
                      WDF_NO_HANDLE);

  //gDriverObj = DriverObject; // �����Ҫ�����԰��������󱣴���ȫ�ֱ�����

  return status;
}
