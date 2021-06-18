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

#pragma warning(disable: 4201)


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
	union {
		struct {
			/// <summary>
			/// 1st and 2nd byte of the segment limit.
			/// </summary>
			UINT16 LimitLow;
			/// <summary>
			/// 1st and 2nd byte of the base address of the segment.
			/// </summary>
			UINT16 BaseLow;
			union {
				struct {
					UINT8 BaseMiddle;
					UINT8 Flags1;
					UINT8 Flags2;
					UINT8 BaseHigh;
				} Bytes;
				struct {
					struct {
						struct {
							/// <summary>
							/// 3rd byte of the base address
							/// </summary>
							ULONG BaseMiddle : 8;
							/// <summary>
							/// Accessed bit. Switched to 1 when the segment is accessed.
							/// </summary>
							ULONG Accessed : 1;
							/// For code segment: Readable bit. 0 means execute-only while 1 means that the segment contains code and data.
							/// For data segment: Writable bit. 0 means read-only segment while 1 means RW segment.
							ULONG WritableReadable : 1;
							/// For code segment: Conforming bit. If 1 the code segment is conforming.
							/// For data segment: Expand Down bit. If 1 the segment is an expand-down stack.
							ULONG ExpandDownConforming : 1;
							/// <summary>
							/// Data/Code byte. If 0 this is a data segment. Otherwise code segment.
							/// </summary>
							ULONG CodeData : 1;
							/// <summary>
							/// System bit. If 0 this is for an OS data structure. Must be 1 for code segment.
							/// </summary>
							ULONG System : 1;
							/// <summary>
							/// Descriptor privilege level
							/// </summary>
							ULONG Dpl : 2;
							/// <summary>
							/// Present bit. 
							/// </summary>
							ULONG Present : 1;
							/// <summary>
							/// Upper Nibble of the limit
							/// </summary>
							ULONG LimitHigh : 4;
							/// <summary>
							/// Available for use by OS kernel.
							/// </summary>
							ULONG AVL : 1;
							/// <summary>
							/// Long Mode bit. 32bits if set to 0 otherwise 64bits.
							/// </summary>
							ULONG LongMode : 1;
							/// <summary>
							/// For code segment: Default bit. 0 means 16bits code segment otherwise 32bits code segment.
							/// For data segment: Big bit. 0 means 
							/// </summary>
							ULONG DefaultBig : 1;
							/// <summary>
							/// Granularity bit. If 0 the segment size is in byte, otherwise size in pages.
							/// </summary>
							ULONG Granularity : 1;
							/// <summary>
							/// 4th byte of the base address.
							/// </summary>
							ULONG BaseHigh : 8;
						};
					} Bits;
					/// <summary>
					/// 4th byte of the base address.
					/// </summary>
					ULONG BaseUpper;
					ULONG MustBeZero;
				};
			};
		};
		struct {
			UINT64 DataLow;
			UINT64 DataHigh;
		};
	};
} SegmentDescriptor, * PSegmentDescriptor;

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

#pragma pack(push, 1)
/// <summary>
/// C data structure representing the returned value of SGDT instruction.
/// </summary>
typedef struct _SGDT_OUT {
	UINT16             Limit;
	PSegmentDescriptor Address;
} SGDT_OUT, * PSGDT_OUT;
#pragma pack(pop)

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

EXTERN_C VOID STDMETHODCALLTYPE _read_gdtr(PSGDT_OUT gdtr);
EXTERN_C VOID STDMETHODCALLTYPE _read_ldtr(PSegment seg);
EXTERN_C VOID STDMETHODCALLTYPE _get_pkpcr(PKPCR pKpcr);

#endif // !__KSEG_H_GUARD__
