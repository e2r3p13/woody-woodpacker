#!/bin/bash

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

echo -ne 'uint8_t stub[] =\n{\n\t' > $2
echo $(objdump -d tmp.o | grep '[[:xdigit:]]:' | cut -f 2 | sed 's/[[:xdigit:]][[:xdigit:]]/0x&, /g' | awk '{$1=$1};1') | sed s'/.$//' >> $2
echo -ne '};\n\nuint32_t stub_len = ' >> $2
objdump -d tmp.o | grep '[[:xdigit:]]:' | cut -f 2 | awk '{$1=$1};1' | wc -w  | tr -d '\n' >> $2
echo ';' >> $2

rm tmp.o

exit 0
