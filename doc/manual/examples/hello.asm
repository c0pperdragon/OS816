  PEA #(message).high.
  PEA #(message).low.
  JSL >$FFF00C
  RTL
message
  DB "Hello, user!",10,0
