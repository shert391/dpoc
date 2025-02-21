#pragma once
#pragma region all
#define WMAX_PATH MAX_PATH * 2
#pragma endregion

#ifdef __um__
#define getCurrDirW(c)                                 \
    [] () {                                            \
        wchar_t szCurrDir[MAX_PATH * 2] {};            \
        GetCurrentDirectoryW(MAX_PATH * 2, szCurrDir); \
        return path(szCurrDir) / c;                    \
    }()

#define getDrvDirW(c)                                 \
    [] () {                                           \
        wchar_t szCurrDir[MAX_PATH * 2] {};           \
        GetSystemDirectoryW(szCurrDir, MAX_PATH * 2); \
        return path(szCurrDir) / c;                   \
    }()

void* mapfile (path& path);
#endif