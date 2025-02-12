#ifdef __um__
#pragma once
#include <string>
#include <windows.h>
#include <filesystem>

#include "dbg.h"
#include "ntdll.h"
#include "ntstatus.h"

using namespace std;
using namespace std::filesystem;

bool loadDriver(_In_ const wchar_t* pDriverPath);
bool unloadDriver(_In_ const wchar_t* pDriverPath);
#endif // __um__