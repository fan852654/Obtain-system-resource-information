#pragma once
#ifndef DATACOMM_H

#define DATACOMM_H
#include <string>
#include <iostream>
class Data_Comm
{
public:
	Data_Comm();
	~Data_Comm();

	static std::string ws2s(const std::wstring& ws);
private:

};

#endif