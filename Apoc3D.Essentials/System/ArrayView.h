#pragma once

#include "apoc3d.Essentials/EssentialCommon.h"

namespace Apoc3D
{
	template <typename T>
	class ArrayView
	{
	public:
		ArrayView(const T* ptr, int32 count)
			: m_elements(const_cast<T*>(ptr))
			, m_count(count)
		{ }

		template <int N>
		ArrayView(const T(&arr)[N])
			: m_elements(const_cast<T*>(arr))
			, m_count(N)
		{ }

		ArrayView(const List<T>& list)
			: m_elements(list.getElements())
			, m_count(list.getCount())
		{ }

		const T* begin() const { return m_elements; }
		const T* end() const { return m_elements + m_count; }

		int32 getCount() const { return m_count; }

		const T& operator[](int32 idx) const { return m_elements[idx]; }
	private:
		const T* m_elements;
		int32 m_count;
	};
}
