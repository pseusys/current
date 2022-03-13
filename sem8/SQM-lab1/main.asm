# Generated with this tool: https://gcc.godbolt.org

sqrtpi:
        .long   -2079671268
        .long   1073503224
tol:
        .long   -350469331
        .long   1058682594
terms:
        .long   12
erf:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 64
        movsd   QWORD PTR [rbp-56], xmm0
        movsd   xmm0, QWORD PTR [rbp-56]
        mulsd   xmm0, xmm0
        movsd   QWORD PTR [rbp-32], xmm0
        movsd   xmm0, QWORD PTR [rbp-56]
        movsd   QWORD PTR [rbp-8], xmm0
        movsd   xmm0, QWORD PTR [rbp-56]
        movsd   QWORD PTR [rbp-16], xmm0
        mov     DWORD PTR [rbp-20], 0
.L2:
        add     DWORD PTR [rbp-20], 1
        movsd   xmm0, QWORD PTR [rbp-8]
        movsd   QWORD PTR [rbp-40], xmm0
        movsd   xmm0, QWORD PTR [rbp-16]
        addsd   xmm0, xmm0
        movapd  xmm1, xmm0
        mulsd   xmm1, QWORD PTR [rbp-32]
        pxor    xmm0, xmm0
        cvtsi2sd        xmm0, DWORD PTR [rbp-20]
        movapd  xmm2, xmm0
        addsd   xmm2, xmm0
        movsd   xmm0, QWORD PTR .LC0[rip]
        addsd   xmm2, xmm0
        divsd   xmm1, xmm2
        movapd  xmm0, xmm1
        movsd   QWORD PTR [rbp-16], xmm0
        movsd   xmm0, QWORD PTR [rbp-16]
        addsd   xmm0, QWORD PTR [rbp-40]
        movsd   QWORD PTR [rbp-8], xmm0
        movsd   xmm0, QWORD PTR .LC1[rip]
        mulsd   xmm0, QWORD PTR [rbp-8]
        comisd  xmm0, QWORD PTR [rbp-16]
        ja      .L2
        movsd   xmm0, QWORD PTR [rbp-8]
        addsd   xmm0, xmm0
        movsd   QWORD PTR [rbp-64], xmm0
        movsd   xmm0, QWORD PTR [rbp-32]
        movq    xmm1, QWORD PTR .LC2[rip]
        movapd  xmm4, xmm0
        xorpd   xmm4, xmm1
        movq    rax, xmm4
        movq    xmm0, rax
        call    exp
        mulsd   xmm0, QWORD PTR [rbp-64]
        movsd   xmm1, QWORD PTR .LC3[rip]
        divsd   xmm0, xmm1
        movq    rax, xmm0
        movq    xmm0, rax
        leave
        ret
erfc:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 64
        movsd   QWORD PTR [rbp-56], xmm0
        movsd   xmm0, QWORD PTR [rbp-56]
        mulsd   xmm0, xmm0
        movsd   QWORD PTR [rbp-24], xmm0
        movsd   xmm0, QWORD PTR [rbp-24]
        movapd  xmm1, xmm0
        addsd   xmm1, xmm0
        movsd   xmm0, QWORD PTR .LC0[rip]
        divsd   xmm0, xmm1
        movsd   QWORD PTR [rbp-32], xmm0
        mov     eax, 12
        pxor    xmm1, xmm1
        cvtsi2sd        xmm1, eax
        movsd   xmm0, QWORD PTR .LC0[rip]
        addsd   xmm0, xmm1
        movapd  xmm1, xmm0
        mulsd   xmm1, QWORD PTR [rbp-32]
        movsd   xmm0, QWORD PTR .LC0[rip]
        addsd   xmm0, xmm1
        movsd   QWORD PTR [rbp-8], xmm0
        mov     DWORD PTR [rbp-12], 12
.L5:
        pxor    xmm0, xmm0
        cvtsi2sd        xmm0, DWORD PTR [rbp-12]
        mulsd   xmm0, QWORD PTR [rbp-32]
        movapd  xmm1, xmm0
        divsd   xmm1, QWORD PTR [rbp-8]
        movsd   xmm0, QWORD PTR .LC0[rip]
        addsd   xmm0, xmm1
        movsd   QWORD PTR [rbp-40], xmm0
        movsd   xmm0, QWORD PTR [rbp-40]
        movsd   QWORD PTR [rbp-8], xmm0
        sub     DWORD PTR [rbp-12], 1
        cmp     DWORD PTR [rbp-12], 0
        jg      .L5
        movsd   xmm0, QWORD PTR [rbp-24]
        movq    xmm1, QWORD PTR .LC2[rip]
        xorpd   xmm0, xmm1
        movq    rax, xmm0
        movq    xmm0, rax
        call    exp
        movq    rax, xmm0
        movsd   xmm0, QWORD PTR [rbp-56]
        movapd  xmm1, xmm0
        mulsd   xmm1, QWORD PTR [rbp-40]
        movsd   xmm0, QWORD PTR .LC3[rip]
        mulsd   xmm0, xmm1
        movq    xmm1, rax
        divsd   xmm1, xmm0
        movq    rax, xmm1
        movq    xmm0, rax
        leave
        ret
.LC4:
        .string "Arg? "
.LC5:
        .string "%lf"
.LC8:
        .string "X = %.8lf; Erf = %.12lf; Erfc = %.12lf\n"
main:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 32
        mov     DWORD PTR [rbp-20], 1
.L16:
        mov     edi, OFFSET FLAT:.LC4
        mov     eax, 0
        call    printf
        lea     rax, [rbp-32]
        mov     rsi, rax
        mov     edi, OFFSET FLAT:.LC5
        mov     eax, 0
        call    __isoc99_scanf
        movsd   xmm1, QWORD PTR [rbp-32]
        pxor    xmm0, xmm0
        comisd  xmm0, xmm1
        jbe     .L21
        mov     DWORD PTR [rbp-20], 0
        jmp     .L10
.L21:
        movsd   xmm0, QWORD PTR [rbp-32]
        pxor    xmm1, xmm1
        ucomisd xmm0, xmm1
        jp      .L11
        pxor    xmm1, xmm1
        ucomisd xmm0, xmm1
        jne     .L11
        pxor    xmm0, xmm0
        movsd   QWORD PTR [rbp-8], xmm0
        movsd   xmm0, QWORD PTR .LC0[rip]
        movsd   QWORD PTR [rbp-16], xmm0
        jmp     .L13
.L11:
        movsd   xmm1, QWORD PTR [rbp-32]
        movsd   xmm0, QWORD PTR .LC7[rip]
        comisd  xmm0, xmm1
        jbe     .L22
        mov     rax, QWORD PTR [rbp-32]
        movq    xmm0, rax
        call    erf
        movq    rax, xmm0
        mov     QWORD PTR [rbp-8], rax
        movsd   xmm0, QWORD PTR .LC0[rip]
        subsd   xmm0, QWORD PTR [rbp-8]
        movsd   QWORD PTR [rbp-16], xmm0
        jmp     .L13
.L22:
        mov     rax, QWORD PTR [rbp-32]
        movq    xmm0, rax
        call    erfc
        movq    rax, xmm0
        mov     QWORD PTR [rbp-16], rax
        movsd   xmm0, QWORD PTR .LC0[rip]
        subsd   xmm0, QWORD PTR [rbp-16]
        movsd   QWORD PTR [rbp-8], xmm0
.L13:
        mov     rax, QWORD PTR [rbp-32]
        movsd   xmm1, QWORD PTR [rbp-16]
        movsd   xmm0, QWORD PTR [rbp-8]
        movapd  xmm2, xmm1
        movapd  xmm1, xmm0
        movq    xmm0, rax
        mov     edi, OFFSET FLAT:.LC8
        mov     eax, 3
        call    printf
.L10:
        cmp     DWORD PTR [rbp-20], 0
        jne     .L16
        mov     eax, 0
        leave
        ret
.LC0:
        .long   0
        .long   1072693248
.LC1:
        .long   -350469331
        .long   1058682594
.LC2:
        .long   0
        .long   -2147483648
        .long   0
        .long   0
.LC3:
        .long   -2079671268
        .long   1073503224
.LC7:
        .long   0
        .long   1073217536 
