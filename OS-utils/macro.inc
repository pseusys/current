print macro strName
    ;---------------------------------------------------
    ; Displays a string.
    ; Receives: ds:dx <- string.
    ;---------------------------------------------------
	push ax
    push dx
    mov dx, offset &strName
	mov ah, 09h
	int 21h
    pop dx
	pop ax
endm


NewLine macro
    ;---------------------------------------------------
    ; Displays an empty string.
    ; Receives: none.
    ;---------------------------------------------------
    local temp
.data
    temp db 13,10,'$'
.code
	print temp
endm



mShowRegisterHex macro regName, regValue
    local tempStr
    ;---------------------------------------------------
    ; Displays a 16-bit register name and contents.
    ; Receives: the register name, the register value.
    ;---------------------------------------------------
.data
    tempStr db '  &regName =     h','$'
.code
	push ax
	push dx
    push di
	mov	ax, regValue
    mov di, offset tempStr+10
	call word_to_hex
	print tempStr
    pop di
    pop dx
	pop	ax
endm


pushall macro
    ;---------------------------------------------------
    ; Push ax, bx, cx, dx, si, di, es, ds, sp, bp regs.
    ; Receives: none.
    ;---------------------------------------------------
	irp case, <ax,bx,cx,dx,si,di,es,ds,sp,bp>
		push &case&
	endm
endm


popall macro
    ;---------------------------------------------------
    ; Pop ax, bx, cx, dx, si, di, es, ds, sp, bp regs.
    ; Receives: none.
    ;---------------------------------------------------
	irp case, <bp,sp,ds,es,di,si,dx,cx,bx,ax>
		pop &case&
	endm
endm


ShowFlag macro flagName, shiftCount
	local flagStr, flagVal, L1
    ;-------------------------------------
    ; Display a single flag name and value
    ; Receives: the flag name, the flag value.
    ;-------------------------------------
.data
    flagStr db "  &flagName=$"
    flagVal db ?,'$'

.code
	push ax
	push dx

	mov ax, flags
	mov flagVal, '1'
	shr ax, shiftCount
	jc L1
	mov flagVal, '0'
L1:
	; display flag name and value
	print flagStr
	print flagVal
	pop  dx
	pop  ax
ENDM

DumpRegs proc
;------------------------------------------------------
; Displays EAX, EBX, ECX, EDX, ESI, EDI, EBP, ESP in
; hexadecimal. Also displays the Zero, Sign, Carry, and
; Overflow flags.
; Receives: nothing.
; Returns: nothing.
;
; Warning: do not create any local variables or stack
; parameters, because they will alter the EBP register.
;------------------------------------------------------
.data
    flags dw ?
    saveIP dw ?
    saveSP dw ?
.code
	pop saveIP	; get current IP
	mov saveSP, sp	; save SP's value at entry
	push saveIP	; replace it on stack
	push ax
	pushf	; push flags

	pushf	; copy flags to a variable
	pop flags

;	NewLine
	mShowRegisterHex ax, ax
	mShowRegisterHex bx, bx
	mShowRegisterHex cx, cx
	mShowRegisterHex dx, dx
;	NewLine
;	mShowRegisterHex si, si
;	mShowRegisterHex di, di

;	mShowRegisterHex bp, bp

;	mov ax, saveSP
;	mShowRegisterHex sp, ax
;	NewLine

;	mov ax, saveIP
;	mShowRegisterHex ip, ax

;	mov ax, flags
;	mShowRegisterHex fl, ax

;	ShowFlag CF, 1
;	ShowFlag SF, 8
;	ShowFlag ZF, 7
;	ShowFlag OF, 12

;	NewLine
;	NewLine

	popf
	pop ax	
	ret
DumpRegs endp