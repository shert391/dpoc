#pragma once

NTSTATUS ioClose (PDEVICE_OBJECT deviceObject, PIRP pIrp);
NTSTATUS ioCreate (PDEVICE_OBJECT deviceObject, PIRP pIrp);
NTSTATUS ioDeviceControl (PDEVICE_OBJECT deviceObject, PIRP pIrp);