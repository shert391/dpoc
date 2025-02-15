#include <windows.h>
#include <iostream>
#include "hde/hde64.h"

void main () {
	unsigned char ida_chars[] = {0x8B, 0x05, 0x7D, 0x87, 0xFE, 0xFF};
	hde64s		  hde		  = disasm(ida_chars);
	printf("--");
}