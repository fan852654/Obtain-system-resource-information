#pragma once
#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <set>
#include <climits>
#include <cstddef>
#include <mutex>
#include <exception>
#include <queue>
#include "SpinLocker.h"

#define NORMAL_4K_LIST_CNT			6
#define MININUM_LOOKDOWN_DEPTH		4
#define MXNINUM_LOOKDOWN_DEPTH		256

typedef unsigned short USHORT;
typedef unsigned long ULONG;
typedef void*(*PALLOCATE_FUNCTION)(size_t);
typedef void(*PFREE_FUNCTION)(void*);

typedef struct _GENERAL_MEMORY_BLOCK {
	void* Memory_Ptr;
	_GENERAL_MEMORY_BLOCK* next;

}GENERAL_MEMORY_BLOCK,* PGENERAL_MEMORY_BLOCK;

typedef struct _GENERAL_LOOKDOWN {
	PGENERAL_MEMORY_BLOCK MemoryBlocksHead;
	PGENERAL_MEMORY_BLOCK MemoryBlocksUsed;
	USHORT Depth;
	USHORT MaxinumDepth;
	ULONG TotalAllocates;
	union {
		ULONG AllocateMisses;
		ULONG AllocateHits;
	};

	ULONG TotalFrees;
	union {
		ULONG FreeMisses;
		ULONG FreeHits;
	};

	ULONG Size;

	PALLOCATE_FUNCTION Allocate;
	PFREE_FUNCTION Free;
	
	ULONG LastTotalAllocates;
	union {
		ULONG LastAllocateMisses;
		ULONG LastAllocateHits;
	};

	SpinLocker blockLocker;
	std::queue<void*> allocatedBlock;
	_GENERAL_LOOKDOWN* next;

}GENERAL_LOOKDOWN, *PGENERAL_LOOKDOWN;

typedef struct _MEMORY_LOOKDOWN_LIST {

	PGENERAL_LOOKDOWN L;
	std::mutex* Lock;

	PALLOCATE_FUNCTION Allocate;
	PFREE_FUNCTION Free;

	std::set<int> misshitAllocateSize;

}MEMORY_LOOKDOWN_LIST,*PMEMORY_LOOKDOWN_LIST;

namespace LookdownList {
	static int NORMAL_4K_SIZE[NORMAL_4K_LIST_CNT] = { 4,8,16,32,64,128 };

	static PGENERAL_LOOKDOWN QueryLookdownBlockBySize(PMEMORY_LOOKDOWN_LIST _trunk, size_t &_size);

	static PMEMORY_LOOKDOWN_LIST InitializeLookdownList(PALLOCATE_FUNCTION _allocatefunc = nullptr, PFREE_FUNCTION _freefunc = nullptr);
	static void SetAllocateOrFreeFuncToLookDownBySize(PMEMORY_LOOKDOWN_LIST _trunk, size_t _size, PALLOCATE_FUNCTION _allocatefunc = nullptr, PFREE_FUNCTION _freefunc = nullptr);
	static PGENERAL_LOOKDOWN AddLookdownTrunkToLookdownList(PMEMORY_LOOKDOWN_LIST _trunk, size_t _size);
	static PGENERAL_MEMORY_BLOCK GeneralMemoryBlock(void* ptr, size_t _size, int _count);
	static bool ReleaseLookdownList(__in PMEMORY_LOOKDOWN_LIST _trunk, bool force = false);

	static void* AllocateFromLookdownList(__in PMEMORY_LOOKDOWN_LIST _lookdownlist, size_t _size);
	static void FreeToLookdownList(__in PMEMORY_LOOKDOWN_LIST _lookdownlist, __in void* _node);

	static int GetTargetSizeBySize(int size);
};

#endif