#include "global.h"

void Unload (IN PDRIVER_OBJECT DriverObject) {
	IoDeleteDevice(DriverObject->DeviceObject);
	dbg("Cleaner Unload\n");
}

NTSTATUS DriverEntry (IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING pRegistryPath) {
	PDEVICE_OBJECT pDeviceObject;
	UNICODE_STRING deviceName {0};
	RtlInitUnicodeString(&deviceName, L"\\Device\\cleaner");
	IoCreateDevice(pDriverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);
	pDriverObject->DriverUnload = Unload;
	cInit();
	return STATUS_SUCCESS;
}