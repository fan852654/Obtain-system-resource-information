// SysInfoCollecter.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "SysInfoColl.h"
#include <iostream>
#include <time.h>
#pragma comment(lib, "D:\\git\\Obtain-system-resource-information\\x64\\Debug\\SysInfoColl.lib")
int main()
{
    time_t ts = time(NULL);
    SysInfoColl sc;
    sc.GetSystemEventLog(ts);
}