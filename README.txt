TODO add the ability write bytes into memory manually.
e.g.
```
test:
    .byte "test", 1 ; Writes ascii bytes for string and byte for 1
```

TODO add ability to use x and y registers
```
lda $01,x
lda $02,y
```
Will also require inx, iny, dcx, dcy to increment and decrement x and y registers

