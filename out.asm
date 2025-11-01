global main
main:
    push rbp
    mov rbp, rsp
    mov rax, 3
    push rax
    mov rax, 5
    push rax
    pop rbx
    pop rax
    add rax, rbx
    push rax
    pop rax
    leave
    ret
    
 section .note.GNU-stack noalloc noexec nowrite progbits   
