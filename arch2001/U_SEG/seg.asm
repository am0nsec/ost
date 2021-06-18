;; @file    seg.asm
;; @author  Paul L. (@am0nsec)
;; @version 1.0
;; @link    https://github.com/am0nsec/ost
;; @brief   Windows specific code for the OpenSecurityTraining2 Architecture 2001 class
;;          https://x.ost.fyi/courses/course-v1:OpenSecurityTraining+Arch2001_x86-64_OS_Internals+2021_V1/about
;; 
.code

_read_cs PROC PUBLIC
	mov word ptr [rcx], cs
	ret
_read_cs ENDP

_read_ss PROC PUBLIC
	mov word ptr [rcx], ss
	ret
_read_ss ENDP

_read_ds PROC PUBLIC
	mov word ptr [rcx], ds
	ret
_read_ds ENDP

_read_es PROC PUBLIC
	mov word ptr [rcx], es
	ret
_read_es ENDP

_read_fs PROC PUBLIC
	mov word ptr [rcx], fs
	ret
_read_fs ENDP

_read_gs PROC PUBLIC
	mov word ptr [rcx], gs
	ret
_read_gs ENDP

;; End of file
end