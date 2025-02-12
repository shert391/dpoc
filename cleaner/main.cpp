#include "global.h"

void Unload(_In_ PDRIVER_OBJECT DriverObject) {
	UNREFERENCED_PARAMETER(DriverObject);
	DbgPrint("Unload\n");
}

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT pDriverObject, _In_ PUNICODE_STRING pRegistryPath)
{
	PDEVICE_OBJECT pDeviceObject;
	UNICODE_STRING deviceName{ 0 };
	RtlInitUnicodeString(&deviceName, L"\\Device\\mapper");

	IoCreateDevice(pDriverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);

	pDriverObject->DriverUnload = Unload;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = ioClose;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = ioCreate;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ioDeviceControl;

	cInit();
	return STATUS_SUCCESS;
}