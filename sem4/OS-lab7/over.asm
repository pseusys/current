ASSUME CS:CODE, DS:DATA, SS:STACKK

DATA SEGMENT
	MODULE_PATH 			db 100 dup(0)

	MEM_FREE_SUCCESS 		db "Memory freed successfully", 13, 10, "$"
	MEM_FREE_ERROR	 		db "Error freeing memory$", 13, 10, "$"

	DELIM_START    			db "<----- Overlay output begins ----->", 13, 10, "$"
    DELIM_END       		db 13, 10, "<----- Overlay output ends ----->", 13, 10, "$"

	LOADED_PATH 			db "Path loaded: $"
	END_L					db 13, 10, "$"

	LOADING_ERROR 			db "ERROR: Size of the ovl wasn't get: $"
	LOADING_NO_FILE 		db "File isn't found.", 13, 10, "$"
	LOADING_SEARCHED 		db "Path searched: $"

	EXEC_MEMORY 			db "Not enought memory!", 13, 10, "$"
	EXEC_LOADING 			db "Overlay isn't loaded!", 13, 10, "$"

	PARAMS 					dw 0
	ENTRY_ADRESS 			dd 0

	OVERLAY_1 				db "ov1.ovl", 0
	OVERLAY_2 				db "ov2.ovl", 0
	OVERLAY_3 				db "ov3.ovl", 0
DATA ENDS

STACKK SEGMENT STACK
	dw 100h 	dup (0)
STACKK ENDS


CODE SEGMENT
PRINT_STRING PROC	near
		push 	ax
		mov 	ah, 09h
		int		21h
		pop 	ax

		ret
PRINT_STRING ENDP

MEMORY_FREE PROC near
		push    ax
		push 	bx
		push 	cx
		push 	dx

		mov 	bx, offset PROGRAM_END
		mov 	ax, es
		sub 	bx, ax
		mov 	cl, 4
		shr 	bx, cl
		inc 	bx

		xor 	ax, ax
		mov 	ah, 4Ah
		int 	21h

		jc 		M_ERR
		mov 	dx, offset MEM_FREE_SUCCESS
		call	PRINT_STRING
		jmp 	M_DEF

	M_ERR:
		mov 	dx, offset MEM_FREE_ERROR
		call	PRINT_STRING
		stc

	M_DEF:
		pop 	dx
		pop 	cx
		pop 	bx
		pop 	ax

		ret
MEMORY_FREE ENDP

STRONG_OVERLOAD PROC
		push 	ax
		push 	es
		push 	si
		push 	di
		push 	dx

		push 	ax
		mov 	es, es:[2Ch]
		mov 	si, 0

	SKIP:
		mov 	ax, es:[si]
		inc 	si
		cmp 	ax, 0
		jne 	SKIP
		add 	si, 3
		mov 	di, offset MODULE_PATH

	EMPATH:
		mov 	al, es:[si]
		mov 	BYTE PTR [di], al
		cmp 	al, 0
		je 		DEPATH
		inc 	di
		inc 	si
		jmp 	EMPATH

	DEPATH:
		mov 	al, [di]
		cmp 	al, '\'
		je 		RECALL
		mov 	BYTE PTR [di], 0
		dec 	di
		jmp 	DEPATH

	RECALL:
		pop 	si

	APPEND:
		inc 	di
		mov 	al, [si]
		cmp 	al, 0
		je 		PROCEED
		mov 	BYTE PTR [di], al
		inc 	si
		jmp 	APPEND

	PROCEED:
		mov 	BYTE PTR [di], 0
		mov 	BYTE PTR [di + 1], '$'

		mov 	dx, offset LOADED_PATH
		call 	PRINT_STRING
		mov 	dx, offset MODULE_PATH
		call 	PRINT_STRING
		mov 	dx, offset END_L
		call 	PRINT_STRING

		pop 	dx
		pop 	di
		pop 	si
		pop 	es
		pop 	ax

		ret
STRONG_OVERLOAD ENDP

OVER_EXEC PROC
		push	ax
		push	bx
		push	cx
		push	dx
		push	si
		push 	es

		mov 	ah, 4Eh
		mov 	cx, 0
		mov 	dx, offset MODULE_PATH
		int 	21h

		jnc 	NO_ERROR
		mov 	dx, offset LOADING_ERROR
		call 	PRINT_STRING
		cmp 	ax, 2
		je 		NO_FILE
		cmp 	ax, 3
		je 		NO_FILE
		cmp 	ax, 12h
		je 		NO_FILE
		jmp 	EXEC_DEF

	NO_FILE:
		mov 	dx, offset LOADING_NO_FILE
		call 	PRINT_STRING

		mov 	dx, offset LOADING_SEARCHED
		call 	PRINT_STRING
		mov 	dx, offset MODULE_PATH
		call 	PRINT_STRING
		mov 	dx, offset END_L
		call 	PRINT_STRING
		jmp 	EXEC_DEF

	NO_ERROR:
		mov 	si, 0080h
		add 	si, 1Ah
		mov 	bx, es:[si]
		mov 	ax, es:[si + 2]
		mov		cl, 4
		shr 	bx, cl
		mov		cl, 12
		shl 	ax, cl
		add 	bx, ax
		add 	bx, 2

		mov 	ah, 48h
		int 	21h

		jnc 	PREPARE_DATA
		mov 	dx, offset EXEC_MEMORY
		call 	PRINT_STRING
		jmp 	EXEC_DEF

	PREPARE_DATA:
		mov 	PARAMS, ax
		mov 	WORD PTR ENTRY_ADRESS + 2, ax

		mov 	dx, offset MODULE_PATH
		push 	ds
		pop 	es
		mov 	bx, offset PARAMS
		mov 	ax, 4B03h
		int 	21h

		jnc 	LOADED
		mov 	dx, offset EXEC_LOADING
		call 	PRINT_STRING
		jmp		EXEC_DEF

	LOADED:
		mov		ax, PARAMS
		mov 	es, ax

		mov 	dx, offset DELIM_START
		call 	PRINT_STRING

		call 	ENTRY_ADRESS

		mov 	dx, offset DELIM_END
		call 	PRINT_STRING

		mov 	es, ax
		mov 	ah, 49h
		int 	21h

	EXEC_DEF:
		pop 	es
		pop 	si
		pop 	dx
		pop 	cx
		pop 	bx
		pop 	ax

		ret
OVER_EXEC ENDP

MAIN PROC
	BEGIN:
		mov 	ax, DATA
		mov 	ds, ax

		call 	MEMORY_FREE
		jc 		MAIN_END
		mov 	dx, offset END_L
		call 	PRINT_STRING

		mov 	ax, offset OVERLAY_1
		call 	STRONG_OVERLOAD
		call 	OVER_EXEC
		mov 	dx, offset END_L
		call 	PRINT_STRING

		mov 	ax, offset OVERLAY_2
		call 	STRONG_OVERLOAD
		call 	OVER_EXEC
		mov 	dx, offset END_L
		call 	PRINT_STRING

		mov 	ax, offset OVERLAY_3
		call 	STRONG_OVERLOAD
		call 	OVER_EXEC

	MAIN_END:
		mov 	ah, 4Ch
		int 	21h
MAIN ENDP

	PROGRAM_END:
CODE ENDS

END BEGIN
