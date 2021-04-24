# OS816

A tiny operating system (in fact just some startup code and a few libraries) 
to work on the 65C816 CPU in a minimal experimental board.
This project also provides some examples (starting with the most basic LED animation) 
to use these libraries and actually get the tool chain to work.

## Compiling for the platform

The WDC compiler tools are used that include a C compiler, so it is possible to
get things done without directly touching the 65C816 machine code. By sacrificing 
some optimization options, it is possible to make a system that can utilize the whole
RAM and ROM of the system without bothering with banks, direct pages and all those
intricate details of this particular CPU.
Access to the hardware (essentially to the IO port) is provided by libraries that are directly
written in machine code for best performance and because sometimes it would not be 
possible otherwise.

## Performance hints

* The C compiler internally uses the 16 bit register mode of the CPU, so working
with 16 bit integers is the default and most optimized option. Using 8-bit values for
local variables or parameters instead only degrades performance. 

* Function calls have a pretty high overhead, so maybe it makes sense to inline 
certain things using macros.

