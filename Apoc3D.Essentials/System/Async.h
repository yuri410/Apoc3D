#pragma once
#include "Optional.h"

namespace Apoc3D
{
	template <typename T, typename ET>
	struct AsyncShare
	{
		Optional<T> m_data;
		Optional<ET> m_error;
		condition_variable m_condVar;
		//int m_counter1 = 0;
		//int m_counter2 = 0;
		mutex m_mutex;

		void Set(const T& data)
		{
			m_mutex.lock();
			m_data = data;
			m_mutex.unlock();
			m_condVar.notify_one();
		}

		void Set(T&& data)
		{
			m_mutex.lock();
			m_data = std::move(data);
			m_mutex.unlock();
			m_condVar.notify_one();
		}

		void SetError(const ET& error)
		{
			m_mutex.lock();
			m_error = error;
			m_mutex.unlock();
			m_condVar.notify_one();
		}

		void SetError(ET&& error)
		{
			m_mutex.lock();
			m_error = std::move(error);
			m_mutex.unlock();
			m_condVar.notify_one();
		}

		void Clear()
		{
			LockGuard lock(m_mutex);
			m_data.Clear();
			m_error.Clear();

			//m_counter1++;
		}

		void Wait()
		{
			unique_lock<mutex> lock(m_mutex);
			m_condVar.wait(lock, [this]()
			{
				return m_data.isSet() || m_error.isSet();
			});
		}
		void Wait(uint32 ms)
		{
			unique_lock<mutex> lock(m_mutex);
			m_condVar.wait_for(lock, milliseconds(ms), [this]()
			{
				return m_data.isSet() || m_error.isSet();
			});
		}

		bool Poll()
		{
			LockGuard lock(m_mutex);
			return m_set;
		}

		//bool CheckCurrentAndUpdate()
		//{
		//	LockGuard lock(m_mutex);
		//	if (m_counter1 == m_counter2)
		//	{
		//		m_counter2++;
		//	}
		//}

		T* Get()
		{
			Wait();
			if (m_data.isSet())
				return &m_data.getContent();
			return nullptr;
		}

		ET* GetError()
		{
			Wait();
			if (m_error.isSet())
				return &m_error.getContent();
			return nullptr;
		}
	};

	template <typename T, typename ET>
	class AsyncPending
	{
		typedef AsyncShare<T, ET> ShareType;

		template <typename T, typename ET>
		friend class AsyncResult;
	public:
		AsyncPending()
			: m_result(std::make_shared<ShareType>())
		{ }

		AsyncPending(AsyncPending&& o)
			: m_result(std::move(o.m_result))
		{ }

		~AsyncPending() { }

		AsyncPending& operator=(AsyncPending&& o)
		{
			if (this != &o)
			{
				m_result = std::move(o.m_result);
			}
			return *this;
		}

		void Set(const T& data)	{ m_result->Set(data); }
		void Set(T&& data)		{ m_result->Set(std::forward<T>(data)); }
		
		void SetError(const ET& error)	{ m_result->SetError(error); }
		void SetError(ET&& error)		{ m_result->SetError(std::forward<T>(error)); } 

		void Clear() { m_result->Clear(); }
		void Wait()	 { m_result->Wait(); }

		bool IsAbandoned() const { return m_result.use_count() <= 1; }
	private:
		std::shared_ptr<ShareType> m_result;

	};

	template <typename T, typename ET>
	class AsyncResult
	{
		typedef AsyncShare<T, ET> ShareType;
		typedef AsyncPending<T, ET> PendingType;

	public:
		explicit AsyncResult(const PendingType& p)
			: m_result(p.m_result)
		{ }

		AsyncResult(AsyncResult&& o)
			: m_result(std::move(o.m_result))
		{ }

		~AsyncResult() { }

		AsyncResult& operator=(AsyncResult&& o)
		{
			if (this != &o)
			{
				m_result = std::move(o.m_result);
			}
			return *this;
		}

		void Wait()			 { m_result->Wait(); }
		void Wait(uint32 ms) { m_result->Wait(ms); }

		//bool Poll() { return m_result->Poll(); }

		T* Get()		{ return m_result->Get(); }
		ET* GetError()	{ return m_result->GetError(); }

	private:
		shared_ptr<ShareType> m_result;

	};
}