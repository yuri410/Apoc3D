#pragma once
#ifndef APOC3D_EVENTDELEGATE_H
#define APOC3D_EVENTDELEGATE_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "apoc3d/Collections/List.h"

using namespace Apoc3D::Collections;

namespace Apoc3D
{
	template <typename ... Args>
	class EventDelegate
	{
	public:
		typedef fastdelegate::FastDelegate<void(Args...)> DeleType;
		typedef Apoc3D::Collections::List <  DeleType > ListType;

		EventDelegate() { }
		~EventDelegate() { assert(!m_locked); }

		void Bind(const DeleType& v) { m_delegates.Add(v); }

		void Bind(void(*function_to_bind)(Args...)) { Bind(DeleType(function_to_bind)); }

		template < class X, class Y >
		void Bind(Y *pthis, void (X::* function_to_bind)(Args...)) { Bind(DeleType(pthis, function_to_bind)); }

		template < class X, class Y >
		void Bind(const Y *pthis, void (X::* function_to_bind)(Args...) const) { Bind(DeleType(pthis, function_to_bind)); }


		void Unbind(const DeleType& v) { assert(!m_locked); m_delegates.Remove(v); }

		void Unbind(void(*function_to_bind)(Args...)) { Unbind(DeleType(function_to_bind)); }

		template < class X, class Y >
		void Unbind(Y *pthis, void (X::* function_to_bind)(Args...)) { Unbind(DeleType(pthis, function_to_bind)); }

		template < class X, class Y >
		void Unbind(const Y *pthis, void (X::* function_to_bind)(Args...) const) { Unbind(DeleType(pthis, function_to_bind)); }


		void Reset() { assert(!m_locked); m_delegates.Clear(); }

		EventDelegate& operator +=(const DeleType& v) { Bind(v); return *this; }
		EventDelegate& operator +=(void(*function_to_bind)(Args...)) { Bind(function_to_bind); return *this; }

		EventDelegate& operator -=(const DeleType& v) { Unbind(v); return *this; }
		EventDelegate& operator -=(void(*function_to_bind)(Args...)) { Unbind(function_to_bind); return *this; }

		//void operator()() { Invoke(); }

		void Invoke(Args... args)
		{
			if (m_delegates.getCount())
			{
				if (UseDestructProtection)
				{
					ListType copy = m_delegates;
					for (const auto& e : copy)
						e(args...);
				}
				else
				{
					assert(!m_locked);
					m_locked = true;
					for (const auto& e : m_delegates)
						e(args...);
					m_locked = false;
				}
			}
		}

		int getCount() const { return m_delegates.getCount(); }

		bool UseDestructProtection = false;

	private:
		ListType m_delegates;
		bool m_locked = false;
	};
}

#endif