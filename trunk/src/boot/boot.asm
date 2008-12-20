;------------------------------------------------------------------------------
; GenOS boot loader
;   This code is loaded by the BIOS at the linear address 7C00h
;------------------------------------------------------------------------------
; Copyright (c) 2008, Cedric Rousseau
; All rights reserved.
; 
; Redistribution and use in source and binary forms, with or without 
; modification, are permitted provided that the following conditions are met:
; 1. Redistributions of source code must retain the above copyright
;    notice, this list of conditions, and the following disclaimer.
; 2. Redistributions in binary form must reproduce the above copyright
;    notice, this list of conditions, and the following disclaimer in the
;    documentation and/or other materials provided with the distribution.
; 3. The name of the author may not be used to endorse or promote products
;    derived from this software without specific prior written permission.
; 
; THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR
; IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
; OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
; IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
; INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
; NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
; DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
; THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
; THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;------------------------------------------------------------------------------

%define BOOT_SEG              07C0h

%define INT_VIDEO             int 10h
%define INT_DISK              int 13h

%define VIDEO_OUTPUT_CHAR     0Eh

%define DISK_RESET            00h
%define DISK_READ_SECTORS     02h

[BITS 16]                   ; the BIOS starts out in 16-bit real mode
[ORG 7C00h]

    jmp BOOT_SEG:EntryPoint
    
;------------------------------------------------------------------------------
; Data
;------------------------------------------------------------------------------
boot_drive      db 0
gdt:
        dd 0            ; Null Segment
        dd 0

        dw 0FFFFh       ; Code segment, read/execute, nonconforming
        dw 0
        db 0
        db 10011010b
        db 11001111b
        db 0

        dw 0FFFFh       ; Data segment, read/write, expand down
        dw 0
        db 0
        db 10010010b
        db 11001111b
        db 0

gdt_desc:                       ; The GDT descriptor
        dw gdt_desc - gdt - 1    ; Limit (size)
        dd gdt                  ; Address of the GDT



EntryPoint:
;------------------------------------------------------------------------------
; The main procedure of the boot process
;------------------------------------------------------------------------------
    mov ax, cs                 ; The BIOS loads the boot sector at 0000:7C00h, so set DS accordinly
    mov ds, ax                 ; Therefore, we don't have to add 7C00h to all our data

    mov [boot_drive], dl       ; Save the BIOS number of the bootable device


reset_disk:                      
    mov ax, DISK_RESET         ; Reset the floppy drive
    mov dl, [boot_drive]       ; Set the drive
    INT_DISK
    jc reset_disk	       ; loop until success


read_sector:
    mov ax, 01000h             ; write into 07C0:new_sector
    mov es, ax                 ;
    xor bx, bx                 ;

    mov ah, DISK_READ_SECTORS  ; Read sector from drive
    mov al, 2                  ; Load 1 sector
    mov ch, 0                  ; Cylinder=0
    mov cl, 2                  ; Sector=2
    mov dh, 0                  ; Head=0
    mov dl, [boot_drive]       ; Bootable drive
    INT_DISK
    jc read_sector             ; loop until success


    cli                     ; no more interruptions
    

    ; set A20 line
    
    xor cx, cx
    
clear_buf:
    in al, 64h              ; get input from keyboard status port
    test al, 02h            ; test the buffer full flag
    loopnz clear_buf        ; loop until buffer is empty
    mov al, 0D1h            ; keyboard: write to output port
    out 64h, al             ; output command to keyboard
clear_buf2:
    in al, 64h              ; wait 'till buffer is empty again
    test al, 02h
    loopnz clear_buf2
    mov al, 0dfh            ; keyboard: set A20
    out 60h, al             ; send it to the keyboard controller
    mov cx, 14h
wait_kbc:                   ; this is approx. a 25uS delay to wait
    out 0edh, ax            ; for the kb controler to execute our 
    loop wait_kbc           ; command.

    
    ; load GDT
    xor ax, ax
    mov ds, ax              ; Set DS-register to 0 - used by lgdt

    lgdt [gdt_desc]         ; Load the GDT descriptor


    ; enter pmode
    mov eax, cr0            ; load the control register in
    or  al, 1               ; set bit 0: pmode bit
    mov cr0, eax            ; copy it back to the control register

    jmp 08h:protected_mode

[Bits 32]
protected_mode:   
    mov ax, 10h             ; Save data segment identifier
    mov ds, ax              ; Move a valid data segment into the data segment register
    mov ss, ax              ; Move a valid data segment into the stack segment register
    mov esp, 090000h        ; Move the stack pointer to 090000h

    mov byte [0B8000h], 'A'      ; Move the ASCII-code of 'P' into first video memory
    mov byte [0B8001h], 1Bh      ; Assign a color code

    jmp 010200h



;------------------------------------------------------------------------------
; A valid boot sector must have the signature 0xAA55 at offset 510
;------------------------------------------------------------------------------
    times 510-($-$$) db 0
    dw 0xAA55
