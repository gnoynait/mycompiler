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


____main proc
	push ebp
	mov ebp, esp
	sub esp, 104
	push ebx
	push esi
	push edi
	mov eax, 1
	mov [ebp-24], eax
label1:
	mov eax, [ebp-24]
	cmp eax, 3
	jg label2
	invoke crt_scanf, SADD("%c"), addr [ebp-76]
	invoke crt_scanf, SADD("%c"), addr [ebp-80]
	mov eax, [ebp-76]
	mov [ebp-4], eax
	mov eax, [ebp-80]
	mov [ebp-8], eax
	print chr$(" the first charater is ")
	invoke crt_printf, SADD("%c "), dword ptr [ebp-76]
	print chr$(10)
	print chr$(" ths acii of c1 is ")
	invoke crt_printf, SADD("%d "), dword ptr [ebp-4]
	print chr$(10)
	print chr$(" the second charater is ")
	invoke crt_printf, SADD("%c "), dword ptr [ebp-80]
	print chr$(10)
	print chr$(" the acii of c2 is ")
	invoke crt_printf, SADD("%d "), dword ptr [ebp-8]
	print chr$(10)
	mov eax, 5
	mov [ebp-16], eax
	mov eax, 4
	mov [ebp-20], eax
	mov eax, [ebp-8]
	mov ecx, 5
	mov edx, 0
	idiv ecx
	mov [ebp-84], eax
	mov eax, [ebp-4]
	add eax, [ebp-84]
	mov [ebp-88], eax
	mov eax, [ebp-88]
	add eax, [ebp-16]
	mov [ebp-92], eax
	mov eax, [ebp-92]
	sub eax, [ebp-20]
	mov [ebp-96], eax
	mov eax, [ebp-96]
	mov edx, [ebp-76]
	imul edx
	mov [ebp-100], eax
	mov eax, [ebp-100]
	sub eax, [ebp-80]
	mov [ebp-104], eax
	invoke crt_printf, SADD("%d "), dword ptr [ebp-104]
	print chr$(10)
	mov eax, [ebp-24]
	inc eax
	mov [ebp-24], eax
	jmp label1
label2:
	mov eax, [ebp+8]
	pop edi
	pop esi
	pop ebx
	add esp, 104
	pop ebp
	ret
____main endp

end start
