#include "global.h"

int main () {
    setlocale(LC_ALL, "Russian");
    wchar_t unsigned char code[] = {0x8B, 0x05, 0x7D, 0x87, 0xFE, 0xFF};
    ntiomsi().drvload(path(L"C:\\Users\\User\\source\\repos\\drv\\x64\\Debug\\drv.sys"));
}