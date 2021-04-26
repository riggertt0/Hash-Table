section .text  

global RolHash  
global RorHash  
global CRC32Hash
extern main
extern strlen

CRC32Hash:

    xor rax, rax
    
    crc32 rax, qword [rdi]
    crc32 rax, qword [rdi + 8]
    crc32 rax, qword [rdi + 16]
    crc32 rax, qword [rdi + 24]

    ret

RolHash:

    push rbp 
    mov  rbp, rsp
    xor rax, rax
    xor rcx, rcx

    rol_while_len0:

        mov cl, byte [rdi]
        cmp cl, 0
        je rol_end
        rol rax, 1
        add rax, rcx
        inc rdi
        jmp rol_while_len0

    rol_end:

    pop rbp
    ret

RorHash:

    push rbp 
    mov  rbp, rsp
    xor rax, rax
    xor rcx, rcx

    ror_while_len0:

        mov cl, byte [rdi]
        cmp cl, 0
        je ror_end
        ror rax, 1
        add rax, rcx
        inc rdi
        jmp ror_while_len0

    ror_end:

    pop rbp
    ret
