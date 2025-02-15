#include "global.h"

int main () {
	setlocale(LC_ALL, "Russian");
	unsigned char code[] = {0x8B, 0x05, 0x7D, 0x87, 0xFE, 0xFF};

	auto a = path(L"C:\\Users\\User\\source\\repos\\drv\\x64\\Debug\\drv.sys");
	ntiomsi().drvload(a);
}