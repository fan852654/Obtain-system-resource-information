// SysInfoCollecter.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <time.h>
typedef struct _GENERAL_MEMORY_BLOCK {
	void* Memory_Ptr;
	_GENERAL_MEMORY_BLOCK* next;
	void* _trunk;
}GENERAL_MEMORY_BLOCK, * PGENERAL_MEMORY_BLOCK;
int main()
{
    time_t ts = time(NULL);
	PGENERAL_MEMORY_BLOCK a = new GENERAL_MEMORY_BLOCK;
	PGENERAL_MEMORY_BLOCK b = new GENERAL_MEMORY_BLOCK;
	b->Memory_Ptr = NULL;
	b->next = NULL;
	b->_trunk = NULL;
	a->Memory_Ptr = malloc(16);
	a->next = b;
	a->_trunk = b;

	void* p = &a->Memory_Ptr;
	void* p1 = &a->next;
	void* p2 = &a->_trunk;
	int siz = sizeof(void*);
	int siz1 = sizeof(_GENERAL_MEMORY_BLOCK*);
	void** ptr = (void**)((int*)p + (siz + siz1) / 4);
	PGENERAL_MEMORY_BLOCK dd = (PGENERAL_MEMORY_BLOCK)(*ptr);
	return 0;
}