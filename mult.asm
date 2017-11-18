; Hardcode factors as the first two set operands
set 43
cpy b a
set 5
cpy c a
; b and c are now the factors
; e will accumulate the product
xor e e
xor f f
loop:
cpy d c
sel d f
not d d
addi d 1
; d is 00 or FF depending on bit in c
and d b
shl d f
add e d
; increment f
addi f 1
; if(f == 8) goto done; else goto loop;
cpy g f
addi g -8
je done
set loop
cpy ip a
done:
xor a a
str a e
set halt
halt:
cpy ip a
