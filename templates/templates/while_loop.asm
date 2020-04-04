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

;code to evaluate condition (would be a condition)
;for now I check cl >= 0
;cl initlaized with the value of the construct
mov cl, 2
check1:
cmp cl, 0
jle exit

;saving value
push ecx    
push eax
push ebx
push edx

mov cl, 4
check2:
   cmp cl, 1
   jle l2end
   call printer
   dec cl   ;the operation in inner loop
   jmp check2

l2end:
pop edx
pop ebx
pop eax
pop ecx
dec cl  ;the operation in outer loop
jmp check1

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





