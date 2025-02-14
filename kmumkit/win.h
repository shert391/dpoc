#pragma once

#ifdef __um__
#include <windows.h>
#include <string>
#include "dbg.h"

using namespace std;

#define getCurrDirW(c)                                 \
	[] () {                                            \
		wchar_t szCurrDir[MAX_PATH * 2] {};            \
		GetCurrentDirectoryW(MAX_PATH * 2, szCurrDir); \
		return wstring(szCurrDir) + wstring(c);        \
	}().c_str()

#define getDrvDirW(c)                                                     \
	[] () {                                                               \
		wchar_t szCurrDir[MAX_PATH * 2] {};                               \
		GetSystemDirectoryW(szCurrDir, MAX_PATH * 2);                     \
		return wstring(szCurrDir) + wstring(L"\\drivers\\") + wstring(c); \
	}().c_str()

void* mapfile (const wchar_t* szPath);
#endif