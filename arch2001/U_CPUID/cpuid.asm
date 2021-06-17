;; @file    cpuid.asm
;; @author  Paul L. (@am0nsec)
;; @version 1.0
;; @link    https://github.com/am0nsec/ost
;; @brief   Windows specific code for the OpenSecurityTraining2 Architecture 2001 class
;;          https://x.ost.fyi/courses/course-v1:OpenSecurityTraining+Arch2001_x86-64_OS_Internals+2021_V1/about
;; 

.code
IsCPUIDSupported PROC PUBLIC
	; 1. Get the RFLAG value and store copy
	pushfq
	mov rbx, qword ptr [rsp]

	; 2. Check if we can get the bit value
check_bit:
	bts qword ptr [rsp], 21
	jnc skip
	btr qword ptr [rsp], 21

	; 3. Check if we can change the value
skip:
	popfq
	xor eax, eax
	inc eax

	pushfq
	test qword ptr [rsp], rbx
	jne done
	xor eax, eax

	; 4. Finally reset back to original value and return
done:
	mov qword ptr [rsp], rbx
	popfq
	ret
IsCPUIDSupported ENDP

CPUIDEX PROC PUBLIC
	; 1. Save all registers
	mov r10, rcx
	mov r11, rdx
	mov r12, r8
	mov r13, r9

	; 2. Get the branche and leaf to query
	mov eax, dword ptr [r10]
	mov ecx, dword ptr [r11]
	cpuid

	; 3. return the data
	mov dword ptr[r10], eax
	mov dword ptr[r11], ecx
	mov dword ptr[r12], ebx
	mov dword ptr[r13], edx

	; 4. Return success
	xor eax, eax
	inc eax
	ret
CPUIDEX ENDP

;; End of file.
end 