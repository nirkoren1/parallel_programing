.intel_syntax noprefix

.section .text
    .global strcmps
    .global hamming_dist
hamming_dist:
    .compare_loop:
        # Load 16 bytes from each string
        movdqu xmm0, [rsi]
        movdqu xmm1, [rdi]

        # Compare strings using PCMPESTRI
        pcmpestri xmm0, xmm1, 0b00011000

        # Get the result
        mov eax, ecx
        setz al

        # Check if strings are equal
        test eax, eax
        jz .equal

        # Get the first mismatching characters
        movzbq rax, byte [rsi + rcx]
        movzbq rdx, byte [rdi + rcx]

        # Calculate the difference
        sub eax, edx
        jmp .done

    .equal:
        cmp al, 1
        # Move to the next 16 bytes
        add rsi, 16
        add rdi, 16

        # Loop back for further comparison
        jmp .compare_loop

    .done:
        # Return the result
        ret
