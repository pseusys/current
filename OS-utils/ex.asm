.model small
.stack 100h
.data
    str db 'aaaaa$'
    mesto db 16 dup (0)
.code

include macro.inc
include func.inc

processing proc far
	push ds
	xor ax, ax
	push ax
	mov ax, @data
	mov ds, ax

    ;print str
	
    mov ax, 256
    mov bx, 200
    mShowRegisterHex ax, ax
    mShowRegisterHex bx, bx
    mShowRegisterHex cx, cx
    mShowRegisterHex dx, dx

    NewLine

    call  DumpRegs

	mov ah, 4ch
	int	21h
	ret
processing endp
end processing