#pragma once
#ifndef SICMPIPOOL_H
#define SICMPIPOOL_H

#include <mutex>
#include <omp.h>
#include <map>

static std::mutex* m_mutex_mpipool = new std::mutex;
class SicMpiPool
{
public:	
	static SicMpiPool* instancePool;
	static SicMpiPool* getInstance();
	SicMpiPool(int _cnt = 1);
	~SicMpiPool();

	int GetOmpThreadsNumbers(void*& token ,int needOmpCnt = 1);
	void ChangeMaxPoolCnt(int _cnt = 1);
	void ReleaseOmpThread(void*& token);
private:
	int MAX_MPI_POOL_COUNT;
protected:
	std::mutex m_lockPool;
	std::map<void*, int> m_pool;
	int m_iMpiUsageCnt;
};
#endif // !SICMPIPOOL_H
