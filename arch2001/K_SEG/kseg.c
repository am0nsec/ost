/// @file    kseg.c
/// @author  Paul L. (@am0nsec)
/// @version 1.0
/// @link    https://github.com/am0nsec/ost
/// @brief   Windows specific code for the OpenSecurityTraining2 Architecture 2001 class
///          https://x.ost.fyi/courses/course-v1:OpenSecurityTraining+Arch2001_x86-64_OS_Internals+2021_V1/about
/// 
#include <ntddk.h>
#include "kseg.h"

/// <summary>
/// Jump table for quick access to the read functions.
/// </summary>
VOID(STDMETHODCALLTYPE* KsegReadFunctions[6])(
	_In_ PSegment Seg
) = {
		_read_cs,
		_read_ss,
		_read_ds,
		_read_es,
		_read_fs,
		_read_gs
};

_Use_decl_annotations_
EXTERN_C VOID KsegUnload(
	_In_ PDRIVER_OBJECT DriverObject
) {
	UNREFERENCED_PARAMETER(DriverObject);
	KdPrint(("[K_SEG] Unloading driver.\n"));

	// 1. Delete symbolic link
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(KSEG_DEVICE_PATH_USERMODE);
	NTSTATUS Status = IoDeleteSymbolicLink(&SymbolicLinkName);
	if (!NT_SUCCESS(Status))
		KdPrint(("[K_SEG] Unable to delete symbolic link to device object: (0x%08X)\n", Status));

	// 2. Delete the device object
	IoDeleteDevice(DriverObject->DeviceObject);

	// 3. Log success and exit.
	KdPrint(("[K_SEG] Driver successfully unloaded.\n"));
	return;
}

_Use_decl_annotations_
EXTERN_C NTSTATUS KsegCreate(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP           Irp
) {
	UNREFERENCED_PARAMETER(DeviceObject);
	KdPrint(("[K_SEG] KmsrCreate.\n"));

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0x00;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

_Use_decl_annotations_
EXTERN_C NTSTATUS KsegClose(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP           Irp
) {
	UNREFERENCED_PARAMETER(DeviceObject);
	KdPrint(("[K_SEG] KmsrClose.\n"));

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0x00;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

_Use_decl_annotations_
EXTERN_C NTSTATUS KsegDeviceIoControl(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP           Irp
) {
	UNREFERENCED_PARAMETER(DeviceObject);
	KdPrint(("[K_SEG] KsegDeviceIoControl.\n"));

	// 1. Get current stack and initialise the return value
	PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS Status = STATUS_SUCCESS;

	// 2. Check that the valid IRQL has been provided
	switch (Stack->Parameters.DeviceIoControl.IoControlCode) {
		// 2.1 If the only IOCTL is provided.
		case IOCTL_KSEG_QUERY: {
			// 2.1.2 Check the size of the buffers
			if (Stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(UINT16)
				|| Stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(KSEG_OUT)) {
				Status = STATUS_BUFFER_TOO_SMALL;
				break;
			}

			// 2.1.3 Get the segment register to get.
			PUINT16 pRegister = (PUINT16)Irp->AssociatedIrp.SystemBuffer;
			if (*pRegister > 6) {
				KdPrint(("[K_SEG] Invalid segment register type provided: %d", *pRegister));
				Status = STATUS_INVALID_DEVICE_REQUEST;
				break;
			}

			// 2.1.4 Get the segment value
			Segment Seg = { 0x00 };
			KsegReadFunctions[*pRegister](&Seg);
			KdPrint(("[K_SEG] Segment register type %d: 0x02x", *pRegister, Seg.value));

			// 2.1.5 Get the location of the LDT or GDT
			//RDMSR_OUT MsrData = { 0x00 };
			//PSegmentDescriptor pDescriptor = NULL;
			//if (Seg.elem.TableIndicator == 0x00) {
			//	// GDT
			//}
			//else {
			//	// LDT
			//}

			// 2.1.6 Return data back to the caller
			PKSEG_OUT DataOut = (PKSEG_OUT)Irp->AssociatedIrp.SystemBuffer;
			DataOut->Seg = Seg;
			Irp->IoStatus.Information = sizeof(KSEG_OUT);
			break;
		}

		// 2.2 If any other IOCTL is provided.
		default: {
			KdPrint(("[K_SEG] Invalid IRQL has been provided.\n"));
			Status = STATUS_INVALID_DEVICE_REQUEST;
			break;
		}
	}

	Irp->IoStatus.Status = Status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}