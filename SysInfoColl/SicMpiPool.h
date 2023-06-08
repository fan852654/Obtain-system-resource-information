#pragma once
#ifndef SICMPIPOOL_H
#define SICMPIPOOL_H

#include <mutex>
#include <omp.h>

static std::mutex* m_mutex_mpipool = new std::mutex;
class SicMpiPool
{
public:	
	static SicMpiPool* instance;
	static SicMpiPool* getInstance();
	SicMpiPool(int _cnt = 1);
	~SicMpiPool();

	void ChangeMaxPoolCnt(int _cnt = 1);
private:
	int MAX_MPI_POOL_COUNT;
protected:
	std::mutex m_lockPool;
};
SicMpiPool* SicMpiPool::instance = nullptr;
#endif // !SICMPIPOOL_H
