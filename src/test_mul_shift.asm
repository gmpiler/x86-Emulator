BITS 32
    org 0x7c00
    mov eax, 2
    imul eax, eax, 3
    shl eax, byte 0x1
    jmp 0

    
