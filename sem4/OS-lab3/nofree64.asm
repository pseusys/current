LAB segment
	ASSUME cs: LAB, ds: LAB, es: NOTHING, ss: NOTHING
	org 100h

START: jmp BEGIN

AVL_MEMORY_INFO     db  "Available memory: $"
EXT_MEMORY_INFO     db  "Extended memory: $"
MCB_INFO            db  "MCBs:", 0Dh, 0Ah, "$"

MCB_NUM_INFO 		db  "MCB number $"
AREA_SIZE_INFO 		db  ", size = $"
OCCUPANT_INFO       db  "; occupied by: $"

END_LINE 			db  0Dh, 0Ah, "$"
BYTES               db  " bytes$"
KBYTES              db  " kbytes", 0Dh, 0Ah, "$"

OWNER_INFO 			db  0Dh, 0Ah, "Block is $"
OWNER_UNKNOWN       db  "owned by PSP = $"
OWNER_FREE 			db  "free$"
OWNER_XMS 			db  "occupied by OS XMS UMB$"
OWNER_TM 			db  "occupied by driver's top memory$"
OWNER_DOS 			db  "occupied by MS DOS$"
OWNER_386CB 		db  "busy by 386MAX UMB$"
OWNER_386B 			db  "blocked by 386MAX$"
OWNER_386 			db  "occupied by 386MAX UMB$"

OCCUPANT_UNKNOWN 	db  "no info$"

FREEING_ERROR		db  "Some error occures during memory freeing.", 0Dh, 0Ah, "$"
ALLOCATING_ERROR	db  "Some error occures during memory allocating.", 0Dh, 0Ah, "$"

;-------------------------------------------------------------------------------

PRINT_STRING PROC NEAR ; (DX : String)
	push 	ax

	mov 	ah, 09h
	int		21h

	pop 	ax
	ret
PRINT_STRING ENDP

DEC_WORD_PRINT PROC NEAR ; (AX : short)
	push 	ax
	push 	cx
	push	dx
	push	bx

	mov 	bx, 10
	xor 	cx, cx

NUM:
	div 	bx
	push	dx
	xor 	dx, dx
	inc 	cx
	cmp 	ax, 0h
	jnz 	NUM

PRINT_NUM:
	pop 	dx
	or 		dl, 30h
	mov 	ah, 02h
	int 	21h
	loop 	PRINT_NUM

	pop 	bx
	pop 	dx
	pop 	cx
	pop 	ax
	ret
DEC_WORD_PRINT ENDP

HEX_BYTE_PRINT PROC NEAR ; (AL : byte)
	push 	ax
	push 	bx
	push 	dx

	mov 	ah, 0
	mov 	bl, 10h
	div 	bl
	mov 	dx, ax
	mov 	ah, 02h
	cmp 	dl, 0Ah
	jl 		PRINT
	add 	dl, 07h

PRINT:
	add 	dl, '0'
	int 	21h;

	mov 	dl, dh
	cmp 	dl, 0Ah
	jl 		PRINT_EXT
	add 	dl, 07h

PRINT_EXT:
	add 	dl, '0'
	int 	21h;

	pop 	dx
	pop 	bx
	pop 	ax
	ret
HEX_BYTE_PRINT ENDP

HEX_WORD_PRINT PROC	NEAR ; (AX : short)
	push 	ax
	push 	ax
	mov 	al, ah
	call 	HEX_BYTE_PRINT
	pop 	ax
	call 	HEX_BYTE_PRINT
	pop 	ax
	ret
HEX_WORD_PRINT ENDP

;-------------------------------------------------------------------------------

FREE_MEM PROC NEAR
	push 	ax
	push 	bx
	push 	cx
	push 	dx

	mov 	bx, offset PROGRAMM_END
	mov 	cl, 4
	shr 	bx, cl
	add 	bx, 1
	mov 	ah, 4Ah
	int 	21h
	jnc		FREE_SUCCESS

FREE_ERROR:
	mov dx, offset FREEING_ERROR
	call PRINT_STRING

FREE_SUCCESS:
	pop 	dx
	pop		cx
	pop 	bx
	pop 	ax
	ret
FREE_MEM ENDP

ALLOC_MEM PROC NEAR
    push 	ax
    push 	bx
    push 	cx
    push 	dx

    mov     bx, 1000h
    mov     ah, 48h
    int     21h
    jnc		FREE_SUCCESS

ALLOC_ERROR:
    mov    dx, offset ALLOCATING_ERROR
    call   PRINT_STRING

ALLOC_SUCCESS:
    pop 	dx
    pop		cx
    pop 	bx
    pop 	ax
    ret
ALLOC_MEM ENDP

PRINT_AVL_MEMORY PROC NEAR
	push    ax
	push    bx
	push    dx

	mov     dx, offset AVL_MEMORY_INFO
	call    PRINT_STRING

	xor     ax, ax
	int 	12h
	xor     dx, dx
	call    DEC_WORD_PRINT

    mov     dx, offset KBYTES
    call    PRINT_STRING

	pop     dx
	pop     bx
	pop     ax
	ret
PRINT_AVL_MEMORY ENDP

PRINT_EXT_MEMORY PROC NEAR
	push 	ax
	push 	bx
	push 	dx

    mov 	dx, offset EXT_MEMORY_INFO
	call 	PRINT_STRING

    mov 	al, 30h
	out 	70h, al
	in 		al, 71h
	mov 	bl, al
	mov 	al, 31h
	out 	70h, al
	in 		al, 71h
	mov 	ah, al
	mov 	al, bl
	xor     dx, dx
	call 	DEC_WORD_PRINT

    mov     dx, offset KBYTES
    call    PRINT_STRING

	pop 	dx
	pop 	bx
	pop 	ax
	ret
PRINT_EXT_MEMORY ENDP

PRINT_MCB PROC near
	push 	ax
	push 	bx
	push 	cx
	push 	dx
	push 	es
	push 	si

    mov     dx, offset MCB_INFO
	call    PRINT_STRING

	mov 	ah, 52h
	int 	21h
	mov 	ax, es:[bx-2]
	mov 	es, ax
	xor 	cx, cx
    push    cx

NEXT_MCB:
    pop     cx
    mov 	dx, offset MCB_NUM_INFO
    call 	PRINT_STRING
    inc 	cx
	mov 	ax, cx
	xor     dx, dx
	call	DEC_WORD_PRINT
    push 	cx

OWNER_START:
	mov 	dx, offset OWNER_INFO
	call 	PRINT_STRING
	mov 	ax, es:[1h]

	cmp 	ax, 0h
	je 		PRINT_FREE
	cmp 	ax, 6h
	je 		PRINT_XMS
	cmp 	ax, 7h
	je 		PRINT_TM
	cmp 	ax, 8h
	je 		PRINT_DOS
	cmp 	ax, 0FFFAh
	je 		PRINT_386CB
	cmp 	ax, 0FFFDh
	je 		PRINT_386B
	cmp 	ax, 0FFFEh
	je 		PRINT_386

    mov 	dx, offset OWNER_UNKNOWN
    call 	PRINT_STRING
	call 	HEX_WORD_PRINT
	jmp 	AREA_SIZE

PRINT_FREE:
	mov 	dx, offset OWNER_FREE
    call 	PRINT_STRING
	jmp 	AREA_SIZE
PRINT_XMS:
	mov 	dx, offset OWNER_XMS
    call 	PRINT_STRING
	jmp 	AREA_SIZE
PRINT_TM:
	mov 	dx, offset OWNER_TM
    call 	PRINT_STRING
	jmp 	AREA_SIZE
PRINT_DOS:
	mov 	dx, offset OWNER_DOS
    call 	PRINT_STRING
	jmp 	AREA_SIZE
PRINT_386CB:
	mov 	dx, offset OWNER_386CB
    call 	PRINT_STRING
	jmp 	AREA_SIZE
PRINT_386B:
	mov 	dx, offset OWNER_386B
    call 	PRINT_STRING
	jmp 	AREA_SIZE
PRINT_386:
	mov 	dx, offset OWNER_386
	call 	PRINT_STRING

AREA_SIZE:
	mov 	dx, offset AREA_SIZE_INFO
	call 	PRINT_STRING

	mov 	ax, es:[3h]
	mov 	bx, 10h
	mul 	bx
	call 	DEC_WORD_PRINT

    mov     dx, offset BYTES
    call    PRINT_STRING

    mov 	dx, offset OCCUPANT_INFO
	call 	PRINT_STRING

    mov     ax, es:[8h]
    cmp     ax, 0h
    je      NO_OCCUPANT

	mov 	cx, 8
	xor 	si, si
OCCUPANT:
	mov     dl, es:[si + 8h]
    mov     ah, 02h
	int     21h
	inc     si
	loop    OCCUPANT
    jmp     PROCEEDING

NO_OCCUPANT:
    mov 	dx, offset OCCUPANT_UNKNOWN
    call 	PRINT_STRING

PROCEEDING:
    xor 	ax, ax
	mov 	al, es:[0h]
    cmp     al, 5Ah
	je      ENDING

	mov     ax, es:[3h]
	mov     bx, es
	add     bx, ax
	inc     bx
	mov     es, bx

    mov     dx, offset END_LINE
	call    PRINT_STRING
	jmp     NEXT_MCB

ENDING:
	pop 	cx
	pop     si
	pop     es
	pop     dx
	pop     cx
	pop     bx
	pop     ax
	ret
PRINT_MCB ENDP

BEGIN:
	call 	FREE_MEM
    call    ALLOC_MEM

    call    PRINT_AVL_MEMORY
	call	PRINT_EXT_MEMORY
    call    PRINT_MCB

    xor     al, al
	mov     ah, 4Ch
    int     21h

PROGRAMM_END:

LAB ends
end START
