.386
.model flat, c

.code
asm_mem_reset proc		; chip8.h: asm_mem_reset(char* addr, int len);
	push ebp
	mov ebp, esp
						; For some reason we need to push/pop all used
						; registers. Otherwise it breaks on release.
						; Got me (an asm noob) debugging for a while.
						; StackOverflow didn't help at all.
	push eax
	mov eax, [ebp+8]	; Load addr into EAX
						; EAX will be memory pointer.
	push ebx
	mov ebx, [ebp+12]	; Load len into EBX.
	add ebx, eax		; Add memory pointer to len

	push ecx
	xor ecx, ecx		; It is faster to allocate large memory blocks 
						; using DWORDS rather than BYTES.
						; ECX can be any DWORD value.
						; If you are at a funny mood, you can even fill
						; the whole memory with 0x69s (haha funny number)
						; using "mov ecx, 069696969h"
	mov [eax], ecx
forloop:
	add eax, 4			; Add 4 bytes to EAX, since the buffer size is
						; sizeof DWORD (4B)
	cmp eax, ebx
	jge exit			; If EAX is greater than the memory block length,
						; exit loop.
	cmp [eax], ecx
	je forloop			; If value of the pointer (EAX) is already ECX,
						; skip address.
	mov [eax], ecx
	jmp forloop	
exit:
	pop ecx				; Aaaaaand... popping.
	pop ebx				; God I love assembly.
	pop eax
	pop ebp
	ret
asm_mem_reset endp

asm_mem_store proc		; chip8.h: asm_mem_store(char* addr, char data);
	push ebp
	mov ebp, esp
	mov eax, [ebp+8]	; Load addr into EAX.
	mov bl, [ebp+12]	; Load data into EBP.
	mov [eax], bl		; Store data into addr in memory.
	pop ebp
	ret
asm_mem_store endp

asm_mem_load proc		; chip8.h: asm_mem_load(char* addr);
	push ebp
	mov ebp, esp
	mov eax, [ebp+8]	; Load addr into EAX.
	mov eax, [eax]		; Load memory from addr into EAX.
	and eax, 000000FFh	; Get only the last byte.
	pop ebp
	ret
asm_mem_load endp

end