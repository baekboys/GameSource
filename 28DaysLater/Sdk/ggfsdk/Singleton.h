#pragma once

#include <cassert>

namespace ggf
{
	namespace oop
	{
template <typename T> class Singleton
{
	static T* ms_Singleton;
public:
	Singleton()
	{
		assert(!ms_Singleton);
		DWORD64 offset = (DWORD64)(T*)1 - (DWORD64)(Singleton<T>*)(T*)1;
		ms_Singleton = (T*)((DWORD64)this+offset);
	}
	~Singleton()
	{
		assert(ms_Singleton);
		ms_Singleton = 0;
	}
	static T& Get()
	{
		assert(ms_Singleton);
		return (*ms_Singleton);
	}
	static T* GetPtr()
	{
		assert(ms_Singleton);
		return (ms_Singleton);
	}
};

template <typename T> T* Singleton<T>::ms_Singleton = 0;
	}
}