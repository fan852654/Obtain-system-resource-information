#include "DataComm.h"

Data_Comm::Data_Comm()
{
}

Data_Comm::~Data_Comm()
{
}

std::string Data_Comm::ws2s(const std::wstring& ws)
{
	std::string curLocale = setlocale(LC_ALL, "");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = wcstombs(NULL, _Source, 0) + 1;
	char* _Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	wcstombs(_Dest, _Source, _Dsize);
	std::string result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}
