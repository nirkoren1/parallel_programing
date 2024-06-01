.intel_syntax noprefix

.section .text
    .global hamming_dist
hamming_dist:
    xorps xmm3, xmm3
    xorps xmm4, xmm4
    xorps xmm0, xmm0
    mov rax, 0
    .compare_loop:
        # Load 16 bytes from each string
        movdqu xmm1, [rsi]
        movdqu xmm2, [rdi]

        paddb xmm4, xmm0

        add rsi, 16
        add rdi, 16

        add rax, 1

        # Compare strings using PCMPESTRM
        pcmpistrm xmm1, xmm2, 0b01001000  

        jnz .compare_loop
        js .sum_result
        jmp .compare_loop


    .sum_result:
        paddb xmm4, xmm0

        movq rdi, rax

        pmovsxbw xmm1, xmm4
        psrldq xmm4, 8
        pmovsxbw xmm2, xmm4
        phaddw xmm1, xmm2

        pxor xmm0, xmm0
        phaddw xmm1, xmm0
        phaddw xmm1, xmm0
        phaddw xmm1, xmm0


        pextrw r8d, xmm1, 0 
        movsx r8, r8w
        imul rax, 16
        addq rax, r8
        

    .end:
        # Return the result
        ret
