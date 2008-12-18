;------------------------------------------------------------------------------
; GenOS boot loader
;------------------------------------------------------------------------------
; Copyright (c) 2008, Cédric Rousseau
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


[BITS 16]                   ; the BIOS starts out in 16-bit real mode
[ORG 0]

EntryPoint:
;------------------------------------------------------------------------------
; The main procedure of the boot process
;------------------------------------------------------------------------------
    mov ax,0x7c0            ; The BIOS loads the boot sector at 0000:7C00h, so set DS accordinly
    mov ds,ax               ; Therefore, we don't have to add 7C00h to all our data

    mov ax,0x9c0            ; Set the stack 2000h bytes after the boot sector
    mov ss,ax
    mov sp,0

    mov si,boot_message     ; Display the startup message
    call PrintMessage

NeverendingLoop:
;------------------------------------------------------------------------------
; This is a loop which never ends and which does not consume CPU power.
;------------------------------------------------------------------------------
    hlt
    jmp NeverendingLoop


PrintMessage:          
;------------------------------------------------------------------------------
; Print ds:si to screen using BIOS interrupt
;------------------------------------------------------------------------------
    mov bx,0007             ; attribute
    mov ah,0eh              ; put character
PM_loop:
    lodsb                   ; load byte at ds:si into al
    or al,al                ; test if character is 0 (end)
    jz PM_done
    int 0x10                ; call BIOS
    jmp PM_loop
PM_done:
    ret

;------------------------------------------------------------------------------
; Data
;------------------------------------------------------------------------------
boot_message    db 'Welcome to the birth of GenOS...',13,10,0


;------------------------------------------------------------------------------
; A valid boot sector must have the signature 0xAA55 at offset 510
;------------------------------------------------------------------------------
    times 510-($-$$) db 0
    dw 0xAA55
