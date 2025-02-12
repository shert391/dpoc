#include "global.h"

int main()
{
    setlocale(LC_ALL, "Russian");
    loadDriver(L"C:\\Users\\User\\source\\repos\\drv\\x64\\Debug\\drv.sys");
    std::cout << "Hello World!\n";
}