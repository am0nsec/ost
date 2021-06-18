/// @file    main.c
/// @author  Paul L. (@am0nsec)
/// @version 1.0
/// @link    https://github.com/am0nsec/ost
/// @brief   Windows specific code for the OpenSecurityTraining2 Architecture 2001 class
///          https://x.ost.fyi/courses/course-v1:OpenSecurityTraining+Arch2001_x86-64_OS_Internals+2021_V1/about
/// 
#include <Windows.h>
#include <stdio.h>

#define PRINT_SEGMENT_INFO(Name, Seg) \
	printf("    - " #Name " = 0x%02x | RPL=%x, TI=%s, Index=0x%04x\n", \
		Seg.value, Seg.elem.RPL, Seg.elem.TableIndicator ? "1 (LDT)" : "0 (GDT)", Seg.elem.Index \
	)

typedef union _Segment {
	struct {
		WORD RPL : 2;             // Request Privilege Level
		WORD TableIndicator : 1;  // GDT if TI = 0 otherwise LDT if TI = 1
		WORD Index : 13;          // Index within either the GDT or LDT
	} elem;
	WORD value;
} Segment, *PSegment;

EXTERN_C VOID STDMETHODCALLTYPE _read_cs(PSegment cs);
EXTERN_C VOID STDMETHODCALLTYPE _read_ss(PSegment ss);
EXTERN_C VOID STDMETHODCALLTYPE _read_ds(PSegment ds);
EXTERN_C VOID STDMETHODCALLTYPE _read_es(PSegment es);
EXTERN_C VOID STDMETHODCALLTYPE _read_fs(PSegment fs);
EXTERN_C VOID STDMETHODCALLTYPE _read_gs(PSegment gs);


INT main() {

	// 1. Initialise all the structurs that will be used to get the segment registers.
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

	return EXIT_SUCCESS;
}