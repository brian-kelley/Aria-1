; write min(a, b) to [c]
xor c, c
set 200
cpy b, a
set 100
; save copy of a for subtraction
cpy d, a
sub d, b  ; like cmp in x86
jl else
str c, b
set done
cpy ip, a
else:
str c, a
set done
done:
cpy ip, a

