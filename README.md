# assembly_in_cpp
C++ interpreter for assembly code

This program is intended to interpret assembly, particularly that used in 
microprocessors like the MSP 430 G series, in the C++ language.

The end goal is for users to be able to input any valid assembly program 
that would operate these processors and get the same results.
This will eventually include features like input/output port simulation,
custom libraries, and more.

The program compiles each instruction into a compressed 32-bit version 
of itself. The first byte is devoted to the instruction code, while 
the other 3 bytes store the argument(s).

Because of challenges associated with storing all of the necessary data 
for an instruction within 32 bits, the preprocessor includes some 
code generation that allows mutliple lines to contribute to 
one actual program instruction. 
