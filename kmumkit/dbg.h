#pragma once
#pragma region all
#define prefixdbg	  "[~] "
#define prefixerror	  "[-] "
#define prefixsuccess "[+] "

#ifdef _DEBUG
#define dbgbp __debugbreak();
#else
#define dbgbp
#endif
#pragma endregion

#ifdef __km__
#define cnull(p) \
	if (!p) {    \
		dbgbp    \
	}
#define ctrue(f) \
	if (!f) {    \
		dbgbp    \
	}
#ifdef _DEBUG
#define dbg(msg, ...) \
	{ DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, prefixdbg msg "\n", __VA_ARGS__); }
#else
#define dbg(msg, ...)
#endif
#endif // __km__

#ifdef __um__
#define сreset		"\033[0m"
#define black		"\033[30m"		  /* Black */
#define red			"\033[31m"		  /* Red */
#define green		"\033[32m"		  /* Green */
#define yellow		"\033[33m"		  /* Yellow */
#define blue		"\033[34m"		  /* Blue */
#define magenta		"\033[35m"		  /* Magenta */
#define cyan		"\033[36m"		  /* Cyan */
#define white		"\033[37m"		  /* White */
#define boldblack	"\033[1m\033[30m" /* Bold Black */
#define boldred		"\033[1m\033[31m" /* Bold Red */
#define boldgreen	"\033[1m\033[32m" /* Bold Green */
#define boldyellow	"\033[1m\033[33m" /* Bold Yellow */
#define boldblue	"\033[1m\033[34m" /* Bold Blue */
#define boldmagenta "\033[1m\033[35m" /* Bold Magenta */
#define boldcyan	"\033[1m\033[36m" /* Bold Cyan */
#define boldwhite	"\033[1m\033[37m" /* Bold White */

#define fm(flags, msgid)           \
	wchar_t* pMsgBuffer = nullptr; \
	FormatMessage(flags, nullptr, msgid, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (wchar_t*)&pMsgBuffer, 0, nullptr);

#ifdef _DEBUG
#define dbg(msg, ...) \
	printf(prefixdbg msg "\n", __VA_ARGS__);
#else
#define dbg(msg, ...)
#endif

#define prf(color, p, msg, ...) \
	printf(color p msg сreset, __VA_ARGS__)

#define error(msg, ...) \
	prf(red, prefixerror, "%s -> \n" msg, __func__, __VA_ARGS__)

#define success(msg, ...) \
	prf(green, prefixsuccess, msg, __VA_ARGS__)

#define cntstatus(status)                                       \
	if (!NT_SUCCESS(status)) {                                  \
		prf(red, prefixerror, "%s -> %lx\n", __func__, status); \
		dbgbp;                                                  \
	}

#define clstatus(lstatus)                                                         \
	if (lstatus != ERROR_SUCCESS) {                                               \
		fm(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, lstatus); \
		printf(red prefixerror "%s -> %ls\n" сreset, __func__, pMsgBuffer);       \
		LocalFree(pMsgBuffer);                                                    \
		dbgbp;                                                                    \
	}

#define chandle(handle)                                                       \
	if ((HANDLE)handle == INVALID_HANDLE_VALUE) {                             \
		error("INVALID_HANDLE_VALUE; GetLastError() = 0x%X", GetLastError()); \
		dbgbp;                                                                \
	}

#define cnull(ptr)                                                      \
	if (ptr == nullptr) {                                               \
		error("Fail exec func; GetLastError() = 0x%X", GetLastError()); \
		dbgbp;                                                          \
	}

#define czero(v)                                                        \
	if (v == 0) {                                                       \
		error("Fail exec func; GetLastError() = 0x%X", GetLastError()); \
		dbgbp;                                                          \
	}

#define ctrue(value)                                                    \
	if (value != true) {                                                \
		error("Fail exec func; GetLastError() = 0x%X", GetLastError()); \
		dbgbp;                                                          \
	}
#endif