ASSUME  CS:CODE,    DS:DATA,    ss:ASTACK

ASTACK SEGMENT STACK
    dw  128 dup(0)
ASTACK ENDS

DATA SEGMENT

    COMP_SUCCESS	db	"Program finished normally with code: $"
    COMP_CC		    db	"Program stopped by CRTL+C command", 13, 10, "$"
    ERR_FILENAME	db  "File not found!", 13, 10, "Path searched: $"
    ERR_MEMORY      db  "Not sufficient memory!", 13, 10, "$"

    START_STR_NAME  db "Launching file $"
    START_STR_PATH  db " in dir $"
    START_STR_END   db " ...", 13, 10, "$"

    DELIM_START     db 13, 10, "<----- Child output begins ----->", 13, 10, "$"
    DELIM_END       db "<----- Child output ends ----->", 13, 10, 13, 10, "$"

    PATH 		    db 	100 dup(0)
    FILENAME 		db 	"VERSER.COM$"
    MODULE          db  150 dup(0)

    PSP 			dw 	?
    CMD             db  12, "sample text", 0
    KEEP_SS 		dw 	?
    KEEP_SP 		dw 	?

    PARAMS 			dw 	7 dup(?)
    MEMORY_ERROR 	db 	0

DATA ENDS

CODE SEGMENT

HEX_BYTE_PRINT	PROC
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
HEX_BYTE_PRINT	ENDP

PRINT_STRING 	PROC	near
		push 	ax
		mov 	ah, 09h
		int		21h
		pop 	ax
	    ret
PRINT_STRING 	ENDP



FREE_MEM 	PROC
		mov 	bx, offset PROGEND
		mov 	ax, es
		sub 	bx, ax
		mov 	cl, 4
		shr 	bx, cl
		mov 	ah, 4Ah
		int 	21h
		jc 		MCATCH
		jmp 	MDEFAULT
	MCATCH:
		mov 	MEMORY_ERROR, 1
	MDEFAULT:
	    ret
FREE_MEM 	ENDP



FINISH 	PROC
		mov 	ah, 4Dh
		int 	21h
		cmp 	ah, 1
		je 		ECTRLC
		mov 	dx, offset COMP_SUCCESS
		call 	PRINT_STRING
		call	HEX_BYTE_PRINT
		mov 	dl, ah
		mov 	ah, 2h
		int 	21h
		jmp 	EDEFAULT
	ECTRLC:
		mov 	dx, offset COMP_CC
		call 	PRINT_STRING
	EDEFAULT:
	    ret
FINISH 	ENDP



FORM_PATHS		PROC	near
		push    ax
		push    es
		push    si
        push    di
		push    dx

		mov     ax, es:[2Ch]
        mov     es, ax

		mov     si, 0
SYMBOL:
        add     si, 1
		mov     al, es:[si]
		mov     ah, es:[si+1]
		cmp     ax, 0
		jne     SYMBOL

        add     si, 4
        mov     di, 0
SYMB:
        mov     al, es:[si]
        cmp     al, 0
        je      DIR_END

        mov     PATH[di], al

        inc     di
        add     si, 1
        jmp     SYMB

DIR_END:
        cmp     PATH[di - 1], "\"
        je      APPENSION
        mov     PATH[di - 1], "$"
        dec     di
        jmp     DIR_END

APPENSION:
        mov     PATH[di], "$"

        mov     si, 0
        mov     di, 0
ADD_PATH:
        cmp     PATH[di], "$"
        je      ADD_NAME_PREP
        mov     al, PATH[di]
        mov     MODULE[si], al
        inc     si
        inc     di
        jmp     ADD_PATH

ADD_NAME_PREP:
        mov     di, 0
ADD_NAME:
        cmp     FILENAME[di], "$"
        je      P_END
        mov     al, FILENAME[di]
        mov     MODULE[si], al
        inc     si
        inc     di
        jmp     ADD_NAME

P_END:
        mov     MODULE[si], "$"

		pop     dx
        pop     di
		pop     si
		pop     es
		pop     ax
		ret
FORM_PATHS		ENDP



MAIN PROC
		mov 	ax, DATA
		mov 	ds, ax

        ; -----> Forming path to module
        call    FORM_PATHS

        mov     dx, offset START_STR_NAME
        call    PRINT_STRING
        mov     dx, offset FILENAME
        call    PRINT_STRING
        mov     dx, offset START_STR_PATH
        call    PRINT_STRING
        mov     dx, offset PATH
        call    PRINT_STRING
        mov     dx, offset START_STR_END
        call    PRINT_STRING

        ; -----> Freeing memory
		call 	FREE_MEM
		cmp 	MEMORY_ERROR, 0
		jne 	PDEFAULT

        ; -----> Creating Environment
        mov     PARAMS[0], 0

        ; -----> Preparing cmd
        mov     PARAMS[2], offset CMD
        mov     PARAMS[4], ds

        mov     dx, offset DELIM_START
        call    PRINT_STRING

		push 	ds
		pop 	es

        mov     ax, ds
        mov     es, ax
		mov 	dx, offset MODULE
		mov 	bx, offset PARAMS

		mov 	KEEP_SS, ss
		mov 	KEEP_SP, sp

		mov 	ax, 4B00h
		int 	21h

        mov     cx, DATA
        mov     ds, cx

		mov 	ss, KEEP_SS
		mov 	sp, KEEP_SP

        mov     dx, offset DELIM_END
        call    PRINT_STRING

        ; -----> Printing results
		jc 		NOFILE
		jmp 	SUCCESS

	NOFILE:
		mov 	dx, offset ERR_FILENAME
		call 	PRINT_STRING
		mov 	dx, offset FILENAME
		call 	PRINT_STRING
		jmp 	PDEFAULT

	SUCCESS:
		call 	FINISH

	PDEFAULT:
		mov 	ah, 4Ch
		int 	21h
MAIN ENDP
PROGEND:

CODE ENDS

end MAIN
