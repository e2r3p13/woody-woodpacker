#!/bin/bash

# This script generates a c style shellcode from 1st argument and output it in 2nd argument.
# It creates a 'stub' variable (uint8_t array), and a 'stub_len' variable (uint32_t).

if [[ "$#" -ne 2 ]]
then
	echo 'Usage: sh shellcode.sh <obj> <out>'
	exit 1
fi

nasm -f elf64 $1 -o tmp.o

if [[ $? -ne 0 ]]
then
	exit 1
fi

echo '/*' > $2
echo ' * This file has been automatically generated with' >> $2
echo " * shellcode.sh from $1." >> $2
echo -e '*/\n' >> $2

echo -e '#include <stdint.h>\n' >> $2
echo -ne 'uint8_t stub[] =\n{\n\t' >> $2
echo $(objdump -dz tmp.o | grep '[[:xdigit:]]:' | cut -f 2 | sed 's/[[:xdigit:]][[:xdigit:]]/0x&, /g' | awk '{$1=$1};1') | sed s'/.$//' >> $2
echo -ne '};\n\nuint32_t stub_len = ' >> $2
objdump -dz tmp.o | grep '[[:xdigit:]]:' | cut -f 2 | awk '{$1=$1};1' | wc -w  | tr -d '\n' >> $2
echo ';' >> $2

rm tmp.o

exit 0
