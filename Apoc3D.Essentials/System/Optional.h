#pragma once

#include "Apoc3D.Essentials/EssentialCommon.h"

namespace Apoc3D
{
	template <typename T>
	class Optional final
	{
		static const int32 ContentSize = sizeof(T);
	public:
		Optional() { }

		Optional(const T& data)
			: m_isSet(true)
		{
			if (m_isSet)
				new (m_data)T(data);
		}

		Optional(T&& data)
			: m_isSet(true)
		{
			if (m_isSet)
				new (m_data)T(std::forward<T>(data));
		}

		Optional(const Optional& o)
			: m_isSet(o.m_isSet)
		{
			if (m_isSet)
				new (m_data)T(o.getContent());
		}

		Optional(Optional&& o)
			: m_isSet(o.m_isSet)
		{
			if (m_isSet)
				new (m_data)T(std::move(o.getContent()));
		}

		~Optional()
		{
			if (m_isSet)
			{
				getContent().~T();
				m_isSet = false;
			}
		}

		Optional& operator=(const Optional& o)
		{
			if (this != &o)
			{
				this->~Optional();
				new (this)Optional(o);
			}
			return *this;
		}

		Optional& operator=(Optional&& o)
		{
			if (this != &o)
			{
				this->~Optional();
				new (this)Optional(std::move(o));
			}
			return *this;
		}

		Optional& operator=(const T& data)
		{
			this->~Optional();
			new (this)Optional(data);
			return *this;
		}

		Optional& operator=(T&& data)
		{
			this->~Optional();
			new (this)Optional(std::forward<T>(data));
			return *this;
		}

		void Clear() 
		{
			this->~Optional();
		}

		bool isSet() const { return m_isSet; }
		T& getContent() { return *(T*)m_data; }
		const T& getContent() const { return *(T*)m_data; }

		const T& getOrDefault(const T& def) const { return m_isSet ? getContent() : def; }

		operator T() { return getContent(); }
		operator T() const { return getContent(); }

		bool operator ==(const Optional& o) const
		{
			if (!m_isSet)
				return m_isSet == o.m_isSet;
			return m_isSet == o.m_isSet && getContent() == o.getContent();
		}
		bool operator !=(const Optional& o) const { return !this->operator=(o); }

	private:
		char m_data[ContentSize];
		bool m_isSet = false;
	};

}