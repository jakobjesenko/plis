format ELF64 executable 3
segment readable executable
entry start

start:
    pushq r12           ; push pointers
    pushq r13           ; |
    sub rsp, 8         ; resrve space for return value on stack
    mov r12, rsp        ; last byte of return value
    sub rsp, 8         ; reserve space for arguments on stack
    mov r13, rsp        ; last byte of arguments
    mov qword [r13], 33 ; store arguments                         \ handled by params push
    call foo
    add rsp, 8         ; delete arguments from stack
    popq rax           ; retrive return value from stack
    popq r13            ; return pointers to previous state
    popq r12            ; |


    mov rax, 60        ; exit syscall
    mov rdi, 0         ; exit status
    syscall

foo:
    mov rax, 1         ; print syscall
    mov rdi, 1         ; file descriptor
    lea rsi, [toprint] ; buffer to print
    mov rdx, 14        ; size of buffer
    syscall


    mov rax, [r13]      ; retrive arguments from stack \ done by arg()
                        ; DO FUNCTION STUFF :)
    mov [r12], rax      ; save return value on stack   \ done by return()
    ret

segment readable writable
toprint: db "Hello, World!", 10