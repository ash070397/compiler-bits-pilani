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
;IMUL operates on signed and unsigned 
;MUL does unsigned
;Similarly DIV and IDIV
;Dividend is in AX and divisor is what is in instruction
;Finally quotient is in AL and rem in AH

; say I want to compute an expression of 
;    *
;   /  \
;  -    /
; / \   / \
;3  2  51   1

;al always has the result...(Accumulator) in case of add sub
;As I traverse the tree first I see - and get the 2 operand into al and bl respectively

;in case of an id left operand I will do
;mov al, [name_of_val] name_of_val should be a byte sized location
;clearing registers
xor eax, eax
xor ebx, ebx

mov al, 13
mov bl, 3
sub al, bl


push eax ;saving the left tree result

xor eax, eax
xor ebx, ebx

mov ax, 15
mov bl, 3
div bl



push eax; saving right tree result

;getting 2nd operand in bx reg (ask me why I used ebx and not bl)
pop ebx
;getting first operand in ax reg 
pop eax

mul bl

mov [num], al

mov eax, SYS_WRITE
mov ebx, STDOUT
mov ecx, num
mov edx, 1
int 80h
;result is in al

;printing the result

exit:
mov eax,1             ;system call number (sys_exit)
int 0x80              ;call kernel


