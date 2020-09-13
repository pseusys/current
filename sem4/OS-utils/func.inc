word_to_str proc near
	;на входе ax число 16 бит
	;si указатель на строку
	;bx разрядность результата
    push ax
	push bx
	push cx
	push dx
	push di
	push si
    cmp bx, 16
    ja end_wts
    cmp ax, 7FFFh
    jna plus
    mov byte ptr [si], '-'
    inc si
    not ax
    inc ax
    plus:
    	xor cx, cx
    	jmp manipulation
    manipulation:
    	xor dx, dx
      	div bx
      	mov di, ax
      	mov al, dl
      	cmp al, 10
      	sbb al, 69h
      	das
	  	push di
	  	lea di, mesto
	  	add di, cx
      	mov byte ptr [di], al
	  	pop di
      	mov ax, di
    inc cx
    test ax, ax
    jz endrep
    jmp manipulation
    endrep:
    	lea di, mesto
      	add di, cx
    copyrep:
      	dec di
      	mov dl, byte ptr [di]
      	mov byte ptr [si], dl
      	inc si
      	loop copyrep
    end_wts:
    pop si
	pop di
	pop dx
	pop cx
	pop bx
	pop ax
    ret
word_to_str endp

tetr_to_hex	proc near
	and	al, 0fh
	cmp al, 09
	jbe	next
	add	al, 07
	next: add al, 30h
	ret
tetr_to_hex	endp

byte_to_hex	proc near
	;байт в al переводится в два символа 16 числа в ax
	push cx
	mov	ah, al
	call tetr_to_hex
	xchg al, ah
	mov	cl, 4
	shr	al, cl
	call tetr_to_hex ;в al старшая цифра
	pop	cx 			 ;в ah младшая цифра
	ret
byte_to_hex	endp

word_to_hex	proc near
	;перевод в 16 сс 16 разрядного числа
	;в ax - число, di - адрес последнего символа
	push bx
	mov	bh, ah
	call byte_to_hex
	mov [di], ah
	dec	di
	mov [di], al
	dec	di
	mov	al, bh
	xor	ah, ah
	call byte_to_hex
	mov	[di], ah
	dec	di
	mov	[di], al
	pop	bx
	ret
word_to_hex	endp


;WriteHex PROC
;
; Writes an unsigned 32-bit hexadecimal number to
; standard output.
; Input parameters: EAX = the number to write.
; Shell interface for WriteHexB, to retain compatibility
; with the documentation in Chapter 5.
;
; Last update: 11/18/02
;------------------------------------------------------
;	push bx
;	mov  bx,4
;	call WriteHexB
;	pop  bx
;	ret
;WriteHex ENDP


;------------------------------------------------------
;WriteHexB PROC
;	LOCAL displaySize
;
; Writes an unsigned 32-bit hexadecimal number to
; standard output.
; Receives: EAX = the number to write. EBX = display size (1,2,4)
; Returns: nothing
;
; Last update: 11/18/02
;------------------------------------------------------

;DOUBLEWORD_BUFSIZE = 8

;.data
;bufferLHB db DOUBLEWORD_BUFSIZE DUP(?),0
;
;.code
;	pushall               	; save all 32-bit data registers
;	mov displaySize,bx	; save component size

; Clear unused bits from EAX to avoid a divide overflow.
; Also, verify that EBX contains either 1, 2, or 4. If any
; other value is found, default to 4.

;.IF BX == 1	; check specified display size
;	and  ax,0FFh	; byte == 1
;.ELSE
;	.IF BX == 2
;	  and  ax,0FFFFh	; word == 2
;	.ELSE
;	  mov displaySize,4	; default (doubleword) == 4
;	.ENDIF
;.ENDIF
;
;	mov   di,displaySize	; let EDI point to the end of the buffer:
;	shl   di,1	; multiply by 2 (2 digits per byte)
;	mov   bufferLHB[di],0 	; store null string terminator
;	dec   di	; back up one position

;	mov   cx,0           	; digit counter
;	mov   bx,16	; hexadecimal base (divisor)

;L1:
;	mov   dx,0          	; clear upper dividend
;	div   bx            	; divide EAX by the base

;	xchg  ax,dx        	; swap quotient, remainder
;	call  AsciiDigit     	; convert AL to ASCII
;	mov   bufferLHB[di],al       ; save the digit
;	dec   di             	; back up in buffer
;	xchg  ax,dx        	; swap quotient, remainder

;	inc   cx             	; increment digit count
;	or    ax,ax        	; quotient = 0?
;	jnz   L1           	; no, divide again

	 ; Insert leading zeros

;	mov   ax,displaySize	; set EAX to the
;	shl   ax,1	; number of digits to print
;	sub   ax,cx	; subtract the actual digit count
;	jz    L3           	; display now if no leading zeros required
;	mov   cx,ax         	; CX = number of leading zeros to insert

;L2:
;	mov   bufferLHB[di],'0'	; insert a zero
;	dec   di                  	; back up
;	loop  L2                	; continue the loop

	; Display the digits. ECX contains the number of
	; digits to display, and EDX points to the first digit.
;L3:
;	mov   cx,displaySize	; output format size
;	shl   cx,1         	; multiply by 2
;	inc   di
;	mov   dx,OFFSET bufferLHB
;	add   dx,di
;	call  WriteString

;	popall	; restore 32-bit registers
;	ret
;WriteHexB ENDP