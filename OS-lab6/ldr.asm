ASSUME  CS:CODE,    DS:DATA,    ss:ASTACK

ASTACK SEGMENT STACK
    dw  128 dup(0)
ASTACK ENDS

DATA SEGMENT

    COMP_SUCCESS	db	"Program finished normally with code: $"
    COMP_CTRLC		db	"Program stopped by CRTL+C command", 13, 10, "$"
    COMP_ERR        db  "Program finished with error", 13, 10, "$"
    COMP_RESID      db  "Program finished, leaving as resident", 13, 10, "$"
    COMP_DEFAULT    db  "Program finished with folowing reason: $"

    ERR_FUNCTION	db  "Functiom number wrong!$"
    ERR_FILENAME	db  "File not found!", 13, 10, "Path searched: $"
    ERR_DISK    	db  "Disk error occures!", 13, 10, "$"
    ERR_MEMORY      db  "Not sufficient memory!", 13, 10, "$"
    ERR_ENV_STR     db  "Environment string wrong!", 13, 10, "$"
    ERR_FORMAT      db  "Wrongly formatted!", 13, 10, "$"
    ERR_DEFAULT     db  "Program crashed with folowing code: $"

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
    SS_BACK 		dw 	?
    SP_BACK 		dw 	?

    ENDOFLINE 		db  13, 10, "$"
    PARAM 			dw 	7 dup(?)
    MEMORY_ERROR 	db 	0

DATA ENDS

CODE SEGMENT

HEX_BYTE_PRINT	PROC
		push 	AX
		push 	BX
		push 	DX

		mov 	AH, 0
		mov 	BL, 10h
		div 	BL
		mov 	DX, AX
		mov 	AH, 02h
		cmp 	DL, 0Ah
		jl 		PRINT
		add 	DL, 07h
	PRINT:
		add 	DL, '0'
		int 	21h;

		mov 	DL, DH
		cmp 	DL, 0Ah
		jl 		PRINT_EXT
		add 	DL, 07h
	PRINT_EXT:
		add 	DL, '0'
		int 	21h;

		pop 	DX
		pop 	BX
		pop 	AX
	ret
HEX_BYTE_PRINT	ENDP

PRINT_STRING 	PROC	near
		push 	AX
		mov 	AH, 09h
		int		21h
		pop 	AX
	ret
PRINT_STRING 	ENDP



MEMORY_FREE 	PROC
		lea 	BX, PROGEND
		mov 	AX, ES
		sub 	BX, AX
		mov 	CL, 4
		shr 	BX, CL
        inc     bx
		mov 	AH, 4Ah
		int 	21h
		jc 		MCATCH
		jmp 	MDEFAULT
	MCATCH:
		mov 	MEMORY_ERROR, 1
	MDEFAULT:
	ret
MEMORY_FREE 	ENDP

FORM_PATHS		PROC	near
		push ax
		push es
		push si
        push di
		push dx

		mov ax, es:[2Ch]
        mov es, ax

		mov si, 0
SYMBOL:
        add si, 1
		mov al, es:[si]
		mov ah, es:[si+1]
		cmp ax, 0
		jne SYMBOL

        add si, 4
        mov di, 0
SYMB:
        mov al, es:[si]
        cmp al, 0
        je DIR_END

        mov PATH[di], al

        inc di
        add si, 1
        jmp SYMB

DIR_END:
        cmp PATH[di - 1], "\"
        je APPENSION
        mov PATH[di - 1], "$"
        dec di
        jmp DIR_END

APPENSION:
        mov PATH[di], "$"

        mov si, 0
        mov di, 0
ADD_PATH:
        cmp PATH[di], "$"
        je ADD_NAME_PREP
        mov al, PATH[di]
        mov MODULE[si], al
        inc si
        inc di
        jmp ADD_PATH

ADD_NAME_PREP:
        mov di, 0
ADD_NAME:
        cmp FILENAME[di], "$"
        je P_END
        mov al, FILENAME[di]
        mov MODULE[si], al
        inc si
        inc di
        jmp ADD_NAME

P_END:
        mov MODULE[si], "$"

		pop dx
        pop di
		pop si
		pop es
		pop ax
		ret
FORM_PATHS		ENDP



EXIT_ERROR 	PROC
		cmp 	ax, 1
		je 		EFUNC
        cmp 	ax, 2
		je 		EFILE
        cmp 	ax, 5
		je 		EDISK
        cmp 	ax, 8
		je 		EMEM
        cmp 	ax, 10
		je 		EENV
        cmp 	ax, 11
		je 		EFORMAT

		jmp 	EDEFAULT

    EFUNC:
    	lea 	DX, ERR_FUNCTION
    	call 	PRINT_STRING
        jmp 	EEND

	EFILE:
		lea 	DX, ERR_FILENAME
		call 	PRINT_STRING
        lea 	DX, MODULE
		call 	PRINT_STRING
        lea 	DX, ENDOFLINE
        call 	PRINT_STRING
        jmp 	EEND

    EDISK:
    	lea 	DX, ERR_DISK
		call 	PRINT_STRING
        jmp 	EEND

    EMEM:
		lea 	DX, ERR_MEMORY
    	call 	PRINT_STRING
        jmp 	EEND

    EENV:
        lea 	DX, ERR_ENV_STR
        call 	PRINT_STRING
        jmp 	EEND

    EFORMAT:
        lea 	DX, ERR_FORMAT
        call 	PRINT_STRING
        jmp 	EEND

	EDEFAULT:
        lea 	DX, ERR_DEFAULT
        call 	PRINT_STRING
        call	HEX_BYTE_PRINT
        lea 	DX, ENDOFLINE
        call 	PRINT_STRING

    EEND:
        ret
EXIT_ERROR 	ENDP

EXIT_SUCCESS 	PROC
		mov 	AH, 4Dh
		int 	21h

		cmp 	AH, 0
		je 		SOK
        cmp 	AH, 1
		je 		SCTRLC
        cmp 	AH, 2
		je 		SERR
        cmp 	AH, 3
		je 		SRES

        jmp     SDEFAULT

    SOK:
        lea 	DX, COMP_SUCCESS
        call 	PRINT_STRING
        call	HEX_BYTE_PRINT
        lea 	DX, ENDOFLINE
        call 	PRINT_STRING
        jmp 	SEND

    SCTRLC:
        lea 	DX, COMP_CTRLC
        call 	PRINT_STRING
        jmp 	SEND

    SERR:
        lea 	DX, COMP_ERR
        call 	PRINT_STRING
        jmp 	SEND

    SRES:
        lea 	DX, COMP_RESID
        call 	PRINT_STRING
        jmp 	SEND

    SDEFAULT:
        lea 	DX, COMP_DEFAULT
        call 	PRINT_STRING
        mov     al, ah
        call	HEX_BYTE_PRINT
        lea 	DX, ENDOFLINE
        call 	PRINT_STRING

	SEND:
	   ret
EXIT_SUCCESS 	ENDP



MAIN PROC near
		mov 	AX, DATA
		mov 	DS, AX

        ; -----> Forming path to module
        call    FORM_PATHS

        lea     dx, START_STR_NAME
        call    PRINT_STRING
        lea     dx, FILENAME
        call    PRINT_STRING
        lea     dx, START_STR_PATH
        call    PRINT_STRING
        lea     dx, PATH
        call    PRINT_STRING
        lea     dx, START_STR_END
        call    PRINT_STRING

        ; -----> Freeing memory
		call 	MEMORY_FREE
		cmp 	MEMORY_ERROR, 0
		jne 	PDEFAULT

        ; -----> Creating Environment
        mov     PARAM[0], 0

        ; -----> Preparing cmd
        mov     PARAM[2], offset CMD
        mov     PARAM[4], ds

        lea     dx, DELIM_START
        call    PRINT_STRING

        push    es
        mov 	SS_BACK, SS
		mov 	SP_BACK, SP

        mov     ax, ds
        mov     es, ax
        mov 	DX, offset MODULE
		mov 	BX, offset PARAM

		mov 	AX, 4B00h
		int 	21h

        mov 	AX, DATA
		mov 	DS, AX
		mov 	SS, SS_BACK
		mov 	SP, SP_BACK
        pop     es

        lea     dx, DELIM_END
        call    PRINT_STRING

		jc 		ERRORS
		jmp 	SUCCESS

    MEM_ERR:
        lea 	DX, ERR_MEMORY
        call 	PRINT_STRING
        jmp     PDEFAULT

	ERRORS:
		call 	EXIT_ERROR
		jmp 	PDEFAULT

	SUCCESS:
        call EXIT_SUCCESS

	PDEFAULT:
		mov 	AH, 4Ch
		int 	21h
MAIN ENDP
PROGEND:

CODE ENDS

end MAIN
