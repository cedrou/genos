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
[ORG 0]

    jmp BOOT_SEG:EntryPoint
    
;------------------------------------------------------------------------------
; Data
;------------------------------------------------------------------------------
boot_drive db 0
    
    
EntryPoint:
;------------------------------------------------------------------------------
; The main procedure of the boot process
;------------------------------------------------------------------------------
    mov ax, cs                 ; The BIOS loads the boot sector at 0000:7C00h, so set DS accordinly
    mov ds, ax                 ; Therefore, we don't have to add 7C00h to all our data

    mov [boot_drive], dl       ; Save the BIOS number of the bootable device
    
    mov ax, 0x9c0              ; Set the stack 2000h bytes after the boot sector
    mov ss, ax
    mov sp, 0


reset_disk:                      
    mov ax, DISK_RESET         ; Reset the floppy drive
    mov dl, [boot_drive]       ; Set the drive
    INT_DISK
    jc reset_disk	       ; loop until success


read_sector:
    mov ax, cs                 ; write into 07C0:new_sector
    mov es, ax                 ;
    mov bx, new_sector         ;

    mov ah, DISK_READ_SECTORS  ; Read sector from drive
    mov al, 1                  ; Load 1 sector
    mov ch, 0                  ; Cylinder=0
    mov cl, 2                  ; Sector=2
    mov dh, 0                  ; Head=0
    mov dl, [boot_drive]       ; Bootable drive
    INT_DISK
    jc read_sector             ; loop until success


    jmp new_sector             ; Jump to the program


;------------------------------------------------------------------------------
; A valid boot sector must have the signature 0xAA55 at offset 510
;------------------------------------------------------------------------------
    times 510-($-$$) db 0
    dw 0xAA55



;------------------------------------------------------------------------------
; Starting from here, the code is not loaded by the BIOS.
;------------------------------------------------------------------------------
new_sector:
    mov si,boot_message     ; Display the startup message
    call PrintMessage

NeverendingLoop:
;------------------------------------------------------------------------------
; This is a loop which never ends and which does not consume CPU power.
;------------------------------------------------------------------------------
    hlt
    jmp NeverendingLoop

;------------------------------------------------------------------------------
; Data
;------------------------------------------------------------------------------
boot_message    db 'Codename: Genesis',13,10,' - loading GenOS...',0

PrintMessage:          
;------------------------------------------------------------------------------
; Print ds:si to screen using BIOS interrupt
;------------------------------------------------------------------------------
    mov bh, 00h                ; page number
    mov bl, 07h                ; foreground color
    mov ah, VIDEO_OUTPUT_CHAR  ; put character
PM_loop:
    lodsb                      ; load byte at ds:si into al
    or al, al                  ; test if character is 0 (end)
    jz PM_done
    INT_VIDEO                  ; call BIOS
    jmp PM_loop
PM_done:
    ret

