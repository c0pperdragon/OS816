# OS816

A small single board computer using the 65C816 processor running at 10MHz, together with 512KB of RAM, 
512KB of flash and a simple but effective I/O circuitry.

I made a small boot loader program for this hardware sitting in the top 4KB of flash to upload further
code via a serial connection to avoid the need of swapping the Flash IC in and out of the board all the time.

User programs can be developed with the WDC compiler tool chain that provides an assembler as well as 
a C compiler confirming to the C89/90 standard (a.k.a ANSI C). 
I had to fix some bugs and add implementations for stdin/stdout over serial and file access
to utilize the unoccupied area of the Flash memory. By now it is complete enough to run the "frotz" 
interactive fiction interpreter to play some of the old text adventure classics.

![alt text](doc/rev3board.jpg "Revision 3")

## Minimum 65C816 setup

This processor is a tricky beast to use in a circuit as it has some unusual quirks that need consideration,
especially when you want to use more than 64KB of address space.

1. RD and WR signals need to be externally generated.
2. Address bits 16-23 are multiplexed with the data bus.
3. Difficult to use flat RAM and ROM areas, as both the stack as well as the initial boot code needs to be 
placed in bank 0.

Points 1 and 2 are just solved with some extra logic on the board. Point 3 is solved by using the emulation bit
to force the top 8 address lines to $FF during startup time. Once the program is running it switches to native mode
to make use of the full address range with flat regions for both RAM, IO and flash.

## Memory map

| from   | to     | type 
| ------ | ------ | -----------------
| 000000 | 07FFFF | RAM
| 400000 | 400000 | IO port
| C00000 | C7EFFF | User flash
| FFF000 | FFFFFF | Boot loader flash

Many other ranges of the 16MB space are mirrors of some of the specified ranges here, but should not 
be used in order to allow future extensions.

## Compiling for the platform

The free WDC compiler tools (running on Windows only) are used that include a C compiler, so it is easy to
get most things done without directly touching the 65C816 machine code. The OS816 libraries are designed
for the "large" memory model to evenly access static and dynamic data without bothering
about banks, direct page and all the other intricate details of this specific CPU.
Low level access to the hardware (essentially to the IO port, but also the flash programming functions) 
is provided by libraries that are directly written in machine code for best performance and because sometimes
it would not be possible otherwise.

The necessary compiler and linker can be found at the Western Design Center's
[download page](https://www.westerndesigncenter.com/wdc/tools.php).
Additionally you need a [python 3 interpreter](https://www.python.org/downloads/) to run my extra build tools.

To see how all these tools should be used, have a look at the build scripts for the various examples provided.

## Get the boot loader into the flash

This is a step you probably need to do only once, but it requires the use of a dedicated chip programmer, 
like the TL866-II Plus and its corresponding programming software. Building the OS816 libraries by executing
the 'build.bat' script in the 'src' directory, will create some libraries in the 'bin' directory,
and also the 'boot.hex' file which needs to be written to the flash IC.
To avoid buying this not completely cheap programmer I could provide you with a pre-flashed IC for a reasonable
price. 

With the kit I sell at [tindie](https://www.tindie.com/products/c0pperdragon/os816-single-board-computer-kit/),
the flash IC comes already pre-programmed.

## Serial interface

With its simple IO hardware, the CPU is in charge of all signal handling. With a 10MHz clock and hand-crafted machine code
it was possible to implement a reliable serial communication that can run at 115.2kHz (1 start bit, 1 stop bit, no parity). 
But as the CPU can not do any communication when being busy with other things it is necessary that the 
communication partner must only send data when the CPU is prepared to receive it. A straight forward solution 
is the use of hardware flow control using RTS/CTS handshake lines. In my reference setup I use a cheap USB-to-UART 
interface board with support for these signals.

| Pin location | OS816 | Partner |
| ------------ | ----- | ------- |
| OUTPUT 0     | TX    | RX      |
| OUTPUT 1     | RTS   | CTS     |
| INPUT  7     | RX    | TX      |
| INPUT  6     | CTS   | RTS     |

If your USB-to-UART interface does only have a CTS input, but no RTS output, this can also be made to work.
Just pull the CTS input of the OS816 permanently to GND to disable flow control in the direction from the OS816 
to your computer, assuming that your computer is fast enough so it can always process all incomming data.

## Uploading 

Compiling with the recommended tools will create a file in Intel HEX format. This file contains the "user code" which can be 
written to flash using the resident boot loader program. When starting up the machine with a serial terminal attached,
the boot loader enters a command line interface. Here memory locations can be inspected or modified and also the content
of the flash can be reprogrammed. After completely erasing the user flash area with the "!" command, you can conveniently 
dump a whole intel hex file into the terminal whose lines will be treated as individual programming commands.
Once the user program is in the flash, the command "x" will start up this program. 
When the machine starts up with a user program already installed, the boot loader will give you 1 second 
time to enter the command line interface by pressing a key. Otherwise it will fire up the user program directly.

## Overclocking

I have chosen this moderately fast 10MHz as the clock speed to always stay safely inside all the timing constraints 
and to make cycle-counting as simple as possible. But I was also able to run the board at 16MHz without problems.
If you want to try this, you need to swap out the crystal and set the serial communication speed in your
terminal program accordingly.

## Contact

For questions regarding purchases of either a pre-programmed flash IC or a whole kit
for self-assembly (using through-hole parts exclusively),
please contact me directly at:  reinhard.grafl (at) aon.at

For other questions that may also be of interest for other users, 
please create an [issue ticket](https://github.com/c0pperdragon/OS816/issues).  
