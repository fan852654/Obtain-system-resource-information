#pragma once
#ifndef SIC_TYPE_PROCESSVIEW_H
#define SIC_TYPE_PROCESSVIEW_H

#include "sic_defines.h"
namespace SIC_TYPE_PROCESSVIEW {
	enum ProcessViewStatus {
		PVS_NO_ERROR = 0,
	};

	typedef struct _Processview {
		_Processview() {
			Status = ProcessViewStatus::PVS_NO_ERROR;
		}
		~_Processview() {
		}
		int Status;
	}Processview;
	typedef Processview* PProcessview;
}

#endif // !SIC_TYPE_PROCESSVIEW_H
