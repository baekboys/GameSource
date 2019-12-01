#pragma once

//#include <cassert>

namespace ggf
{
namespace oop
{
	/*template <typename T> class Singleton
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
	template <typename T> T* Singleton<T>::ms_Singleton = 0;*/

	template<typename T> class Singleton
	{
	private:
		static T * ms_pSingleton;

	public:
		Singleton()
		{
			//assert(NULL == Singleton<T>::ms_pSingleton);
			Singleton<T>::ms_pSingleton = (T*)this;
		}

		~Singleton()
		{
			Singleton<T>::ms_pSingleton = NULL;
		}

		static T* GetPtr()
		{
			return ms_pSingleton;
		}

		static T&  Get()
		{
			return *ms_pSingleton;
		}
	};
	template<typename T> T * Singleton<T>::ms_pSingleton = NULL;
}
}