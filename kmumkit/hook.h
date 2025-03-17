#pragma once

void hAbsolute (IN void* pTarget, IN void* pDst, OUT void** ppOriginalStub, IN void* pBase, IN size_t sizeHook, IN const char* szSectionForPit = ".text");
void hMemset (IN void* pDst, IN char val, IN size_t size);
void hMemcpy (IN void* pDst, IN void* pSrc, IN size_t size);
void hRestoreWrite (IN void* pTarget, IN size_t size);
void hAllowWrite (IN void* pTarget, IN size_t size);
