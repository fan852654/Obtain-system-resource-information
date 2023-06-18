#include "MemoryPool.h"

PMEMORY_LOOKDOWN_LIST LookdownList::InitializeLookdownList(PALLOCATE_FUNCTION _allocatefunc, PFREE_FUNCTION _freefunc)
{
	PMEMORY_LOOKDOWN_LIST result = new MEMORY_LOOKDOWN_LIST;
	result->Lock = new std::mutex;
	result->Allocate = _allocatefunc ? _allocatefunc : malloc;
	result->Free = _freefunc ? _freefunc : free;
	PGENERAL_LOOKDOWN header = nullptr;
	PGENERAL_LOOKDOWN end = nullptr;
	for (int i = 0; i < NORMAL_4K_LIST_CNT; ++i)
	{
		PGENERAL_LOOKDOWN nd = AddLookdownTrunkToLookdownList(result, NORMAL_4K_SIZE[i]);
		if (!header) {
			header = nd;
			end = nd;
		}
		else {
			end->next = nd;
			end = end->next;
		}
	}
	if(end)
		end->next = nullptr;
	std::lock_guard<std::mutex> locker(*result->Lock);
	result->L = header;
	return result;
}

PGENERAL_LOOKDOWN LookdownList::AddLookdownTrunkToLookdownList(PMEMORY_LOOKDOWN_LIST _trunk, size_t _size)
{
	if (!_trunk)return nullptr;
	PGENERAL_LOOKDOWN result = new GENERAL_LOOKDOWN;
	result->Allocate = _trunk->Allocate;
	result->Free = _trunk->Free;
	result->AllocateHits = 0;
	result->TotalAllocates = 0;
	result->LastAllocateMisses = 0;
	result->LastTotalAllocates = 0;
	result->Size = _size;
	result->Depth = MININUM_LOOKDOWN_DEPTH;
	result->MaxinumDepth = MXNINUM_LOOKDOWN_DEPTH;
	void* tank = result->Allocate(_size * result->Depth);
	result->MemoryBlocksHead = GeneralMemoryBlock(tank, _size, result->Depth, _trunk, result);
	result->allocatedBlock.push(result->MemoryBlocksHead);
	return result;
}

PGENERAL_MEMORY_BLOCK LookdownList::GeneralMemoryBlock(void* ptr, size_t _size, int _count, PMEMORY_LOOKDOWN_LIST _trunkptr,PGENERAL_LOOKDOWN _parent)
{
	if (!ptr) return nullptr;
	PGENERAL_MEMORY_BLOCK result = new GENERAL_MEMORY_BLOCK;
	result->_trunk = _trunkptr;
	result->used = false;
	result->pre = nullptr;
	result->LookdownTrunk = _parent;
	PGENERAL_MEMORY_BLOCK end = result;
	void* ptrindex = ptr;
	for (int i = 0; i < _count; ++i)
	{
		if (!end) {
			_exception* exc = new _exception;
			exc->type = _OVERFLOW;
			throw exc;
		}
		end->Memory_Ptr = ptrindex;
		if (i != _count - 1) {
			end->next = new GENERAL_MEMORY_BLOCK;
			end->_trunk = _trunkptr;
			end->used = false;
			end->pre = end;
			end->LookdownTrunk = _parent;
			end = end->next;
		}
		else {
			end = nullptr;
		}
		ptrindex = &ptrindex + _size;
	}
	_parent->MemoryBlocksEnd = end;
	
	return result;
}

void LookdownList::SetAllocateOrFreeFuncToLookDownBySize(PMEMORY_LOOKDOWN_LIST _trunk, size_t _size, PALLOCATE_FUNCTION _allocatefunc, PFREE_FUNCTION _freefunc)
{
	PGENERAL_LOOKDOWN ld = QueryLookdownBlockBySize(_trunk, _size);
	ld->Allocate = _allocatefunc ? _allocatefunc : malloc;
	ld->Free = _freefunc ? _freefunc : free;
}

bool LookdownList::ReleaseLookdownList(__in PMEMORY_LOOKDOWN_LIST _trunk, bool force)
{
	if (!_trunk)return false;
	{
		std::lock_guard<std::mutex> lock(*_trunk->Lock);
		PGENERAL_LOOKDOWN nodehead = _trunk->L;
		if (!force) {
			while (nodehead)
			{
				if (nodehead->MemoryBlocksHead && nodehead->MemoryBlocksHead->used) {
					return false;
				}
				nodehead = nodehead->next;
			}
			nodehead = _trunk->L;
		}
		while (nodehead)
		{
			PGENERAL_LOOKDOWN nxt = nodehead->next;

			for (PGENERAL_MEMORY_BLOCK hd = nodehead->MemoryBlocksHead;hd;)
			{
				PGENERAL_MEMORY_BLOCK pmbnxt = hd->next;
				delete hd;
				hd = pmbnxt;
			}

			while (!nodehead->allocatedBlock.empty())
			{
				_trunk->Free(nodehead->allocatedBlock.front()); 
				nodehead->allocatedBlock.pop();
			}
			_trunk->Free(nodehead);
			delete nodehead;

			nodehead = nxt;
		}
	}
	delete _trunk->Lock;
	delete _trunk;
	return true;
}

void* LookdownList::AllocateFromLookdownList(__in PMEMORY_LOOKDOWN_LIST _lookdownlist, size_t _size)
{
	size_t Targetsize = _size;
	PGENERAL_LOOKDOWN ldb = QueryLookdownBlockBySize(_lookdownlist, Targetsize);
	if (!ldb) {
		_lookdownlist->misshitAllocateSize.insert(_lookdownlist->misshitAllocateSize.begin(), Targetsize);
		return _lookdownlist->Allocate(_size);
	}
	void* resptr = nullptr;
	ldb->blockLocker.lock();
	if (ldb->MemoryBlocksHead) {
		++ldb->AllocateHits;
		resptr = ldb->MemoryBlocksHead->Memory_Ptr;
		ldb->MemoryBlocksHead->used = true;
		ldb->MemoryBlocksHead->pre = ldb->MemoryBlocksEnd;
		ldb->MemoryBlocksEnd->next = ldb->MemoryBlocksHead;
		PGENERAL_MEMORY_BLOCK headnext = ldb->MemoryBlocksHead->next;
		ldb->MemoryBlocksHead->next = nullptr;
		ldb->MemoryBlocksHead = headnext;
	}
	else {
		++ldb->AllocateMisses;
		resptr = ldb->Allocate(_size);
	}
	ldb->blockLocker.unlock();
	++ldb->TotalAllocates;
	return resptr;
}

void LookdownList::FreeToLookdownList(__in PMEMORY_LOOKDOWN_LIST _lookdownlist,__in void* _node)
{
	if (!_lookdownlist)return;
	void** ptrTrunk = (void**)((int*)_node + GMB_2_TRUNK_PTRSIZE);
	if (ptrTrunk == (void*)_lookdownlist) {
		PGENERAL_MEMORY_BLOCK block = (PGENERAL_MEMORY_BLOCK)_node;
		PGENERAL_LOOKDOWN lookdowntrunk = (PGENERAL_LOOKDOWN)block->LookdownTrunk;
		lookdowntrunk->blockLocker.lock();

		block->used = false;
		block->pre->next = block->next;
		if (!block->next)
			lookdowntrunk->MemoryBlocksEnd = block->pre;
		block->pre = nullptr;
		block->next = lookdowntrunk->MemoryBlocksHead;
		lookdowntrunk->MemoryBlocksHead->pre = block;
		lookdowntrunk->MemoryBlocksHead = block;

		lookdowntrunk->blockLocker.unlock();
	}
	else {
		_lookdownlist->Free(_node);
	}
}

int LookdownList::GetTargetSizeBySize(int _size)
{
	int targetsize = NORMAL_4K_SIZE[0];
	while (targetsize < _size)
	{
		targetsize *= 2;
	}
	return targetsize;
}

PGENERAL_LOOKDOWN LookdownList::QueryLookdownBlockBySize(PMEMORY_LOOKDOWN_LIST _trunk, size_t &_size)
{
	int targetsize = GetTargetSizeBySize(_size);
	PGENERAL_LOOKDOWN header = _trunk->L;
	while (header)
	{
		if (header->Size == targetsize)
			return header;
		header = header->next;
	}
	_size = targetsize;
	return nullptr;
}