void __declspec(naked) kmain()
{
	__asm
	{
		mov eax, 0B8002h 
    mov [eax], 'B'      ; Move the ASCII-code of 'B' into first video memory
		inc eax
    mov [eax], 1Bh      ; Assign a color code

hang:
    jmp hang
	}
}