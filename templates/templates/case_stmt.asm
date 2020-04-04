SYS_EXIT  equ 1
SYS_READ  equ 3
SYS_WRITE equ 4
STDIN     equ 0
STDOUT    equ 1
section	.bss
    num resb 1

section	.text
   global _start    ;must be declared for using gcc

segment .data 

   msg1 db "I am in 1:  ", 0xA,0xD 
   len1 equ $- msg1 
   

_start: 
    mov cl, 4
    cmp cl,1
    je case1
    cmp cl, 2
    je case2
    cmp cl, 3
    je case3
    jmp def

    case1:
        call printer
        jmp exit

    case2:
        call printer
        jmp exit

    case3:
        call printer
        jmp exit

    ;This label is always generated 
    def:
        call printer
        jmp exit
        



printer:
    push ecx    
    push eax
    push ebx
    push edx

    ;print command
    mov eax, SYS_WRITE
    mov ebx, STDOUT
    mov ecx, msg1
    mov edx, len1
    int 80h

    pop edx
    pop ebx
    pop eax
    pop ecx

    ret




exit:
mov eax,1             ;system call number (sys_exit)
int 0x80              ;call kernel
