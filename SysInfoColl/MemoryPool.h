#pragma once
#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <climits>
#include <cstddef>
#include <functional>
class MemoryPool
{
public:
	static MemoryPool* instance;
	static MemoryPool* getInstance();
	MemoryPool();
	~MemoryPool();
private:

};

#endif // !MEMORY_POOL_H