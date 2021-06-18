/// @file    main.c
/// @author  Paul L. (@am0nsec)
/// @version 1.0
/// @link    https://github.com/am0nsec/ost
/// @brief   Windows specific code for the OpenSecurityTraining2 Architecture 2001 class
///          https://x.ost.fyi/courses/course-v1:OpenSecurityTraining+Arch2001_x86-64_OS_Internals+2021_V1/about
/// 
#include <Windows.h>
#include <stdio.h>

/// General information about the driver
#define KMSR_DEVICE_PATH L"\\\\.\\KMsr"
#define KMSR_DEVICE_TYPE 0x8000

/// List of IOCTL exposed by this driver
#define IOCTL_KMSR_READ  CTL_CODE(KMSR_DEVICE_TYPE, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_KMSR_WRITE CTL_CODE(KMSR_DEVICE_TYPE, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

/// Example of IA-32 Architectural MSRs
#define IA32_STAR           0xC0000081 // System Call Target Address (R/W)
#define IA32_LSTAR          0xC0000082 // IA-32e Mode System Call Target Address (R/W). Target RIP for the called procedure when SYSCALL is executed in 64-bit mode.
#define IA32_CSTAR          0xC0000083 // IA-32e Mode System Call Target Address (R/W). Not used, as the SYSCALL instruction is not recognized in compatibility mode.
#define IA32_FMASK          0xC0000084 // System Call Flag Mask (R/W)
#define IA32_FS_BASE        0xC0000100 // Map of BASE Address of FS (R/W)
#define IA32_GS_BASE        0xC0000101 // Map of BASE Address of GS (R/W)
#define IA32_KERNEL_GS_BASE 0xC0000102 // Swap Target of BASE Address of GS (R/W
#define IA32_TSC_AUX        0xC0000103 // Auxiliary TSC (RW)

typedef struct _WRMSR_IN {
	UINT EAX;
	UINT EDX;
} WRMSR_IN, *PWRMSR_IN;

typedef UINT RDMSR_IN;
typedef PUINT PRDMSR_IN;

typedef struct _RDMSR_OUT {
	UINT EAX;
	UINT EDX;
} RDMSR_OUT, *PRDMSR_OUT;

#define QUERY_AND_CHECK(x) \
	if (!x) { goto error; }

_Success_(return != 0x00) _Must_inspect_result_
BYTE QueryDevice(
	_In_  PHANDLE  phDevice,
	_In_  UINT     uiMSR,
	_Out_ PDWORD64 pdwValue
) {
	if (phDevice == NULL || *phDevice == INVALID_HANDLE_VALUE || pdwValue == NULL)
		return FALSE;
	*pdwValue = 0x00;

	// 1. Get the stack variables ready
	RDMSR_IN  InData = uiMSR;
	RDMSR_OUT OutData = { 0x00 };
	DWORD dwBytesReturned = 0x00;

	// 2. Query the device
	BOOL bSuccess = DeviceIoControl(
		*phDevice,
		IOCTL_KMSR_READ,
		&InData,
		sizeof(RDMSR_IN),
		&OutData,
		sizeof(OutData),
		&dwBytesReturned,
		NULL
	);

	// 3. Check for error 
	if (!bSuccess) {
		printf("Failed to query the device: %d\n", GetLastError());
		return FALSE;
	}

	// 4. Modify varaibles by reference
	*pdwValue = (DWORD64)OutData.EDX << 32 | (DWORD64)OutData.EAX;
	return TRUE;
}

/// <summary>
/// Entry point of the application.
/// </summary>
/// <returns>Process exit status code.</returns>
INT main() {
	
	// 1. Get an handle to the device object.
	HANDLE hDevice = CreateFileW(
		KMSR_DEVICE_PATH,
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
	printf("Handle to the device: 0x%08X\n\n", hDevice);

	// 2. Get various MSRs
	DWORD64 dwMsrValue = 0x00;
	QUERY_AND_CHECK(QueryDevice(&hDevice, IA32_STAR, &dwMsrValue));
	printf("IA32_STAR           : 0x%p\n", dwMsrValue);

	QUERY_AND_CHECK(QueryDevice(&hDevice, IA32_LSTAR, &dwMsrValue));
	printf("IA32_LSTAR          : 0x%p\n", dwMsrValue);

	QUERY_AND_CHECK(QueryDevice(&hDevice, IA32_CSTAR, &dwMsrValue));
	printf("IA32_CSTAR          : 0x%p\n", dwMsrValue);

	QUERY_AND_CHECK(QueryDevice(&hDevice, IA32_FMASK, &dwMsrValue));
	printf("IA32_FMASK          : 0x%p\n", dwMsrValue);

	QUERY_AND_CHECK(QueryDevice(&hDevice, IA32_FS_BASE, &dwMsrValue));
	printf("IA32_FS_BASE        : 0x%p\n", dwMsrValue);

	QUERY_AND_CHECK(QueryDevice(&hDevice, IA32_GS_BASE, &dwMsrValue));
	printf("IA32_GS_BASE        : 0x%p\n", dwMsrValue);

	QUERY_AND_CHECK(QueryDevice(&hDevice, IA32_KERNEL_GS_BASE, &dwMsrValue));
	printf("IA32_KERNEL_GS_BASE : 0x%p\n", dwMsrValue);

	QUERY_AND_CHECK(QueryDevice(&hDevice, IA32_TSC_AUX, &dwMsrValue));
	printf("IA32_TSC_AUX        : 0x%p\n", dwMsrValue);

	// 3. close handle and exit
	CloseHandle(hDevice);
	return EXIT_SUCCESS;

error:
	printf("Failed to query the device: %d\n", GetLastError());
	CloseHandle(hDevice);
	return EXIT_FAILURE;
}