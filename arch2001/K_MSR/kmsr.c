/// @file    kmsr.c
/// @author  Paul L. (@am0nsec)
/// @version 1.0
/// @link    https://github.com/am0nsec/ost
/// @brief   Windows specific code for the OpenSecurityTraining2 Architecture 2001 class
///          https://x.ost.fyi/courses/course-v1:OpenSecurityTraining+Arch2001_x86-64_OS_Internals+2021_V1/about
/// 
#include <ntddk.h>
#include "kmsr.h"

_Use_decl_annotations_
EXTERN_C NTSTATUS KmsrCreate(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP           Irp
) {
	UNREFERENCED_PARAMETER(DeviceObject);
	KdPrint(("[K_MSR] KmsrCreate.\n"));

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0x00;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

_Use_decl_annotations_
EXTERN_C NTSTATUS KmsrClose(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP           Irp
) {
	UNREFERENCED_PARAMETER(DeviceObject);
	KdPrint(("[K_MSR] KmsrClose.\n"));

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0x00;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

_Use_decl_annotations_
EXTERN_C NTSTATUS KmsrDeviceIoControl(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP           Irp
) {
	UNREFERENCED_PARAMETER(DeviceObject);

	// 1. Log execution of the routine.
	KdPrint(("[K_MSR] KmsrDeviceIoControl.\n"));

	// 2. Get the stack and initialise return variable
	PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS Status = STATUS_SUCCESS;

	// 3. Check if valid IRQL has been provide
	switch (Stack->Parameters.DeviceIoControl.IoControlCode) {
		// 3.1 Will handle the IOCTL_KMSR_READ IOCTL
		case IOCTL_KMSR_READ: {
			// 3.1.1 Ensure that the input and output buffers are large enough
			KdPrint(("[K_MSR] Input buffer: %d\n", Stack->Parameters.DeviceIoControl.InputBufferLength));
			if (Stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(RDMSR_IN)
				|| Stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(RDMSR_OUT)) {
				Status = STATUS_BUFFER_TOO_SMALL;
				break;
			}

			// 3.1.2 Check the data provided
			PRDMSR_IN pTargetMsr = (PRDMSR_IN)Irp->AssociatedIrp.SystemBuffer;
			if (pTargetMsr == NULL) {
				Status = STATUS_INVALID_DEVICE_REQUEST;
				break;
			}

			// 3.1.3 Call the assembly routine to get the data
			KdPrint(("[K_MSR] _rdmsr.\n"));
			_rdmsr(pTargetMsr, (PRDMSR_OUT)Irp->AssociatedIrp.SystemBuffer);

			// 3.1.4 Update size of data to return and exit
			Irp->IoStatus.Information = sizeof(RDMSR_OUT);
			break;
		}

		// 3.2 Will handle the IOCTL_KMSR_WRITE IOCTL
		case IOCTL_KMSR_WRITE: {
			// 3.2.1 Ensure that the input is large enough
			if (Stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(RDMSR_IN)) {
				Status = STATUS_BUFFER_TOO_SMALL;
				break;
			}

			// 3.2.2 Call the internal routine to modify an MSR value.
			KdPrint(("[K_MSR] _wrmsr.\n"));
			_wrmsr((PWRMSR_IN)Stack->Parameters.DeviceIoControl.Type3InputBuffer);
			break;
		}
		
		// 3.3 An invalid or at least an unknown IOCTL has been provided 
		default: {
			Status = STATUS_INVALID_DEVICE_REQUEST;
			break;
		}
	}

	// 4. Complete the request
	Irp->IoStatus.Status = Status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return Status;
}
