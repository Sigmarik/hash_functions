default rel

global HashTable_find_value

section .text
HashTable_find_value:
    vmovdqu ymm0, [rdx]  ; Load the search word into YMM-registers
    vmovdqu ymm1, [rdx+32]

    mov rax, rsi  ; RAX = hash
    xor rdx, rdx
    div QWORD [rdi]  ; RDX = hash % size

    mov rax, rdx  ; RDX *= 40 (= 0010 1000) (= sizeof(List))
    shl rax, 5
    shl rdx, 3
    add rdx, rax

    mov rax, QWORD [rdi + 8]  ; RAX = table->contents
    mov rcx, QWORD [rax + rdx + 16]  ; RCX = table->contents[hash % size].size = RAX[RDI].size
    mov rdx, QWORD [rax + rdx]  ; RDX = table->contents[hash % size].buffer = RAX[RDI].buffer
    
    .SearchLoopBgn:
        cmp rcx, 0  ; If reached the size of the list, terminate
        je .SearchLoopEnd

        dec rcx
        add rdx, 24  ; RDX = cell->next (literaly next cell)

        mov rax, QWORD [rdx]  ; RAX = cell->content
        
        vmovdqu ymm2, [rax]  ; Load current word into YMM-registers
        vmovdqu ymm3, [rax+32]

        vptest ymm0, ymm2  ; If the word is not equal to the search word, move to the next iteration
        jnb .SearchLoopBgn
        vptest ymm1, ymm3
        jnb .SearchLoopBgn

        ret  ; Return RAX
    .SearchLoopEnd:
    
    xor rax, rax  ; Return NULL

    ret