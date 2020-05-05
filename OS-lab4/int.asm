ASSUME  CS:CODE,    DS:DATA,    SS:ASTACK

CODE    SEGMENT

INTERRUPT    PROC    FAR
        jmp     INT_START

    INT_DATA:
        SUB_STACK             dw  128 dup(0)

        INTERRUPTIONS_INFO    db  " interruptions     "
        INT_CODE              dw  42025
        INTERRUPTIONS         dw  0

        KEEP_IP               dw  0
        KEEP_CS 	          dw  0
		KEEP_PSP           	  dw  0
        KEEP_SS               dw  0
        KEEP_SP               dw  0

    INT_START:
        mov     KEEP_SS, ss
        mov     KEEP_SP, sp
        mov     ss, SUB_STACK
        mov     sp, 0

		push	ax
		push    bx
		push    cx
		push    dx
		push    si
        push    es
        push    ds
        push    bp

    INT_SETUP:
		mov 	ax, seg INTERRUPT
		mov 	ds, ax
        mov     es, ax

    INT_SAVE_CURSOR:
        mov     ah, 03h
		mov     bh, 0h
		int     10h
        push    dx

	INT_ADD:
		mov 	si, offset INTERRUPTIONS
		mov 	ah, [si]
        mov     al, [si + 1]
		inc 	ax
		mov 	[si], ah
		mov 	[si + 1], al

	INT_PREP_TO_DEC:
        xor     dx, dx
        mov 	bx, 10
    	xor 	cx, cx

    INT_TO_DEC_CYCLE:
    	div 	bx
    	push	dx
    	xor 	dx, dx
    	inc 	cx
    	cmp 	ax, 0h
    	jnz 	INT_TO_DEC_CYCLE

        mov     ah, 2
        mov     bh, 0
        mov     dh, 23
        mov     dl, 0
        int     10h

    INT_PRINT_NUM:
    	pop 	ax
    	or 		al, 30h

        push    cx

        mov     ah, 09h
        mov 	bl, 2h
        mov     bh, 0
        mov     cx, 1
        int     10h

        mov     ah, 2
        mov     bh, 0
        add     dx, 1
        int     10h

        pop     cx

    	loop 	INT_PRINT_NUM

	INT_PRINT_STRING:
		mov     bp, offset INTERRUPTIONS_INFO
		mov     ah, 13h
		mov     al, 1h
		mov 	bl, 2h
		mov     bh, 0
		mov     cx, 19
		int     10h

    INT_LOAD_CURSOR:
        pop     dx
        mov     ah, 02h
		mov     bh, 0h
		int     10h

    INT_END:
        pop     bp
		pop     ds
		pop     es
		pop		si
		pop     dx
		pop     cx
		pop     bx
		pop		ax

        mov     ss, KEEP_SS
        mov     sp, KEEP_SP

		mov     al, 20h
		out     20h, al
        iret
INTERRUPT    ENDP
    LAST_BYTE:



LOAD        PROC
        push    ax
		push    bx
		push    cx
		push    dx
		push    es
		push    ds

        mov     ah, 35h
		mov     al, 1Ch
		int     21h
        mov     KEEP_IP, bx
		mov     KEEP_CS, es

        mov     dx, offset INTERRUPT
        mov     ax, seg INTERRUPT
		mov     ds, ax
		mov     ah, 25h
		mov     al, 1Ch
		int     21h
		pop		ds

        mov     dx, offset LAST_BYTE
        add     dx, 100h
		mov     cl, 4h
		shr     dx, cl
		inc     dx
		mov     ah, 31h
		int     21h

        pop     es
		pop     dx
		pop     cx
		pop     bx
		pop     ax

	    ret
LOAD        ENDP



UNLOAD      PROC
		push    ax
		push    bx
		push    dx
		push    ds
		push    es
		push    si

		mov     ah, 35h
		mov     al, 1Ch
		int     21h

		mov 	si, offset KEEP_IP
        sub     si, offset INTERRUPT
		mov 	dx, es:[bx + si]
        mov 	si, offset KEEP_CS
        sub     si, offset INTERRUPT
		mov 	ax, es:[bx + si]

		push 	ds
		mov     ds, ax
		mov     ah, 25h
		mov     al, 1Ch
		int     21h
		pop 	ds

        mov 	si, offset KEEP_PSP
        sub     si, offset INTERRUPT
        mov 	ax, es:[bx + si]
        mov     es, ax
        push    es
        mov 	ax, es:[2Ch]
		mov 	es, ax
		mov 	ah, 49h
		int 	21h

        pop     es
        mov 	ah, 49h
		int 	21h

        pop     si
		pop     es
		pop     ds
		pop     dx
		pop     bx
		pop     ax

		sti
	    ret
UNLOAD      ENDP



INT_CHECK       PROC
		push    ax
		push    bx
		push    si

		mov     ah, 35h
		mov     al, 1Ch
		int     21h

		mov     si, offset INT_CODE
        sub     si, offset INTERRUPT
		mov     ax, es:[bx + si]
		cmp	    ax, INT_CODE
		jne     INT_CHECK_END

		mov     INT_LOADED, 1

	INT_CHECK_END:
		pop     si
		pop     bx
		pop     ax

	    ret
INT_CHECK       ENDP



TAIL_CHECK        PROC
		cmp     byte ptr es:[82h], '/'
		jne     CL_CHECK_END
		cmp     byte ptr es:[83h], 'u'
		jne     CL_CHECK_END
		cmp     byte ptr es:[84h], 'n'
		jne     CL_CHECK_END
		mov     TAIL_UN, 1

	CL_CHECK_END:
		ret
TAIL_CHECK        ENDP



PRINT_STRING    PROC    NEAR
        push    ax
        mov     ah, 09h
        int     21h
        pop     ax

        ret
PRINT_STRING    ENDP



MAIN PROC
		push    ds
		xor     ax, ax
		push    ax
		mov     ax, DATA
		mov     ds, ax

        mov     KEEP_PSP, es

        call    TAIL_CHECK
		call    INT_CHECK

		cmp     TAIL_UN, 1
		je      INT_UNLOAD
		cmp     INT_LOADED, 1
		jne     INT_LOAD

		mov     dx, offset IS_LOADED_INFO
		call    PRINT_STRING
		jmp     MAIN_END

	INT_LOAD:
        mov     dx, offset LOADED_INFO
        call    PRINT_STRING
		call    LOAD
		jmp     MAIN_END

	INT_UNLOAD:
		cmp     INT_LOADED, 1
		jne     NOT_EXIST

		call    UNLOAD
        mov     dx, offset NOT_LOADED_INFO
		call    PRINT_STRING
		jmp     MAIN_END

	NOT_EXIST:
		mov     dx, offset IS_NOT_LOADED_INFO
		call    PRINT_STRING

	MAIN_END:
		xor 	al, al
		mov 	ah, 4Ch

		int 	21h
	MAIN ENDP

CODE    ENDS

ASTACK  SEGMENT STACK
    dw  128 dup(0)
ASTACK  ENDS

DATA    SEGMENT
    LOADED_INFO         db  "Interruption was loaded", 10, 13, "$"
	IS_LOADED_INFO      db  "Interruption is loaded", 10, 13, "$"
    NOT_LOADED_INFO		db  "Interruption was unloaded$"
	IS_NOT_LOADED_INFO	db  "Interruption is not loaded$"
    INT_LOADED          db  0
    TAIL_UN               db  0
DATA    ENDS

END 	MAIN
