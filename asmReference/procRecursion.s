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
    mov qword [r13], 3 ; store arguments                         \ handled by params push
    call fac
    add rsp, 8         ; delete arguments from stack
    popq rax           ; retrive return value from stack
    popq r13            ; return pointers to previous state
    popq r12            ; |

    mov [result], rax
    mov rax, 1
    mov rdi, 1
    lea rsi, [result]
    mov rdx, 9
    syscall


    mov rax, 60        ; exit syscall
    mov rdi, 0         ; exit status
    syscall

fac:
    mov rax, [r13]
    cmp rax, 1
    jg continue
    mov qword [r12], 1
    ret

continue:
    mov rdi, rax
    sub rdi, 1
    
    pushq r12           ; push pointers
    pushq r13           ; |
    sub rsp, 8         ; resrve space for return value on stack
    mov r12, rsp        ; last byte of return value
    sub rsp, 8         ; reserve space for arguments on stack
    mov r13, rsp        ; last byte of arguments
    mov [r13], rdi
    call fac
    add rsp, 8
    popq rax
    popq r13
    popq r12

    mov rdi, [r13]

    imul rax, rdi
    mov [r12], rax
    ret


segment readable writable
result rq 1
linefeed db 10