xor b b
xor e e
set 1
cpy c a
loop:
str e c
addi e 1
set 13
sub a e
je done
cpy d b
add d c
cpy b c
cpy c d
set loop
cpy ip a
done:
set halt
halt:
cpy ip a
