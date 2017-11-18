xor b b
xor e e
set 1
cpy c a
loop:
str e c
addi e 1
cpy d b
add d c
cpy b c
cpy c d
set loop
cpy ip a
