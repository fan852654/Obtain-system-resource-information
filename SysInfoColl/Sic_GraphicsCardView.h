#pragma once
#ifndef SIC_GRAPHICSCARDVIEW_H
#define SIC_GRAPHICSCARDVIEW_H

#include "IStrategy.h"
#include "DllFuncDef.h"

static std::mutex* m_mutex_graphics = new std::mutex;
static PGraphicsCardview m_graphicsview = nullptr;

class Sic_GraphicsCardView :IStrategy
{
public:
	static Sic_GraphicsCardView* instance;
	static Sic_GraphicsCardView* getInstance();
	Sic_GraphicsCardView();
	~Sic_GraphicsCardView();

	void Execute(void);
	std::string GetErrors(void);

private:
	void GetGpuInfoByNvml(void);
protected:
};
Sic_GraphicsCardView* Sic_GraphicsCardView::instance = nullptr;

#endif
