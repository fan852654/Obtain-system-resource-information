#include "SicMpiPool.h"


SicMpiPool::SicMpiPool(int _cnt)
{
	MAX_MPI_POOL_COUNT = _cnt > 1 ? _cnt : 1;
	m_iMpiUsageCnt = 0;
}

SicMpiPool::~SicMpiPool()
{
}

int SicMpiPool::GetOmpThreadsNumbers(void*& token, int needOmpCnt)
{
	if (needOmpCnt <= 1) return 1;
	if ((MAX_MPI_POOL_COUNT - m_iMpiUsageCnt) <= 0)return 1;
	--needOmpCnt;
	m_lockPool.lock();
	int willGiveCnt = (MAX_MPI_POOL_COUNT - m_iMpiUsageCnt) >= needOmpCnt ? needOmpCnt : (MAX_MPI_POOL_COUNT - m_iMpiUsageCnt);
	m_iMpiUsageCnt += willGiveCnt;
	token = (void*)malloc(sizeof(void*));
	m_pool[token] = willGiveCnt;
	m_lockPool.unlock();
	return willGiveCnt + 1;
}

void SicMpiPool::ChangeMaxPoolCnt(int _cnt)
{
	m_lockPool.lock();
	MAX_MPI_POOL_COUNT = _cnt > 1 ? _cnt : 1;
	m_lockPool.unlock();
}

void SicMpiPool::ReleaseOmpThread(void*& token)
{
	if (!token)return;
	m_lockPool.lock();
	std::map<void*, int>::iterator fd = m_pool.find(token);
	if (fd != m_pool.end())
	{
		m_iMpiUsageCnt -= fd->second;
		m_pool.erase(fd);
		free(token);
	}
	m_lockPool.unlock();
}

SicMpiPool* SicMpiPool::getInstance()
{
	std::lock_guard<std::mutex> lock(*m_mutex_mpipool);
	if (instancePool == nullptr) {
		instancePool = new SicMpiPool();
	}
	return instancePool;
}