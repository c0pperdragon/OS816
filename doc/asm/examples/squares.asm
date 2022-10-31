send    set $FFF006
sendnum set $FFF01B

  STZ n
computesquare
  LDA #0
  LDX n
  BEQ squaredone
squareloop
  CLC
  ADC n  
  DEX
  BNE squareloop
squaredone
  PHA
  JSL >sendnum
  PEA #" "
  JSL >send
  ; continue with next
  INC n
  LDA n
  CMP #256
  BNE computesquare
  RTL
n
  DW 0
