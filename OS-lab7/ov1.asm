ASSUME CS:OVL1, DS:OVL1, SS:NOTHING, ES:NOTHING

OVL1 SEGMENT
MAIN1 PROC FAR
		push 	ds
		push 	dx
		push 	di
		push 	ax
		push 	bx

		mov 	ax, cs
		mov 	ds, ax
		mov 	bx, offset MESSAGE
		add 	bx, 48
		mov 	di, bx
		mov 	ax, cs
		call 	WRD_TO_HEX
		mov 	dx, offset MESSAGE
		call 	PRINT

		pop 	bx
		pop 	ax
		pop 	di
		pop 	dx
		pop 	ds

		retf
MAIN1 ENDP

PRINT PROC NEAR
	push 	ax
	mov 	ah, 09h
	int 	21h
	pop 	ax
	ret
PRINT ENDP

TETR_TO_HEX		PROC near
		and		al, 0Fh
		cmp		al, 09
		jbe		NEXT
		add		al, 07
		
	NEXT:
		add	al, 30h
		ret
TETR_TO_HEX		ENDP

BYTE_TO_HEX		PROC near
		push	cx
		mov		ah, al
		call	TETR_TO_HEX
		xchg	al, ah
		mov		cl, 4
		shr		al, cl
		call	TETR_TO_HEX
		pop		cx
		ret
BYTE_TO_HEX		ENDP

WRD_TO_HEX		PROC	near
		push	bx
		mov		bh, ah
		call	BYTE_TO_HEX
		mov		[di], ah
		dec		di
		mov		[di], al
		dec		di
		mov		al, bh
		xor		ah, ah
		call	BYTE_TO_HEX
		mov		[di], ah
		dec		di
		mov		[di], al
		pop		bx
		ret
WRD_TO_HEX		ENDP

MESSAGE  DB 'Hello from overlay module 1!', 13, 10, 'My address is:     $'

OVL1 ENDS
END
