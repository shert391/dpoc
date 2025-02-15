#include "global.h"

int main () {
	setlocale(LC_ALL, "Russian");
	auto a = path(L"C:\\Users\\User\\source\\repos\\drv\\x64\\Debug\\drv.sys");
	rtcore().drvload(a);
}