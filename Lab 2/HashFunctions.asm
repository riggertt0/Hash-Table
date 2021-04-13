section .text  

global RolHash  
global RorHash  
global CRC32Hash
extern main
extern strlen

CRC32Hash:

    push rbp 
    mov  rbp, rsp

    push rdi
    call strlen
    pop rdi
    mov rcx, rax
    xor rax, rax

    crc_while_len4:

        cmp rcx, 4
        jb crc_while_len0
        crc32 eax, dword [rdi]
        add rdi, 4
        sub rcx, 4
        jmp crc_while_len4

    crc_while_len0:

        cmp rcx, 0
        je crc_end
        crc32 eax, byte [rdi]
        inc rdi
        dec rcx
        jmp crc_while_len0

    crc_end:
    
    pop rbp
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
