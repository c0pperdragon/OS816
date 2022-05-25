# OS816 Rev.4 - Bill of Materials

All parts go to the top side of the PCB, which is were the part designators are printed on.
The bottom side of the PCB just shows where the various architectural components (CPU, RAM, etc.)
are placed.
Connector JADR1 is inteded for possible extension and not populated.

| Manufacturer Number  | Designator  |  Qty  |  Description                 | Case       | Assembly notes                            |
| -------------------- | ----------- |------ | ---------------------------- | ---------- | ----------------------------------------- |
| W65C816S             | U1          |     1 | Processor                    | DIP-40     |                                           |
| AS6C4008-55PCN       | U2          |     1 | Static RAM 512K              | DIP-32     |                                           |
| SST39SF040-70-4C-PHE | U3          |     1 | NOR Flash 512K               | DIP-32     |                                           |
| 74AC573/74AHC573     | U4          |     1 | Octal Transparent Latch      | DIP-20     |                                           |
| 74HC04               | U5          |     1 | Hex Inverter                 | DIP-14     |                                           |
| 74HC138              | U6          |     1 | 3-to-8 Decoder               | DIP-16     |                                           |
| 74HC574              | U7          |     1 | Flip-Flop                    | DIP-20     |                                           |
| 74HC245              | U8          |     1 | Bus Tranceiver               | DIP-20     |                                           |
|                      | U1          |     1 | IC Socket                    | DIP-40     |                                           |
|                      | U2, U3      |     2 | IC Socket                    | DIP-32     |                                           |
|                      | U4, U7, U8  |     3 | IC Socket                    | DIP-20     |                                           |
|                      | U6          |     1 | IC Socket                    | DIP-16     |                                           |
|                      | U5          |     1 | IC Socket                    | DIP-14     |                                           |
|                      | R1          |     1 | Resistor 1k                  | 6mm        | brown-black-red                           |
|                      | R2          |     1 | Resistor 1M                  | 6mm        | brown-black-green                         |
|                      | R3, R4      |     2 | Resistor 10k                 | 6mm        | brown-back-orange                         |
|                      | RN1, RN3    |     2 | Series Resistor 8x10k        | SIP-9      | match location of common pin              |
|                      | RN2         |     1 | Series Resistor 8x1k         | SIP-9      | match location of common pin              |
|                      | C1-C4       |     4 | Ceramic Capacitor 100nF      | MLCC 3mm   |                                           |
|                      | C8          |     1 | Ceramic Capacitor 1uF        | MLCC 4mm   |                                           |
|                      | C11         |     1 | Electrolytic Capacitor 100uF | radial 5mm | check correct polarity                    |
|                      | C12, C13    |     2 | Ceramic Capacitor 33pF       | disc 6mm   |                                           |
|                      | LED0 - LED7 |     8 | Rectangular LED 2x5 mm       | 2x5x5 mm   | short leg (negative) next to the resistor |
|                      | Y1          |     1 | Quarz Crystal 10MHz          | 10mm       |                                           |
|                      | RESET1      |     1 | Push Button                  | 6x6mm      |                                           | 
|                      | JIOPWR1     |     1 | Pin Header 1x20              | rm 2.54mm  |                                           |
