VERSER SEGMENT
		ASSUME CS:VERSER, DS:VERSER, ES:NOTHING, SS:NOTHING
		org 100H
START: 	jmp	BEGIN

UNAVAILABLE_ADDRESS			db	'Segment unavailable memory address: $'
ENVIRONMENT_ADDRESS		    db	'Segment environment address: $'
COMMAND_LINE_TAIL			db	'Command line tail: $'
ENVIRONMENT_CONTENT	 	    db	'Environment content: $'
LOADING_PATH            	db	'Loading module path: $'

TWO_SYMBOLS_CONTAINER		db	'  $'
ENVIRONMENTAL_LINE	 	    db	0dh, 0ah, '                     $'
ENDL_LINE					db	0dh, 0ah, '$'



PRINT_STRING 	PROC	near
		push 	ax
		mov 	ah, 09h
		int		21h
		pop 	ax
		ret
PRINT_STRING 	ENDP



;--------------------------------------------------------------------------------



TETR_TO_HEX		PROC	near
		and		al, 0fh
		cmp		al, 09
		jbe		NEXT
		add		al, 07
NEXT:	add		al, 30h
		ret
TETR_TO_HEX		ENDP

;--------------------------------------------------------------------------------

BYTE_IN_HEX		PROC 	near
		push	cx
		push 	si
		mov		al, ah
		call	TETR_TO_HEX
		xchg	al, ah
		mov		cl, 4
		shr		al, cl
		call	TETR_TO_HEX
		lea 	si, TWO_SYMBOLS_CONTAINER
		mov 	[si], al
		mov 	[si+1], ah
		mov 	dx, offset TWO_SYMBOLS_CONTAINER
		call 	PRINT_STRING
		pop 	si
		pop		cx
		ret
BYTE_IN_HEX		ENDP

;--------------------------------------------------------------------------------

PRINT_REG		PROC 	near
		push	cx
		push 	dx
		mov 	cx, ax
		call	BYTE_IN_HEX
		mov 	al, ch
		call	BYTE_IN_HEX
		mov 	dx, offset ENDL_LINE
		call 	PRINT_STRING
		pop		dx
		pop		cx
		ret
PRINT_REG		ENDP



;--------------------------------------------------------------------------------



PRINT_TAIL		PROC	near
		push ax
		push cx
		push si
		mov cx, 0
		mov cl, ds:[80h]
		cmp CL, 0
		je PT_END
		mov si, 0
		mov ax, 0
CYCLE:	mov al, ds:[81h+si]
		call PRINT_BYTE
		add si, 1
		loop CYCLE
PT_END:	mov dx, offset ENDL_LINE
		call PRINT_STRING
		pop si
		pop cx
		pop ax
		ret
PRINT_TAIL		ENDP

;--------------------------------------------------------------------------------

PRINT_BYTE		PROC	near
		push ax
		push dx
		mov dx, 0h
		mov dl, al
		mov ah, 02h
		int 21h
		pop dx
		pop ax
		ret
PRINT_BYTE		ENDP

;--------------------------------------------------------------------------------

PRINT_ENV		PROC	near
		push ax
		push es
		push si
		push dx
		push bx
		mov bx, 2Ch
		mov es, [bx]
		pop bx
		mov si, 0
		mov al, es:[si]
SYMBOL:	cmp al, 0
		jne NOLINE
		mov dx, offset ENVIRONMENTAL_LINE
		call PRINT_STRING
		jmp LOOPER
NOLINE:	call PRINT_BYTE
LOOPER:	add si, 1
		mov al, es:[si]
		mov ah, es:[si+1]
		cmp ax, 0
		jne SYMBOL
		mov dx, offset ENDL_LINE
		call PRINT_STRING
		call PRINT_PATH
		pop dx
		pop si
		pop es
		pop ax
		ret
PRINT_ENV		ENDP

;--------------------------------------------------------------------------------

PRINT_PATH		PROC	near
		mov dx, offset LOADING_PATH
		call PRINT_STRING
		add si, 4
SYMB:	mov al, es:[si]
		cmp al, 0
		je P_END
		call PRINT_BYTE
		add si, 1
		jmp SYMB
P_END:	ret
PRINT_PATH		ENDP



;--------------------------------------------------------------------------------



BEGIN:
		push    dx
		push 	ax

		mov 	dx, offset UNAVAILABLE_ADDRESS
		call 	PRINT_STRING
		mov 	ax, ds:[02h]
		call	PRINT_REG

		mov 	dx, offset ENVIRONMENT_ADDRESS
		call 	PRINT_STRING
		mov 	ax, ds:[2Ch]
		call	PRINT_REG

		mov 	dx, offset COMMAND_LINE_TAIL
		call 	PRINT_STRING
		call	PRINT_TAIL

		mov 	dx, offset ENVIRONMENT_CONTENT
		call 	PRINT_STRING
		call	PRINT_ENV

ENDING:
		pop		ax
		pop 	dx

		xor		al, al
		mov 	ah, 4ch
		int		21h
		ret
VERSER 	ENDS
		END  	START
