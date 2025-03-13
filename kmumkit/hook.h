#pragma once

void hAbsolute (IN void* pTarget, IN void* pDst, OUT void** ppOriginalStub, IN void* pBase, IN size_t sizeHook, IN const char* szSectionForPit = ".text");
void hRestoreWrite (IN void* pTarget, IN size_t size);
void hAllowWrite (IN void* pTarget, IN size_t size);
