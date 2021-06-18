/// @file    kmsr.h
/// @author  Paul L. (@am0nsec)
/// @version 1.0
/// @link    https://github.com/am0nsec/ost
/// @brief   Windows specific code for the OpenSecurityTraining2 Architecture 2001 class
///          https://x.ost.fyi/courses/course-v1:OpenSecurityTraining+Arch2001_x86-64_OS_Internals+2021_V1/about
///
#ifndef __KMSR_H_GUARD__
#define __KMSR_H_GUARD__
#include <wdm.h>
#include <ntdef.h>

/// General information about the driver
#define KMSR_DEVICE_TYPE 0x8000
#define KMSR_DEVICE_NAME L"KMsr"
#define KMSR_DEVICE_PATH L"\\Device\\KMsr"
#define KMSR_DEVICE_PATH_USERMODE L"\\??\\KMsr"

/// List of IOCTL exposed by this driver
#define IOCTL_KMSR_READ  CTL_CODE(KMSR_DEVICE_TYPE, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_KMSR_WRITE CTL_CODE(KMSR_DEVICE_TYPE, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _WRMSR_IN {
	UINT32 EAX;
	UINT32 EDX;
} WRMSR_IN, * PWRMSR_IN;

typedef UINT32 RDMSR_IN;
typedef PUINT32 PRDMSR_IN;

typedef struct _RDMSR_OUT {
	UINT32 EAX;
	UINT32 EDX;
} RDMSR_OUT, * PRDMSR_OUT;

_IRQL_requires_max_(PASSIVE_LEVEL)
EXTERN_C NTSTATUS KmsrClose(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP           Irp
);

_IRQL_requires_max_(PASSIVE_LEVEL)
EXTERN_C NTSTATUS KmsrCreate(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP           Irp
);

_IRQL_requires_max_(PASSIVE_LEVEL)
EXTERN_C NTSTATUS KmsrDeviceIoControl(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP           Irp
);

EXTERN_C VOID _rdmsr(
	_In_ PRDMSR_IN  pDataIn,
	_In_ PRDMSR_OUT pDataOut
);

EXTERN_C VOID _wrmsr(
	_In_ PWRMSR_IN  pDataIn
);

#endif // !__KMSR_H_GUARD__

