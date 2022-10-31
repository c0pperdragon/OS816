loop
  INY
  BNE loop
  INC
  ST8 >$7F0000
  BRA loop
