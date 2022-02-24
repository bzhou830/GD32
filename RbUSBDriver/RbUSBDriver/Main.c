#include "public.h"

#pragma alloc_text(INIT, DriverEntry)

NTSTATUS DriverEntry(IN PDRIVER_OBJECT  DriverObject, IN PUNICODE_STRING  RegistryPath)
{
  WDF_DRIVER_CONFIG config;
  NTSTATUS status = STATUS_SUCCESS;

  // 编译变量__DATE__, __TIME__在Checked环境中有效
  // 表示编译时候的时间戳
  KDBG(DPFLTR_INFO_LEVEL, "[***RbUSBDriver.sys DriverEntry ***]");
  KDBG(DPFLTR_INFO_LEVEL, "Compiled time: %s (%s)", __DATE__, __TIME__);

  // 创建WDF框架驱动对象。这个对象我们永远用不到，也不被暴露。但他确实存在着。
  // 我们可以把它看成一只隐藏着的手吧。
  // DriverEntry里面的这种初始化工作，和所有的小端口驱动一模一样，简直别无二致。
  // 读者可以据此参看诸如Scasi、文件过滤、NDIS、StreamPort等类型的小端口驱动，是如何运作的。
  WDF_DRIVER_CONFIG_INIT(&config, PnpAdd);

  status = WdfDriverCreate(DriverObject, // WDF驱动对象
                      RegistryPath,
                      WDF_NO_OBJECT_ATTRIBUTES,
                      &config, // 配置参数
                      WDF_NO_HANDLE);

  //gDriverObj = DriverObject; // 如果需要，可以把驱动对象保存在全局变量中

  return status;
}
