;; @file    kmsr.c
;; @author  Paul L. (@am0nsec)
;; @version 1.0
;; @link    https://github.com/am0nsec/ost
;; @brief   Windows specific code for the OpenSecurityTraining2 Architecture 2001 class
;;          https://x.ost.fyi/courses/course-v1:OpenSecurityTraining+Arch2001_x86-64_OS_Internals+2021_V1/about
;;

.code 
_rdmsr PROC PUBLIC
	; 1. Get the data out of the MSR
	mov ecx, dword ptr [rcx]
	mov r10, rdx
	rdmsr

	; 2. Save the data in the structure
	mov dword ptr [r10 + 0], eax
	mov dword ptr [r10 + 4], edx
	ret
_rdmsr ENDP

;; End of file
end