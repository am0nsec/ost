/// @file    main.c
/// @author  Paul L. (@am0nsec)
/// @version 1.0
/// @link    https://github.com/am0nsec/ost
/// @brief   Windows specific code for the OpenSecurityTraining2 Architecture 2001 class
///          https://x.ost.fyi/courses/course-v1:OpenSecurityTraining+Arch2001_x86-64_OS_Internals+2021_V1/about
/// 
#include <Windows.h>
#include <stdio.h>

#pragma warning(disable: 4201)

/// Helps making the code less bloated
#define PRINT_SEGMENT_INFO(Name, Seg) \
	printf("    - " #Name " = 0x%02x | RPL=%x, TI=%s, Index=0x%04x\n", \
		Seg.value, Seg.elem.RPL, Seg.elem.TableIndicator ? "1 (LDT)" : "0 (GDT)", Seg.elem.Index \
	)

#define CALL_AND_CHECK(x) \
	if (!x) { return EXIT_FAILURE; }

/// General information about the driver
#define KSEG_DEVICE_TYPE 0x8000
#define KSEG_DEVICE_NAME L"KSeg"
#define KSEG_DEVICE_PATH L"\\\\.\\KSeg"

/// List of IOCTL exposed by this driver
#define IOCTL_KSEG_QUERY CTL_CODE(KSEG_DEVICE_TYPE, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)


/// List of the different segment register types
#define SEGMENT_CS 0x00
#define SEGMENT_SS 0x01
#define SEGMENT_DS 0x02
#define SEGMENT_ES 0x03
#define SEGMENT_FS 0x04
#define SEGMENT_GS 0x05

/// <summary>
/// C data structure to store the visible part of a segment register.
/// </summary>
typedef union _Segment {
	struct {
		WORD RPL : 2;             // Request Privilege Level
		WORD TableIndicator : 1;  // GDT if TI = 0 otherwise LDT if TI = 1
		WORD Index : 13;          // Index within either the GDT or LDT
	} elem;
	WORD value;
} Segment, *PSegment;

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
} KSEG_OUT, * PKSEG_OUT;

EXTERN_C VOID STDMETHODCALLTYPE _read_cs(PSegment cs);
EXTERN_C VOID STDMETHODCALLTYPE _read_ss(PSegment ss);
EXTERN_C VOID STDMETHODCALLTYPE _read_ds(PSegment ds);
EXTERN_C VOID STDMETHODCALLTYPE _read_es(PSegment es);
EXTERN_C VOID STDMETHODCALLTYPE _read_fs(PSegment fs);
EXTERN_C VOID STDMETHODCALLTYPE _read_gs(PSegment gs);

BOOL QueryDevice(
	_In_ PHANDLE phDevice,
	_In_ LPCSTR  szRegister,
	_In_ UINT16  uiRegisterType
) {
	if (phDevice == NULL || szRegister == NULL)
		return FALSE;

	// 1. Query the device
	KSEG_OUT DataOut = { 0x00 };

	BOOL bSuccess = DeviceIoControl(
		*phDevice,
		IOCTL_KSEG_QUERY,
		&uiRegisterType,
		sizeof(UINT16),
		&DataOut,
		sizeof(KSEG_OUT),
		NULL,
		NULL
	);

	// 2. Check for error and print segment value
	if (!bSuccess) {
		printf("Failed to query the device: %d\n", GetLastError());
		CloseHandle(*phDevice);
		return FALSE;
	}
	printf("%s = 0x%02x | RPL=%x, TI=%s, Index=0x%04x\n",
		szRegister,
		DataOut.Seg.value,
		DataOut.Seg.elem.RPL,
		DataOut.Seg.elem.TableIndicator ? "1 (LDT)" : "0 (GDT)",
		DataOut.Seg.elem.Index
	);

	// 3. Get the information relate to the descriptor itself.
	UINT32 BaseAddress = DataOut.Descriptor.BaseLow 
		| ((UINT32)DataOut.Descriptor.Bytes.BaseMiddle << 16)
		| ((UINT32)DataOut.Descriptor.Bytes.BaseHigh << 24);
	UINT32 Limit = DataOut.Descriptor.LimitLow | (DataOut.Descriptor.Bits.LimitHigh << 16);

	printf("Base Address: 0x%p\n", BaseAddress);
	printf("Limit:        0x%p\n", Limit);
	printf("DPL:          %s (0x%02x)\n",DataOut.Descriptor.Bits.Dpl == 0x00 ? "Kernel Mode" : "User Mode", DataOut.Descriptor.Bits.Dpl);
	printf("Accessed:     %s\n", DataOut.Descriptor.Bits.Accessed ? "true" : "false");
	if (DataOut.Descriptor.Bits.CodeData == 0x00) {
		printf("Writable:     %s\n", DataOut.Descriptor.Bits.WritableReadable ? "Read Only" : "Read & Write");
		printf("Expand Down:  %s\n", DataOut.Descriptor.Bits.ExpandDownConforming ? "true" : "false");
	}
	else {
		printf("Readable:     %s\n", DataOut.Descriptor.Bits.WritableReadable ? "Execute Only" : "Execute & Data");
		printf("Conforming:   %s\n", DataOut.Descriptor.Bits.ExpandDownConforming ? "true" : "false");
	}
	printf("Long mode:    %s\n\n", DataOut.Descriptor.Bits.LongMode ? "true" : "false");

	return TRUE;
}

INT main() {

	// 1. Initialise all the structures that will be used to get the segment registers.
	Segment cs = { 0x00 };
	Segment ss = { 0x00 };
	Segment ds = { 0x00 };
	Segment es = { 0x00 };
	Segment fs = { 0x00 };
	Segment gs = { 0x00 };

	// 2. Get the segment registers of the current user mode process
	printf("[*] User mode process segment registers:\n");

	_read_cs(&cs);
	_read_ss(&ss);
	_read_ds(&ds);
	_read_es(&es);
	_read_fs(&fs);
	_read_gs(&gs);

	PRINT_SEGMENT_INFO(CS, cs);
	PRINT_SEGMENT_INFO(SS, ss);
	PRINT_SEGMENT_INFO(DS, ds);
	PRINT_SEGMENT_INFO(ES, es);
	PRINT_SEGMENT_INFO(FS, fs);
	PRINT_SEGMENT_INFO(GS, gs);

	// 3. Query the driver to get the kernel segment registers and their descriptors
	// 3.1 Get an handle to the device
	printf("\n[*] Kernel mode segment registers:\n");
	HANDLE hDevice = CreateFileW(
		KSEG_DEVICE_PATH,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0x00,
		NULL
	);
	if (hDevice == INVALID_HANDLE_VALUE) {
		printf("Unable to get an handle to the device object: %d\n", GetLastError());
		return EXIT_FAILURE;
	}

	// 3.2. Query the device for all 6 segment registers
	CALL_AND_CHECK(QueryDevice(&hDevice, "CS", SEGMENT_CS));
	CALL_AND_CHECK(QueryDevice(&hDevice, "SS", SEGMENT_SS));
	CALL_AND_CHECK(QueryDevice(&hDevice, "DS", SEGMENT_DS));
	CALL_AND_CHECK(QueryDevice(&hDevice, "ES", SEGMENT_ES));
	CALL_AND_CHECK(QueryDevice(&hDevice, "FS", SEGMENT_FS));
	CALL_AND_CHECK(QueryDevice(&hDevice, "GS", SEGMENT_GS));

	return EXIT_SUCCESS;
}