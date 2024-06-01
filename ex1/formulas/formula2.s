.intel_syntax noprefix

.section .data
    ones: .float 1.0, 1.0, 1.0, 1.0
    zeros: .float 0.0, 0.0, 0.0, 0.0

.section .text
    .global formula2
formula2:
    xorps xmm3, xmm3
    xorps xmm4, xmm4
    xorps xmm0, xmm0
    movaps xmm5, [ones]  #the mul
    movaps xmm6, [zeros] #the sum
    movaps xmm7, [ones]  
    movq rax, rdx
    movq r8, 4
    xor rdx, rdx
    divq rax, r8
    movq r8, 0
    .compare_loop:
        # Load 16 bytes from each string
        movaps xmm1, [rsi]
        movaps xmm2, [rdi]

        movaps xmm3, xmm1
        mulps xmm3, xmm2
        addps xmm6, xmm3

        movaps xmm4, xmm1
        subps xmm4, xmm2
        mulps xmm4, xmm4
        addps xmm4, xmm7
        mulps xmm5, xmm4

        add rsi, 16
        add rdi, 16

        add r8, 1

        cmp r8, rax
        jl .compare_loop


    .sum_result:

        pxor xmm0, xmm0
        haddps xmm6, xmm0
        haddps xmm6, xmm0

        movaps xmm1, xmm5        # Copy xmm0 to xmm1
        shufps xmm1, xmm1, 0x0e # Shuffle xmm1 to {c, d, b, a}
        mulps xmm5, xmm1         # xmm0 = {a*c, b*d, b*c, a*d}
        movaps xmm1, xmm5       # Move high to low, xmm1 = {b*d, a*c}
        shufps xmm1, xmm1, 0x01
        mulss xmm5, xmm1         # xmm0 = {a*b*c*d, ..., ..., ...}

        movss xmm0, xmm5        # Move the lowest 32-bit float from xmm5 to xmm0
        movd r8d, xmm0          # Move the float as a 32-bit integer into eax
        mov r8, r8  

        movss xmm0, xmm6        # Move the lowest 32-bit float from xmm5 to xmm0
        movd eax, xmm0          # Move the float as a 32-bit integer into eax
        mov rax, rax 

        divss xmm6, xmm5
        movaps xmm0, xmm6
        #movq rax, xmm6

        #xor rdx, rdx
        #div rax, r8
        

    .end:
        # Return the result
        ret
