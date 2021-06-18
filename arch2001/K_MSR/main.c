/// @file    main.c
/// @author  Paul L. (@am0nsec)
/// @version 1.0
/// @link    https://github.com/am0nsec/ost
/// @brief   Windows specific code for the OpenSecurityTraining2 Architecture 2001 class
///          https://x.ost.fyi/courses/course-v1:OpenSecurityTraining+Arch2001_x86-64_OS_Internals+2021_V1/about
/// 
#include <ntddk.h>
#include "kmsr.h"

/// <summary>
/// IRQL 0 - Executed when the driver is unloaded.
/// </summary>
/// <param name="DriverObject">Pointer to the driver object.</param>
_IRQL_requires_max_(PASSIVE_LEVEL)
EXTERN_C VOID DriverUnload(
	_In_ PDRIVER_OBJECT DriverObject
) {
	UNREFERENCED_PARAMETER(DriverObject);
	KdPrint(("[K_MSR] Unloading driver.\n"));

	// 1. Delete symbolic link
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(KMSR_DEVICE_PATH_USERMODE);
	NTSTATUS Status = IoDeleteSymbolicLink(&SymbolicLinkName);
	if (!NT_SUCCESS(Status))
		KdPrint(("[K_MSR] Unable to delete symbolic link to device object: (0x%08X)\n", Status));

	// 2. Delete the device object
	IoDeleteDevice(DriverObject->DeviceObject);

	// 3. Log success and exit.
	KdPrint(("[K_MSR] Driver successfully unloaded.\n"));
	return;
}

/// <summary>
/// IRQL 0 - Driver entry point.
/// </summary>
/// <param name="DriverObject">Pointer to the driver object.</param>
/// <param name="RegistryPath">Pointer to the path in the Windows Registry of the object. Unused.</param>
/// <returns>Whether an error was encountered while initialising the driver.</returns>
_IRQL_requires_max_(PASSIVE_LEVEL)
EXTERN_C NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT  DriverObject,
	_In_ PUNICODE_STRING RegistryPath
) {
	UNREFERENCED_PARAMETER(RegistryPath);
	KdPrint(("[K_MSR] Begin driver initialisation.\n"));

	// 1. Specify the unload routine.
	DriverObject->DriverUnload = DriverUnload;

	// 2. Provide the major function dispatch routines
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = KmsrDeviceIoControl;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = KmsrCreate;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = KmsrClose;

	// 3. Create device object
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(KMSR_DEVICE_PATH);
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
		KdPrint(("[K_MSR] Unable to create device object: (0x%08X)\n", Status));
		IoDeleteDevice(DeviceObject);
		return Status;
	}

	// 4. Create a symbolic link for user mode access
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(KMSR_DEVICE_PATH_USERMODE);
	Status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);
	if (!NT_SUCCESS(Status)) {
		KdPrint(("[K_MSR] Unable to create symbolic link to device object: (0x%08X)\n", Status));
		IoDeleteDevice(DeviceObject);
		return Status;
	}

	// Log success and return.
	KdPrint(("[K_MSR] Driver initialisation successful.\n"));
	return STATUS_SUCCESS;
}