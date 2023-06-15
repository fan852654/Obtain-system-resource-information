#include "MemoryPool.h"

MemoryPool::MemoryPool()
{
}

MemoryPool::~MemoryPool()
{
}

MemoryPool* MemoryPool::getInstance()
{
	if (!instance) {
		instance = new MemoryPool;
	}
	return instance;
}