#pragma once
#ifndef DLLFUNCDEF_H
#define DLLFUNCDEF_H

#include <Windows.h>
#define KERNEL32_DLL	"kernel32"


typedef BOOL(WINAPI* LPFN_GLPI)(
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION,
	PDWORD);

#endif // !DLLFUNCDEF_H
