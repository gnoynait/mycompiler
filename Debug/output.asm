.486
.model flat, stdcall
option casemap :none
include     C:\Masm32\include\windows.inc
include     C:\Masm32\include\kernel32.inc
include     C:\Masm32\include\msvcrt.inc
include     C:\masm32\include\masm32.inc
include     C:\Masm32\macros\macros.asm
includelib  C:\Masm32\lib\msvcrt.lib
includelib  C:\Masm32\lib\kernel32.lib
includelib  C:\masm32\lib\masm32.lib
.code

start:
	push eax
	call ____main
	exit


__p proc
	push ebp
	mov ebp, esp
	sub esp, 0
	push ebx
	push esi
	push edi
	mov eax, 1
	mov [ebp+8], eax
	mov eax, [ebp+8]
	pop edi
	pop esi
	pop ebx
	add esp, 0
	pop ebp
	ret
__p endp


____main proc
	push ebp
	mov ebp, esp
	sub esp, 8
	push ebx
	push esi
	push edi
	push ebp
	push eax
	call __p
	add esp, 16
	mov [ebp-8], eax
	mov eax, 1
	mov [ebp-4], eax
	mov eax, [ebp+8]
	pop edi
	pop esi
	pop ebx
	add esp, 8
	pop ebp
	ret
____main endp

end start
