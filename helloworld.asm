main:
    LDX #$00     ; Set x register to 0
loop:
    LDA text,x  ; Load character from label text with offset of x into accumulator
    BRZ done    ; If accumulator == 0 goto done
    OTC A       ; Output value of accumulator as ascii character
    INX         ; Increment X register
    BRA loop    ; Goto loop
done:
    HLT         ; Stop execution
text:
    .byte "hello world", #$0A, "Goodbye  \"world\", #$0A ; .byte is TokenType macro, anything after a macro is passed to a seperate lexer to change how it is tokenized