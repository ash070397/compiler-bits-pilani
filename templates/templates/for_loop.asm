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

   msg1 db "The value of the first number is:  ", 0xA,0xD 
   len1 equ $- msg1 
   

_start: 

mov cl, 2

l1:
cmp cl, 4
jge exit

push ecx    
push eax
push ebx
push edx
mov cl, 2
l2:
    cmp cl, 6
    jge l2end
    call printer
    inc cl
    jmp l2

    
l2end:
pop edx
pop ebx
pop eax
pop ecx
inc cl
jmp l1

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



