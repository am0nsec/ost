/// @file    main.c
/// @author  Paul L. (@am0nsec)
/// @version 1.0
/// @link    https://github.com/am0nsec/ost
/// @brief   Windows specific code for the OpenSecurityTraining2 Architecture 2001 class
///          https://x.ost.fyi/courses/course-v1:OpenSecurityTraining+Arch2001_x86-64_OS_Internals+2021_V1/about
/// 
#include <ntddk.h>
#include "kseg.h"

_IRQL_requires_max_(PASSIVE_LEVEL)
EXTERN_C NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT  DriverObject,
	_In_ PUNICODE_STRING RegistryPath
) {
	UNREFERENCED_PARAMETER(RegistryPath);
	KdPrint(("[K_SEG] Begin driver initialisation.\n"));

	// 1. Specify the unload routine.
	DriverObject->DriverUnload = KsegUnload;

	// 2. Provide the major function dispatch routines
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = KsegDeviceIoControl;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = KsegCreate;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = KsegClose;

	// 3. Create device object
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(KSEG_DEVICE_PATH);
	PDEVICE_OBJECT DeviceObject = NULL;

	NTSTATUS Status = IoCreateDevice(
		DriverObject,
		0x00,
		&DeviceName,
		FILE_DEVICE_UNKNOWN,
		0x00,
		FALSE,
		&DeviceObject
	);
	if (!NT_SUCCESS(Status)) {
		KdPrint(("[K_SEG] Unable to create device object: (0x%08X)\n", Status));
		IoDeleteDevice(DeviceObject);
		return Status;
	}

	// 4. Create a symbolic link for user mode access
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(KSEG_DEVICE_PATH_USERMODE);
	Status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);
	if (!NT_SUCCESS(Status)) {
		KdPrint(("[K_SEG] Unable to create symbolic link to device object: (0x%08X)\n", Status));
		IoDeleteDevice(DeviceObject);
		return Status;
	}

	// Log success and return.
	KdPrint(("[K_SEG] Driver initialisation successful.\n"));
	return STATUS_SUCCESS;
}