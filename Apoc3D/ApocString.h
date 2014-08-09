#pragma once
#ifndef APOC3D_STRING_H
#define APOC3D_STRING_H

#include "Common.h"

namespace Apoc3D
{
	template <typename CharType>
	class StringBase
	{
		static const int LocalBufferSize = 24;
		static const int LocalBufferLength = LocalBufferSize / sizeof(CharType);

		static_assert((LocalBufferSize % sizeof(CharType)) == 0, "");
		static_assert(std::is_trivially_copyable<CharType>::value, "");

	public:


		void Resize(int32 newCapacity)
		{
			if (newCapacity > LocalBufferLength)
			{
				CharType* buf = new CharType[newCapacity];
				CopyTo(buf);

				if (isAllocated())
				{
					delete m_content.allocatedBuffer;
				}

				m_content.allocatedBuffer = buf;

				m_capacity = newCapacity;
			}
		}

		CharType* begin() { return c_str(); }
		CharType* end() { return c_str() + m_length; }

		CharType* c_str() const { return isAllocated() ? m_content.allocatedBuffer : m_content.localBuffer; }

	private:
		bool isAllocated() const { return m_capacity > LocalBufferLength; }

		
		void EnsureCapacity() { EnsureCapacity(1); }

		void EnsureCapacity(int32 more)
		{
			int32 finalLength = m_length + more + 1;

			int32 capacity = m_capacity;
			if (finalLength >= m_capacity)
			{
				int32 nextSize = capacity * 2;
				if (more > LocalBufferLength &&
					nextSize < finalLength)
					nextSize = finalLength;

				Resize(nextSize);
			}
		}

		void CopyTo(CharType* dest)
		{
			memcpy(dest, c_str(), (m_length + 1)*sizeof(CharType));
		}
		

		union
		{
			CharType localBuffer[LocalBufferLength];

			CharType* allocatedBuffer;
		} m_content;

		int m_length = 0;
		int m_capacity = LocalBufferLength;

	};
}

#endif