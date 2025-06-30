//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef LIBMMD_BASE_SINGLETON_H_
#define LIBMMD_BASE_SINGLETON_H_

#include <functional>
#include <mutex>

namespace libmmd
{
	class SingletonFinalizer
	{
	public:
		using FinalizerFunc = std::function<void()>;

		static void AddFinalizer(const FinalizerFunc& finalizer);
		static void Finalize();
	};

	template <typename T>
	class Singleton final
	{
	public:
		static T* Get()
		{
			std::call_once(m_initFlag, Create);
			return m_instance;
		}

	private:
		static void Create()
		{
			m_instance = new T();
			SingletonFinalizer::AddFinalizer(&Singleton::Destroy);
		}

		static void Destroy()
		{
			delete m_instance;
			m_instance = nullptr;
		}

		static std::once_flag	m_initFlag;
		static T*				m_instance;
	};
	template <typename T> std::once_flag Singleton<T>::m_initFlag;
	template <typename T> T* Singleton<T>::m_instance = nullptr;
}

#endif // !LIBMMD_BASE_SINGLETON_H_
