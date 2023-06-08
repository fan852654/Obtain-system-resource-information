#include "Sic_ProcessView.h"

Sic_ProcessView::Sic_ProcessView()
{
	if (!m_processview) {
		m_processview = new Processview;
	}
	m_sysInfo->m_ProcessView = m_processview;
}

Sic_ProcessView::~Sic_ProcessView()
{
	if (m_processview) {
		delete m_processview;
		m_processview = nullptr;
	}
	m_sysInfo->m_ProcessView = nullptr;
}

void Sic_ProcessView::Execute(void)
{
	GetProcessDetils();
}

std::string Sic_ProcessView::GetErrors(void)
{
	return std::string();
}

void Sic_ProcessView::GetProcessDetils(void)
{
}

Sic_ProcessView* Sic_ProcessView::getInstance()
{
	m_mutex_process->lock();
	if (instance == nullptr) {
		instance = new Sic_ProcessView;
	}
	m_mutex_process->unlock();
	return instance;
}
