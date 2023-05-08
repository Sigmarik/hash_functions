default rel

global HashTable_find_value

section .text

HashTable_find_value:  ; Code below is a modified version of CLang compiler with -Ofast compilation flag output
        mov     rcx, 0xF0F0F0F0F0F0F0F1
        mov     rax, rsi
        mul     rcx
        mov     rax, qword [rdi + 8]
        shr     rdx, 4
        mov     rcx, rdx
        shl     rcx, 4
        add     rcx, rdx
        sub     rsi, rcx
        lea     rdx, [rsi + 4*rsi]
        mov     rcx, qword [rax + 8*rdx + 16]
        test    rcx, rcx
        je      .ZeroReturnValue
        mov     rax, qword [rax + 8*rdx]
        add     rax, 32
        .LoopBegin:
                vmovdqa ymm1, yword [rax]
                vptest  ymm1, ymm0
                jb      .Return
                add     rax, 64
                add     rcx, -1
                jne     .LoopBegin
        .ZeroReturnValue:
        xor     eax, eax
        .Return:
        vzeroupper
        ret
