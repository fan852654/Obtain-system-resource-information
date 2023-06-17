#pragma once
#ifndef SPINLOCKER_H
#define SPINLOCKER_H

#include <atomic>

class SpinLocker
{
public:
	SpinLocker() :_flag(false) {};
	~SpinLocker() {};
	void lock() {
		bool expect = false; while (!_flag.compare_exchange_weak(expect, true)) { expect = false; }
	}
	void unlock() { _flag.store(false); }
private:
	std::atomic<bool> _flag;
};

#endif // !SPINLOCKER_H
