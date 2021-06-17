/// @file    main.c
/// @author  Paul L. (@am0nsec)
/// @version 1.0
/// @link    https://github.com/am0nsec/ost
/// @brief   Windows specific code for the OpenSecurityTraining2 Architecture 2001 class
///          https://x.ost.fyi/courses/course-v1:OpenSecurityTraining+Arch2001_x86-64_OS_Internals+2021_V1/about
/// 
#include <Windows.h>
#include <stdio.h>

#define SUCCESS(x) (x != 0x00)
#define FAILED(x) !(x != 0x00)

typedef union _BasicInformationEcx {
	struct {
		UINT SSE3 : 1;
		UINT PCLMULQDQ : 1;
		UINT ReserveF : 1;
		UINT MONITOR : 1;
		UINT ReserveE : 1;
		UINT ReserveD : 1;
		UINT ReserveC : 1;
		UINT ReserveB : 1;
		UINT SSSE3 : 1;
		UINT ReserveA : 1;
		UINT Reserve9 : 1;
		UINT FMA : 1;
		UINT CMPXCHG16B : 1;
		UINT Reserve8 : 1;
		UINT Reserve7 : 1;
		UINT Reserve6 : 1;
		UINT Reserve5 : 1;
		UINT Reserve4 : 1;
		UINT SSE41 : 1;
		UINT SSE42 : 1;
		UINT Reserved3 : 1;
		UINT Reserved2 : 1;
		UINT POPCNT : 1;
		UINT Reserved1 : 1;
		UINT AES : 1;
		UINT XSAVE : 1;
		UINT OSXSAVE : 1;
		UINT AVX : 1;
		UINT F16C : 1;
		UINT Reserved0 : 1;
		UINT RAZ : 1;
	} elem;
	UINT value;
} BasicInformationEcx, *PBasicInformationEcx;

typedef union _BasicInformationEdx {
	struct {
		UINT FPU : 1;
		UINT VME : 1;
		UINT DE : 1;
		UINT PSE : 1;
		UINT TSC : 1;
		UINT MSR : 1;
		UINT PAE : 1;
		UINT MCE : 1;
		UINT CMPXCHG8B : 1;
		UINT APIC : 1;
		UINT Reserve9 : 1;
		UINT SysEnterSysExit : 1;
		UINT MTRR : 1;
		UINT PGE : 1;
		UINT MCA : 1;
		UINT CMOV : 1;
		UINT PAT : 1;
		UINT PSE36 : 1;
		UINT Reserve8 : 1;
		UINT CLFSH : 1;
		UINT Reserve7 : 1;
		UINT Reserve6 : 1;
		UINT Reserve5 : 1;
		UINT MMX : 1;
		UINT FXSR : 1;
		UINT SSE : 1;
		UINT SSE2 : 1;
		UINT HTT : 1;
		UINT Reserve4 : 1;
		UINT Reserve3 : 1;
		UINT Reserve2 : 1;
		UINT Reserve1 : 1;
	} elem;
	UINT value;
} BasicInformationEdx, * PBasicInformationEdx;

typedef union _StructuredExtendedFeatureEbx {
	struct {
		UINT FSGSBASE : 1;
		UINT IA32_TSC_ADJUST : 1;
		UINT SGX : 1;
		UINT BMI1 : 1;
		UINT HLE : 1;
		UINT AVX2 : 1;
		UINT FDP_EXCPTN_ONLY : 1;
		UINT SMEP : 1;
		UINT BMI2 : 1;
		UINT EnhancedREP : 1;
		UINT INVPCID : 1;
		UINT RTM : 1;
		UINT RDTM : 1;
		UINT DeprecatesFPUCS : 1;
		UINT MPX : 1;
		UINT RDTA : 1;
		UINT AVX512F : 1;
		UINT AVX512DQ : 1;
		UINT RDSEED : 1;
		UINT ADX : 1;
		UINT SMAP : 1;
		UINT AVX512_IFMA : 1;
		UINT Reserved0 : 1;
		UINT CLFLUSHOPT : 1;
		UINT CLWB : 1;
		UINT IntelProcessorTrace : 1;
		UINT AVX512PF : 1;
		UINT AVX512ER : 1;
		UINT AVX512CD : 1;
		UINT SHA : 1;
		UINT AVX512BW : 1;
		UINT AVX512VL : 1;
	} elem;
	UINT value;
} StructuredExtendedFeatureEbx, *PStructuredExtendedFeatureEbx;


/// <summary>
/// Check whether CPUID instruction is supported.
/// </summary>
/// <returns>True if CPUID instruction is supported.</returns>
_Success_(return != 0x00) _Must_inspect_result_
EXTERN_C BYTE STDMETHODCALLTYPE IsCPUIDSupported();

/// <summary>
/// Execute the CPUID instruction to get information about the system.
/// </summary>
/// <param name="pEAX">Pointer to the EAX register.</param>
/// <param name="pECX">Pointer to the ECX register.</param>
/// <param name="pEBX">Pointer to the EBX register.</param>
/// <param name="pEDX">Pointer to the EDX register.</param>
/// <returns>Whether the CPUID instruction has been executed successfully.</returns>
_Success_(return != 0x00) _Must_inspect_result_
EXTERN_C UINT STDMETHODCALLTYPE CPUIDEX(
	_Inout_ PUINT pEAX,
	_Inout_ PUINT pECX,
	_Inout_ PUINT pEBX,
	_Inout_ PUINT pEDX
);

/// <summary>
/// Entry point of the application.
/// </summary>
/// <returns>Process exit status code.</returns>
INT main() {
	UINT eax = 0x00;
	UINT ecx = 0x00;
	UINT ebx = 0x00;
	UINT edx = 0x00;

	// 1. Check if CPUID is supported.
	if (!IsCPUIDSupported()) {
		printf("CPUID instruction is not supported by the microprocessor.\n");
		return EXIT_FAILURE;
	}

	// 2. Get the microprocessor vendor
	UINT uiResult = CPUIDEX(&eax, &ecx, &ebx, &edx);
	if (FAILED(uiResult)) {
		printf("Unable to get the microprocessor vendor.\n");
		return EXIT_FAILURE;
	}

	LPCSTR szVendorName = (LPCSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, eax);
	if (szVendorName == NULL)
		return EXIT_FAILURE;
	RtlCopyMemory(&(szVendorName[0]), &ebx, sizeof(UINT));
	RtlCopyMemory(&(szVendorName[4]), &edx, sizeof(UINT));
	RtlCopyMemory(&(szVendorName[8]), &ecx, sizeof(UINT));
	printf("Microprocessor vendor: %s\n", szVendorName);

	// 3. Check various features and capabilities.
	ecx = 0x00;
	eax = 0x01;
	if (FAILED(CPUIDEX(&eax, &ecx, &ebx, &edx))) {
		printf("Unable to get the microprocessor feature identifiers.\n");
		return EXIT_FAILURE;
	}
	BasicInformationEcx Feature1 = { .value = ecx };
	BasicInformationEdx Feature2 = { .value = edx };

	printf("Basic CPUID Information:\n");
	printf("   - F16C: half-precision convert instruction support (%s)\n",                  Feature1.elem.F16C == 1 ? "true" : "false");
	printf("   - AVX: AVX instruction support (%s)\n",                                      Feature1.elem.AVX  == 1 ? "true" : "false");
	printf("   - OSXSAVE: XSAVE (and related) instructions are enabled (%s)\n",             Feature1.elem.OSXSAVE == 1 ? "true" : "false");
	printf("   - XSAVE: XSAVE (and related) instructions are supported by hardware (%s)\n", Feature1.elem.XSAVE == 1 ? "true" : "false");
	printf("   - AES: AES instruction support (%s)\n",                                      Feature1.elem.AES == 1 ? "true" : "false");
	printf("   - POPCNT: POPCNT instruction (%s)\n",                                        Feature1.elem.POPCNT == 1 ? "true" : "false");
	printf("   - SSE42: SSE4.2 instruction support (%s)\n",                                 Feature1.elem.SSE42 == 1 ? "true" : "false");
	printf("   - SSE41: SSE4.1 instruction support (%s)\n",                                 Feature1.elem.SSE41 == 1 ? "true" : "false");
	printf("   - CMPXCHG16B: CMPXCHG16B instruction (%s)\n",                                Feature1.elem.CMPXCHG16B == 1 ? "true" : "false");
	printf("   - FMA: FMA instruction support (%s)\n",                                      Feature1.elem.FMA == 1 ? "true" : "false");
	printf("   - SSSE3: supplemental SSE3 instruction support (%s)\n",                      Feature1.elem.SSSE3 == 1 ? "true" : "false");
	printf("   - MONITOR: MONITOR/MWAIT instructions (%s)\n",                               Feature1.elem.MONITOR == 1 ? "true" : "false");
	printf("   - PCLMULQDQ: PCLMULQDQ instruction support (%s)\n",                          Feature1.elem.PCLMULQDQ == 1 ? "true" : "false");
	printf("   - SSE3: SSE3 instruction support (%s)\n",                                    Feature1.elem.SSE3 == 1 ? "true" : "false");
	printf("   - FPU: x87 floating point unit on-chip (%s)\n",               Feature2.elem.FPU == 1 ? "true" : "false");
	printf("   - VME: virtual-mode enhancements (%s)\n",                     Feature2.elem.VME == 1 ? "true" : "false");
	printf("   - DE: debugging extensions (%s)\n",                           Feature2.elem.DE == 1 ? "true" : "false");
	printf("   - PSE: page-size extensions (%s)\n",                          Feature2.elem.PSE == 1 ? "true" : "false");
	printf("   - TSC: time stamp counter (%s)\n",                            Feature2.elem.TSC == 1 ? "true" : "false");
	printf("   - MSR: AMD model-specific registers (%s)\n",                  Feature2.elem.MSR == 1 ? "true" : "false");
	printf("   - PAE: physical-address extensions (%s)\n",                   Feature2.elem.PAE == 1 ? "true" : "false");
	printf("   - MCE: Machine check exception (%s)\n",                       Feature2.elem.MCE == 1 ? "true" : "false");
	printf("   - CMPXCHG8B: CMPXCHG8B instruction (%s)\n",                   Feature2.elem.CMPXCHG8B == 1 ? "true" : "false");
	printf("   - APIC: advanced programmable interrupt controller (%s)\n",   Feature2.elem.APIC == 1 ? "true" : "false");
	printf("   - SysEnterSysExit: SYSENTER and SYSEXIT instructions (%s)\n", Feature2.elem.SysEnterSysExit == 1 ? "true" : "false");
	printf("   - MTRR: memory-type range registers (%s)\n",                  Feature2.elem.MTRR == 1 ? "true" : "false");
	printf("   - PGE: page global extension (%s)\n",                         Feature2.elem.PGE == 1 ? "true" : "false");
	printf("   - MCA: machine check architecture (%s)\n",                    Feature2.elem.MCA == 1 ? "true" : "false");
	printf("   - CMOV: conditional move instructions (%s)\n",                Feature2.elem.CMOV == 1 ? "true" : "false");
	printf("   - PAT: page attribute table (%s)\n",                          Feature2.elem.PAT == 1 ? "true" : "false");
	printf("   - PSE36: page-size extensions (%s)\n",                        Feature2.elem.PSE36 == 1 ? "true" : "false");
	printf("   - CLFSH: CLFLUSH instruction support (%s)\n",                 Feature2.elem.CLFSH == 1 ? "true" : "false");
	printf("   - MMX: MMX™ instructions (%s)\n",                             Feature2.elem.MMX == 1 ? "true" : "false");
	printf("   - FXSR: FXSAVE and FXRSTOR instructions (%s)\n",              Feature2.elem.FXSR == 1 ? "true" : "false");
	printf("   - SSE: SSE instruction support (%s)\n",                       Feature2.elem.SSE == 1 ? "true" : "false");
	printf("   - SSE2: SSE2 instruction support (%s)\n",                     Feature2.elem.SSE2 == 1 ? "true" : "false");
	printf("   - HTT: hyper-threading technology (%s)\n",                    Feature2.elem.HTT == 1 ? "true" : "false");


	// 4. Get the Structured Extended Feature Flags Enumeration Leaf 
	ecx = 0x00;
	eax = 0x07;
	if (FAILED(CPUIDEX(&eax, &ecx, &ebx, &edx))) {
		printf("Unable to get the microprocessor feature identifiers.\n");
		return EXIT_FAILURE;
	}
	StructuredExtendedFeatureEbx ExtendedFeatures = { .value = ebx };

	printf("Structured Extended Feature Flags Enumeration Leaf:\n");
	printf("   - FSGSBASE: Supports RDFSBASE/RDGSBASE/WRFSBASE/WRGSBASE (%s)\n",                                 ExtendedFeatures.elem.FSGSBASE == 1 ? "true" : "false");
	printf("   - IA32_TSC_ADJUST MSR is supported (%s)\n",                                                       ExtendedFeatures.elem.IA32_TSC_ADJUST == 1 ? "true" : "false");
	printf("   - SGX: Supports Intel® Software Guard Extensions (Intel® SGX Extensions) (%s)\n",                 ExtendedFeatures.elem.SGX == 1 ? "true" : "false");
	printf("   - BMI1 (%s)\n",                                                                                   ExtendedFeatures.elem.BMI1 == 1 ? "true" : "false");
	printf("   - HLE (%s)\n",                                                                                    ExtendedFeatures.elem.HLE == 1 ? "true" : "false");
	printf("   - AVX2 (%s)\n",                                                                                   ExtendedFeatures.elem.AVX2 == 1 ? "true" : "false");
	printf("   - FDP_EXCPTN_ONLY: x87 FPU Data Pointer updated only on x87 exceptions (%s)\n",                   ExtendedFeatures.elem.FDP_EXCPTN_ONLY == 1 ? "true" : "false");
	printf("   - SMEP: Supports Supervisor-Mode Execution Prevention (%s)\n",                                    ExtendedFeatures.elem.SMEP == 1 ? "true" : "false");
	printf("   - BMI2 (%s)\n",                                                                                   ExtendedFeatures.elem.BMI2 == 1 ? "true" : "false");
	printf("   - Supports Enhanced REP MOVSB/STOSB (%s)\n",                                                      ExtendedFeatures.elem.EnhancedREP == 1 ? "true" : "false");
	printf("   - INVPCID (%s)\n",                                                                                ExtendedFeatures.elem.INVPCID == 1 ? "true" : "false");
	printf("   - RTM (%s)\n",                                                                                    ExtendedFeatures.elem.RTM == 1 ? "true" : "false");
	printf("   - RDT-M: Supports Intel® Resource Director Technology (Intel® RDT) Monitoring capability (%s)\n", ExtendedFeatures.elem.RDTM == 1 ? "true" : "false");
	printf("   - Deprecates FPU CS and FPU DS values (%s)\n",                                                    ExtendedFeatures.elem.DeprecatesFPUCS == 1 ? "true" : "false");
	printf("   - MPX: Supports Intel® Memory Protection Extensions (%s)\n",                                      ExtendedFeatures.elem.MPX == 1 ? "true" : "false");
	printf("   - RDT-A: Supports Intel® Resource Director Technology (Intel® RDT) Allocation capability (%s)\n", ExtendedFeatures.elem.RDTA == 1 ? "true" : "false");
	printf("   - AVX512F (%s)\n",                                                                                ExtendedFeatures.elem.AVX512F == 1 ? "true" : "false");
	printf("   - AVX512DQ (%s)\n",                                                                               ExtendedFeatures.elem.AVX512DQ == 1 ? "true" : "false");
	printf("   - RDSEED (%s)\n",                                                                                 ExtendedFeatures.elem.RDSEED == 1 ? "true" : "false");
	printf("   - ADX (%s)\n",                                                                                    ExtendedFeatures.elem.ADX == 1 ? "true" : "false");
	printf("   - SMAP: Supports Supervisor-Mode Access Prevention (and the CLAC/STAC instructions) (%s)\n",      ExtendedFeatures.elem.SMAP == 1 ? "true" : "false");
	printf("   - AVX512_IFMA (%s)\n",                                                                            ExtendedFeatures.elem.AVX512_IFMA == 1 ? "true" : "false");
	printf("   - CLFLUSHOPT (%s)\n",                                                                             ExtendedFeatures.elem.CLFLUSHOPT == 1 ? "true" : "false");
	printf("   - CLWB (%s)\n",                                                                                   ExtendedFeatures.elem.CLWB == 1 ? "true" : "false");
	printf("   - Intel Processor Trace (%s)\n",                                                                  ExtendedFeatures.elem.IntelProcessorTrace == 1 ? "true" : "false");
	printf("   - AVX512PF (%s)\n",                                                                               ExtendedFeatures.elem.AVX512PF == 1 ? "true" : "false");
	printf("   - AVX512ER (%s)\n",                                                                               ExtendedFeatures.elem.AVX512ER == 1 ? "true" : "false");
	printf("   - AVX512CD (%s)\n",                                                                               ExtendedFeatures.elem.AVX512CD == 1 ? "true" : "false");
	printf("   - SHA: supports Intel® Secure Hash Algorithm Extensions (Intel® SHA Extensions) (%s)\n",          ExtendedFeatures.elem.SHA == 1 ? "true" : "false");
	printf("   - AVX512BW (%s)\n",                                                                               ExtendedFeatures.elem.AVX512BW == 1 ? "true" : "false");
	printf("   - AVX512VL (%s)\n",                                                                               ExtendedFeatures.elem.AVX512VL == 1 ? "true" : "false");

	return EXIT_SUCCESS;
}