#pragma once
#ifndef DATACOMM_H

#define DATACOMM_H
#include <windows.h>
#include <string>
#include <iostream>
#include <tchar.h>
#include <malloc.h>
class Data_Comm
{
public:
	Data_Comm();
	~Data_Comm();

	static std::string ws2s(const std::wstring& ws);
	static DWORD CountSetBits(ULONG_PTR bitMask);
private:

};

#endif