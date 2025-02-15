#pragma once
#ifdef __um__
#include <filesystem>
#include <functional>
#include <windows.h>
#include <string>

using namespace std;
using namespace std::filesystem;
#endif

#ifdef __km__
#include <ntddk.h>
#include <ntimage.h>
#endif

#pragma region all
#include "hde/hde64.h"
#include "amd64.h"
#include "dbg.h"
#include "sig.h"
#pragma endregion

#ifdef __um__
#include "ntstatus.h"
#include "ntdll.h"
#include "ntos.h"
#include "conf.h"
#include "win.h"
#endif // __um__