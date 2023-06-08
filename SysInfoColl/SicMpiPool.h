#pragma once
#ifndef SICMPIPOOL_H
#define SICMPIPOOL_H

#include <mutex>
#include <omp.h>

static std::mutex* m_mutex_mpipool = new std::mutex;
class SicMpiPool
{
public:	
	static SicMpiPool* instancePool;
	static SicMpiPool* getInstance();
	SicMpiPool(int _cnt = 1);
	~SicMpiPool();

	int GetOmpThreadsNumbers();
	void ChangeMaxPoolCnt(int _cnt = 1);
private:
	int MAX_MPI_POOL_COUNT;
protected:
	std::mutex m_lockPool;
};
SicMpiPool* SicMpiPool::instancePool = nullptr;
#endif // !SICMPIPOOL_H
