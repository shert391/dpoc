#pragma once

#define IMAGE_NT_HEADERS64(p) (IMAGE_NT_HEADERS64*)((uintptr_t)p + ((IMAGE_DOS_HEADER*)p)->e_lfanew)

#define TYPE_RELOC(entry)                 entry >> 12
#define COUNT_ENTRY_IN_RELOC_BLOCK(block) (block->SizeOfBlock - 8) / 2
#define NEXT_RELOC_BLOCK(block)           (void*)((char*)block + block->SizeOfBlock)
#define ROUND_UP_TO_4K_PAGE(p)            (uintptr_t)(((uintptr_t)p + 0xFFF) & ~0xFFF)

void* fExecuteMap (IN const wchar_t* szPath);
void* fFileMap (IN const wchar_t* szPath, OUT OPTIONAL size_t* pSize = nullptr);