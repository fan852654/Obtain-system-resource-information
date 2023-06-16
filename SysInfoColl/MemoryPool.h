#pragma once
#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <climits>
#include <cstddef>
#include <mutex>

typedef unsigned short USHORT;
typedef unsigned long ULONG;
typedef void*(*PALLOCATE_FUNCTION)(size_t);
typedef void(*PFREE_FUNCTION)(void*);

typedef struct _GENERAL_MEMORY_BLOCK {
	void* Memory_Ptr;
	PGENERAL_MEMORY_BLOCK* next;

}GENERAL_MEMORY_BLOCK,* PGENERAL_MEMORY_BLOCK;

typedef struct _GENERAL_LOOKDOWN {
	PGENERAL_MEMORY_BLOCK MemoryBlocksHead;
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

}GENERAL_LOOKDOWN, *PGENERAL_LOOKDOWN;

typedef struct _MEMORY_LOOKDOWN_LIST {

	GENERAL_LOOKDOWN L;
	std::mutex* Lock;

}MEMORY_LOOKDOWN_LIST,*PMEMORY_LOOKDOWN_LIST;

namespace LookdownList {
	static int NORMAL_4K_SIZE[4] = { 4,8,16,32 };
	static PMEMORY_LOOKDOWN_LIST instance = nullptr;
	static PMEMORY_LOOKDOWN_LIST InitializeLookdownList();
	static void ReleaseLookdownList();
	static void* AllocateFromLookdownList(__in PMEMORY_LOOKDOWN_LIST _lookdownlist);
	static void FreeToLookdownList(__in PMEMORY_LOOKDOWN_LIST _lookdownlist, __in void* _node);
};

#endif