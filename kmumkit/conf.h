﻿#pragma once
#define SIG_CI_OPTIONS				"\x8B\x05\x00\x00\x00\x00\xA8\x01\x74\x00\x48\x8B\x05\x00\x00\x00\x00\x80\x38\x00\x74\x00\x48\x8B\x05\x00\x00\x00\x00\x80\x38\x01"
#define SIG_DDB_CACHE				"\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x3D\x00\x00\x00\x00\x0F\x83\x00\x00\x00\x00"	   // RVA = 0x781CBC
#define SIG_DDB_CACHE_LOCK			"\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8B\x0D\x00\x00\x00\x00\x48\x85\xC9\x74\x58" // RVA = 0x74253C
#define SIG_MM_UNLOADED_DRIVERS		"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9"														   // RVA = 0x5380FC
#define SIG_MM_LAST_UNLOADED_DRIVER "\x8B\x35\x00\x00\x00\x00\x48\x8D\x59\x08"														   // RVA = 0x53B732