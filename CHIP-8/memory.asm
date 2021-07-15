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
resetloop:
	add eax, 4			; Add 4 bytes to EAX, since the buffer size is
						; sizeof DWORD (4B)
	cmp eax, ebx
	jge resetexit		; If EAX is greater than the memory block length,
						; exit loop.
	cmp [eax], ecx
	je resetloop		; If value of the pointer (EAX) is already ECX,
						; skip address.
	mov [eax], ecx
	jmp resetloop	
resetexit:
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
	push ebx
	mov bl, [ebp+12]	; Load data into EBP.
	mov [eax], bl		; Store data into addr in memory.
	pop ebx
	pop ebp
	ret
asm_mem_store endp

asm_mem_load proc		; chip8.h: asm_mem_load(char* addr);
	push ebp
	mov ebp, esp
	mov eax, [ebp+8]	; Load addr into EAX.
	mov eax, [eax]		; Load memory from addr into EAX.
	xchg ah, al			; Converting EAX to little-endian.
    rol eax, 16
    xchg ah, al
	mov bl, 4			; Max bytes to load is 4.
	mov cl, [ebp+12]	; Load size into EBX.
	sub bl, cl
	mov cl, bl
	shl cl, 3			; Multiply size by 8. (Convert bytes to bits)
	shr eax, cl			; Right-shift EAX by CL.
	pop ebp
	ret
asm_mem_load endp

end