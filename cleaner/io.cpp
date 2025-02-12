#include "global.h"

NTSTATUS ioDeviceControl(PDEVICE_OBJECT deviceObject, PIRP pIrp)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PIO_STACK_LOCATION pStack = IoGetCurrentIrpStackLocation(pIrp);

	switch (pStack->Parameters.DeviceIoControl.IoControlCode)
	{
	case CTL_INIT:
		cInit();
		status = STATUS_SUCCESS;
		break;
	case CTL_SHOW_DDB_CACHE:
		cShowDdbCacheTable();
		status = STATUS_SUCCESS;
		break;
	}

	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return status;
}

NTSTATUS ioCreate(PDEVICE_OBJECT deviceObject, PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS ioClose(PDEVICE_OBJECT deviceObject, PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}
