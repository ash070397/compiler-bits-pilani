mov [sys_fleft], __float32__(3.2)
mov [sys_fright], __float32__(4.9)
fld dword[sys_fleft]
fld dword[sys_fright]
faddp
fstp dword[sys_fleft]