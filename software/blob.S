    .global track
    .global track_size
    .section .rodata
track:
    .incbin "../src/bin/test.bin"
1:
track_size:
    .int 1b - track
