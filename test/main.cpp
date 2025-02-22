#include <windows.h>
#include "file.h"

void main () {
    void* pEp = fExecuteMap(L"C:\\IDEProjects\\cpp_projs\\test_console\\x64\\Release\\test_console.exe");
    ((void(__fastcall*)())pEp)();
}