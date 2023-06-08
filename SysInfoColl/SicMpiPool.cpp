#include "SicMpiPool.h"


SicMpiPool::SicMpiPool(int _cnt)
{
	MAX_MPI_POOL_COUNT = _cnt > 1 ? _cnt : 1;
}

SicMpiPool::~SicMpiPool()
{
}

void SicMpiPool::ChangeMaxPoolCnt(int _cnt)
{
	m_lockPool.lock();
	MAX_MPI_POOL_COUNT = _cnt > 1 ? _cnt : 1;
	m_lockPool.unlock();
}

SicMpiPool* SicMpiPool::getInstance()
{
	m_mutex_mpipool->lock();
	if (instance == nullptr) {
		instance = new SicMpiPool();
	}
	m_mutex_mpipool->unlock();
	return instance;
}