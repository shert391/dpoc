#include "global.h"

void Unload (IN PDRIVER_OBJECT DriverObject) {
    IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS DriverEntry (IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING pRegistryPath) {
    PDEVICE_OBJECT pDeviceObject;
    UNICODE_STRING deviceName {0};
    RtlInitUnicodeString(&deviceName, L"\\Device\\mapper");
    IoCreateDevice(pDriverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);
    pDriverObject->DriverUnload = Unload;
    __debugbreak();
    void* pEpDrv = fExecuteMap(L"\\??\\C:\\drv\\testdrv.sys");
    __debugbreak();
    ((NTSTATUS(__fastcall*)(uintptr_t, uintptr_t))pEpDrv)(0, 0);
    return STATUS_SUCCESS;
}