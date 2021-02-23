main:
    LDA #$0A   ; Loads 10 in to accumulator
loop:
    OUT A      ; Outputs the accumulator's value
    OTC #$0A   ; Outputs newline ASCII character
    SUB #$01   ; Subtracts one
    BRP loop   ; Go back to label loop
    OUT #$7F   ; Output 127 to signify finished
    LDA #$7F
    STA $7FFF
    HLT