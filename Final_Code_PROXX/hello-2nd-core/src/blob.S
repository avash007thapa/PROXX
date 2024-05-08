    .global coin
    .global coin_size
    .section .rodata
coin:
    .incbin "../src/bin/piano.bin"
1:
coin_size:
    .int 1b - coin
