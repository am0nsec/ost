/// @file    kseg.h
/// @author  Paul L. (@am0nsec)
/// @version 1.0
/// @link    https://github.com/am0nsec/ost
/// @brief   Windows specific code for the OpenSecurityTraining2 Architecture 2001 class
///          https://x.ost.fyi/courses/course-v1:OpenSecurityTraining+Arch2001_x86-64_OS_Internals+2021_V1/about
/// 
#ifndef __KSEG_H_GUARD__
#define __KSEG_H_GUARD__
#include <ntddk.h>

/// General information about the driver
#define KSEG_DEVICE_TYPE 0x8000
#define KSEG_DEVICE_NAME L"KSeg"
#define KSEG_DEVICE_PATH L"\\Device\\KSeg"
#define KSEG_DEVICE_PATH_USERMODE L"\\??\\KSeg"

/// List of IOCTL exposed by this driver
#define IOCTL_KSEG_QUERY CTL_CODE(KSEG_DEVICE_TYPE, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

/// <summary>
/// C data structure to store the visible part of a segment register.
/// </summary>
typedef union _Segment {
	struct {
		UINT16 RPL : 2;             // Request Privilege Level
		UINT16 TableIndicator : 1;  // GDT if TI = 0 otherwise LDT if TI = 1
		UINT16 Index : 13;          // Index within either the GDT or LDT
	} elem;
	UINT16 value;
} Segment, * PSegment;

/// <summary>
/// C data structure to store the information related to a segment descriptor from either an LDT or the GDT.
/// </summary>
typedef union _SegmentDescriptor {
	struct {
		UINT64 LimitLow : 16;         // Low 16 bits of the segment limit
		UINT64 BaseLow : 16;          // 1st and 2nd byte of the base address of the segment
		UINT64 BaseMidLow : 8;        // 3rd byte of the base address of the segment
		struct {
			UCHAR Accessed : 1;       // Accessed bit
			union {
				UCHAR Writable : 1;   // Writable bit. 0 means read only
				UCHAR Readbale : 1;   // Readable bit. 0 means execute-only
			} WR;
			union {
				UCHAR ExpandDown : 1; // Expand-Down bit.
				UCHAR Conforming : 1; // If 1 the segment is conforming
			} EC;
			UCHAR CodeData : 1;       // Code/Data byte. Code segment if 1
		} Type;
		UINT64 System : 1;            // System bit. 0 means OS data structure descriptor
		UINT64 DPL : 2;               // Descriptor privilege level
		UINT64 Present : 1;           // Present bit
		UINT64 LimitUpperNibble : 4;  // Upper nibble of the segment limit
		UINT64 AVL : 1;               // Available for use by system
		UINT64 Long : 1;              // Long bit
		union {
			UINT64 Big : 1;           //
			UINT64 Default : 1;       //
		} BD;
		UINT64 Granularity : 1;       // Granularity bit. If 0 size in byte otherwise in pages
		UINT64 BaseMidHigh : 8;       // 4th byte of the base address of the segment
	} Entry;
	UINT64 Value;
} SegmentDescriptor, *PSegmentDescriptor;

/// <summary>
/// Data returned by the IO query
/// </summary>
typedef struct _KSEG_OUT {
	Segment           Seg;
	SegmentDescriptor Descriptor;
} KSEG_OUT, *PKSEG_OUT;

/// <summary>
/// C data structure representing the returned value of RDMSR instruction.
/// </summary>
typedef struct _RDMSR_OUT {
	UINT32 EAX;
	UINT32 EDX;
} RDMSR_OUT, * PRDMSR_OUT;

/// <summary>
/// IRQL 0 - Executed when the driver is unloaded.
/// </summary>
/// <param name="DriverObject">Pointer to the driver object.</param>
_IRQL_requires_max_(PASSIVE_LEVEL)
EXTERN_C VOID KsegUnload(
	_In_ PDRIVER_OBJECT DriverObject
);

_IRQL_requires_max_(PASSIVE_LEVEL)
EXTERN_C NTSTATUS KsegClose(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP           Irp
);

_IRQL_requires_max_(PASSIVE_LEVEL)
EXTERN_C NTSTATUS KsegCreate(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP           Irp
);

_IRQL_requires_max_(PASSIVE_LEVEL)
EXTERN_C NTSTATUS KsegDeviceIoControl(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP           Irp
);

EXTERN_C VOID STDMETHODCALLTYPE _read_cs(PSegment cs);
EXTERN_C VOID STDMETHODCALLTYPE _read_ss(PSegment ss);
EXTERN_C VOID STDMETHODCALLTYPE _read_ds(PSegment ds);
EXTERN_C VOID STDMETHODCALLTYPE _read_es(PSegment es);
EXTERN_C VOID STDMETHODCALLTYPE _read_fs(PSegment fs);
EXTERN_C VOID STDMETHODCALLTYPE _read_gs(PSegment gs);

#endif // !__KSEG_H_GUARD__
