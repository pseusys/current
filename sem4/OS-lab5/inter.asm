ASSUME  CS:CODE,    DS:DATA,    ss:ASTACK

CODE    SEGMENT

INTERRUPT    PROC    FAR
        jmp     INT_START
    INT_DATA:
        SUB_STACK             dw  128 dup(0)

        INT_CODE    dw  42025

        KEEP_IP 	dw  0
        KEEP_CS 	dw  0
        KEEP_SS		dw  0
        KEEP_SP 	dw  0
        KEEP_PSP 	dw	0

        SYMB 		db 	0
        TEMP        dw  0

    INT_START:
        mov     KEEP_SS, ss
        mov     KEEP_SP, sp
        mov     TEMP, seg INTERRUPT
        mov     ss, TEMP
        mov     sp, offset SUB_STACK
        add     sp, 256

        push	ax
        push    bx
        push    cx
        push    dx
        push    si
        push    es
        push    ds

        mov 	ax, SEG INTERRUPT
        mov 	ds, ax

        in 		al, 60h
        cmp 	al, 2Eh ; replace 'C'
        je 		INT_PASS_S
        cmp 	al, 18h ; replace 'O'
        je 		INT_PASS_A
        cmp 	al, 20h ; replace 'D'
        je 		INT_PASS_T

        pushf
        call 	DWORD PTR KEEP_IP
        jmp 	INT_END

    INT_PASS_S:
        mov		SYMB, 'S'
        jmp		INT_PASS
    INT_PASS_A:
        mov		SYMB, 'A'
        jmp		INT_PASS
    INT_PASS_T:
        mov		SYMB, 'T'
        jmp		INT_PASS

    INT_PASS:
        in 		al, 61h
        mov 	ah, al
        or 		al, 80h
        out 	61h, al
        xchg	al, al
        out 	61h, al
        mov 	al, 20h
        out 	20h, al

    INT_PRINT:
        mov 	ah, 05h
        mov 	cl, SYMB
        mov 	ch, 00h
        int 	16h
        or 		al, al
        jz 		INT_END

        mov 	ax, 0040h
        mov 	es, ax
        mov 	ax, es:[1Ah]
        mov 	es:[1Ch], ax
        jmp 	INT_PRINT

    INT_END:
        pop     ds
        pop     es
        pop		si
        pop     dx
        pop     cx
        pop     bx
        pop     ax

        mov     ss, KEEP_SS
        mov     sp, KEEP_SP

        mov 	al, 20h
        out 	20h, al
        IRET
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
		mov     al, 09h
		int     21h
        mov     KEEP_IP, bx
		mov     KEEP_CS, es

        mov     dx, offset INTERRUPT
        mov     ax, seg INTERRUPT
		mov     ds, ax
		mov     ah, 25h
		mov     al, 09h
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
		mov     al, 09h
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
		mov     al, 09h
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
		mov     al, 09h
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
    LOADED_INFO         db  "Interruption was loaded$"
	IS_LOADED_INFO      db  "Interruption is loaded$"
    NOT_LOADED_INFO		db  "Interruption was unloaded$"
	IS_NOT_LOADED_INFO	db  "Interruption is not loaded$"
    INT_LOADED          db  0
    TAIL_UN               db  0
DATA    ENDS

END 	MAIN
