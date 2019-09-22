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
	sub esp, 12
	push ebx
	push esi
	push edi
	invoke crt_scanf, SADD("%d"), addr [ebp-4]
	invoke crt_scanf, SADD("%d"), addr [ebp-8]
	invoke crt_scanf, SADD("%d"), addr [ebp-12]
	mov eax, [ebp-4]
	cmp eax, [ebp-8]
	jl label1
	mov eax, [ebp-4]
	cmp eax, [ebp-12]
	jge label3
	print chr$(" a ge b a l c")
	print chr$(10)
	jmp label4
label3:
	print chr$(" a ge b a ge c")
	print chr$(10)
label4:
	jmp label2
label1:
	mov eax, [ebp-4]
	cmp eax, [ebp-12]
	jge label5
	print chr$("a l b a l c")
	print chr$(10)
	jmp label6
label5:
	print chr$("a l b a ge c")
	print chr$(10)
label6:
label2:
	mov eax, [ebp+8]
	pop edi
	pop esi
	pop ebx
	add esp, 12
	pop ebp
	ret
____main endp

end start
