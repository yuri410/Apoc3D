#pragma once
#ifndef APOC3D_EVENTDELEGATE_H
#define APOC3D_EVENTDELEGATE_H
/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "Collections/LinkedList.h"

using namespace Apoc3D::Collections;

namespace Apoc3D
{
	class EventDelegate0
	{
	public:
		typedef fastdelegate::FastDelegate0<void> DeleType;

		EventDelegate0() : m_locked(false), UseDestructProtection(false) { }
		~EventDelegate0() { assert(!m_locked); }


		void Bind(DeleType& v) { m_delegates.PushBack(v); }
		void Bind(void (*function_to_bind)() ) { Bind(DeleType(function_to_bind)); }
		template < class X, class Y >
		void Bind(Y *pthis, void (X::* function_to_bind)() ) { Bind(DeleType(pthis, function_to_bind)); }
		template < class X, class Y >
		void Bind(const Y *pthis, void (X::* function_to_bind)() const ) { Bind(DeleType(pthis, function_to_bind)); }


		void Unbind(DeleType& v) { assert(!m_locked); m_delegates.Remove(v); }
		void Unbind(void (*function_to_bind)() ) { Unbind(DeleType(function_to_bind)); }
		template < class X, class Y >
		void Unbind(Y *pthis, void (X::* function_to_bind)() ) { Unbind(DeleType(pthis, function_to_bind)); }
		template < class X, class Y >
		void Unbind(const Y *pthis, void (X::* function_to_bind)() const ) { Unbind(DeleType(pthis, function_to_bind)); }


		void Reset() { assert(!m_locked); m_delegates.Clear(); }

		void Invoke()
		{
			if (m_delegates.getCount())
			{
				if (UseDestructProtection)
				{
					Apoc3D::Collections::LinkedList<DeleType> copy = m_delegates;
					for (Apoc3D::Collections::LinkedList<DeleType>::Iterator iter = copy.Begin(); iter != copy.End(); ++iter)
						(*iter)();
				}
				else
				{
					assert(!m_locked);
					m_locked = true;
					for (Apoc3D::Collections::LinkedList<DeleType>::Iterator iter = m_delegates.Begin(); iter != m_delegates.End(); ++iter)
						(*iter)();
					m_locked = false;
				}
			}
		}

		int getCount() const { return m_delegates.getCount(); }

		bool UseDestructProtection;
	private:
		Apoc3D::Collections::LinkedList<DeleType> m_delegates;
		bool m_locked;
	};


	template<typename Arg1Type>
	class EventDelegate1
	{
	public:
		typedef fastdelegate::FastDelegate1<Arg1Type, void> DeleType;

		EventDelegate1() : m_locked(false), UseDestructProtection(false) { }
		~EventDelegate1() { assert(!m_locked); }

		void Bind(DeleType& v) { m_delegates.PushBack(v); }
		void Bind(void (*function_to_bind)(Arg1Type p1) ) { Bind(DeleType(function_to_bind)); }
		template < class X, class Y >
		void Bind(Y *pthis, void (X::* function_to_bind)(Arg1Type p1) ) { Bind(DeleType(pthis, function_to_bind)); }
		template < class X, class Y >
		void Bind(const Y *pthis, void (X::* function_to_bind)(Arg1Type p1) const ) { Bind(DeleType(pthis, function_to_bind)); }


		void Unbind(DeleType& v) { assert(!m_locked); m_delegates.Remove(v); }
		void Unbind(void (*function_to_bind)(Arg1Type p1) ) { Unbind(DeleType(function_to_bind)); }
		template < class X, class Y >
		void Unbind(Y *pthis, void (X::* function_to_bind)(Arg1Type p1) ) { Unbind(DeleType(pthis, function_to_bind)); }
		template < class X, class Y >
		void Unbind(const Y *pthis, void (X::* function_to_bind)(Arg1Type p1) const ) { Unbind(DeleType(pthis, function_to_bind)); }


		void Reset() { assert(!m_locked); m_delegates.Clear(); }

		void Invoke(Arg1Type a1)
		{
			if (m_delegates.getCount())
			{
				if (UseDestructProtection)
				{
					Apoc3D::Collections::LinkedList<DeleType> copy = m_delegates;
					for (Apoc3D::Collections::LinkedList<DeleType>::Iterator iter = copy.Begin(); iter != copy.End(); ++iter)
						(*iter)(a1);
				}
				else
				{
					assert(!m_locked);
					m_locked = true;
					for (Apoc3D::Collections::LinkedList<DeleType>::Iterator iter = m_delegates.Begin(); iter != m_delegates.End(); ++iter)
						(*iter)(a1);
					m_locked = false;
				}
			}
		}

		int getCount() const { return m_delegates.getCount(); }

		bool UseDestructProtection;
	private:
		Apoc3D::Collections::LinkedList<DeleType> m_delegates;
		bool m_locked;
	};

	template<typename Arg1Type, typename Arg2Type>
	class EventDelegate2
	{
	public:
		typedef fastdelegate::FastDelegate2<Arg1Type, Arg2Type, void> DeleType;
		
		EventDelegate2() : m_locked(false), UseDestructProtection(false) { }
		~EventDelegate2() { assert(!m_locked); }

		void Bind(DeleType& v) { m_delegates.PushBack(v); }
		void Bind(void (*function_to_bind)(Arg1Type p1, Arg2Type p2) ) { Bind(DeleType(function_to_bind)); }
		template < class X, class Y >
		void Bind(Y *pthis, void (X::* function_to_bind)(Arg1Type p1, Arg2Type p2) ) { Bind(DeleType(pthis, function_to_bind)); }
		template < class X, class Y >
		void Bind(const Y *pthis, void (X::* function_to_bind)(Arg1Type p1, Arg2Type p2) const ) { Bind(DeleType(pthis, function_to_bind)); }


		void Unbind(DeleType& v) { assert(!m_locked); m_delegates.Remove(v); }
		void Unbind(void (*function_to_bind)(Arg1Type p1, Arg2Type p2) ) { Unbind(DeleType(function_to_bind)); }
		template < class X, class Y >
		void Unbind(Y *pthis, void (X::* function_to_bind)(Arg1Type p1, Arg2Type p2) ) { Unbind(DeleType(pthis, function_to_bind)); }
		template < class X, class Y >
		void Unbind(const Y *pthis, void (X::* function_to_bind)(Arg1Type p1, Arg2Type p2) const ) { Unbind(DeleType(pthis, function_to_bind)); }


		void Reset() { assert(!m_locked); m_delegates.Clear(); }

		void Invoke(Arg1Type a1, Arg2Type a2)
		{
			if (m_delegates.getCount())
			{
				if (UseDestructProtection)
				{
					Apoc3D::Collections::LinkedList<DeleType> copy = m_delegates;
					for (Apoc3D::Collections::LinkedList<DeleType>::Iterator iter = copy.Begin(); iter != copy.End(); ++iter)
						(*iter)(a1, a2);
				}
				else
				{
					assert(!m_locked);
					m_locked = true;
					for (Apoc3D::Collections::LinkedList<DeleType>::Iterator iter = m_delegates.Begin(); iter != m_delegates.End(); ++iter)
						(*iter)(a1, a2);
					m_locked = false;
				}
			}
		}

		int getCount() const { return m_delegates.getCount(); }

		bool UseDestructProtection;
	private:
		Apoc3D::Collections::LinkedList<DeleType> m_delegates;
		bool m_locked;
	};

	template<typename Arg1Type, typename Arg2Type, typename Arg3Type>
	class EventDelegate3
	{
	public:
		typedef fastdelegate::FastDelegate3<Arg1Type, Arg2Type, Arg3Type, void> DeleType;
		
		EventDelegate3() : m_locked(false), UseDestructProtection(false) { }
		~EventDelegate3() { assert(!m_locked); }

		void Bind(DeleType& v) { m_delegates.PushBack(v); }
		void Bind(void (*function_to_bind)(Arg1Type p1, Arg2Type p2, Arg3Type p3) ) { Bind(DeleType(function_to_bind)); }
		template < class X, class Y >
		void Bind(Y *pthis, void (X::* function_to_bind)(Arg1Type p1, Arg2Type p2, Arg3Type p3) ) { Bind(DeleType(pthis, function_to_bind)); }
		template < class X, class Y >
		void Bind(const Y *pthis, void (X::* function_to_bind)(Arg1Type p1, Arg2Type p2, Arg3Type p3) const ) { Bind(DeleType(pthis, function_to_bind)); }


		void Unbind(DeleType& v) { assert(!m_locked); m_delegates.Remove(v); }
		void Unbind(void (*function_to_bind)(Arg1Type p1, Arg2Type p2, Arg3Type p3) ) { Unbind(DeleType(function_to_bind)); }
		template < class X, class Y >
		void Unbind(Y *pthis, void (X::* function_to_bind)(Arg1Type p1, Arg2Type p2, Arg3Type p3) ) { Unbind(DeleType(pthis, function_to_bind)); }
		template < class X, class Y >
		void Unbind(const Y *pthis, void (X::* function_to_bind)(Arg1Type p1, Arg2Type p2, Arg3Type p3) const ) { Unbind(DeleType(pthis, function_to_bind)); }


		void Reset() { assert(!m_locked); m_delegates.Clear(); }

		void Invoke(Arg1Type a1, Arg2Type a2, Arg3Type a3)
		{
			if (m_delegates.getCount())
			{
				if (UseDestructProtection)
				{
					Apoc3D::Collections::LinkedList<DeleType> copy = m_delegates;
					for (Apoc3D::Collections::LinkedList<DeleType>::Iterator iter = copy.Begin(); iter != copy.End(); ++iter)
						(*iter)(a1, a2, a3);
				}
				else
				{
					assert(!m_locked);
					m_locked = true;
					for (Apoc3D::Collections::LinkedList<DeleType>::Iterator iter = m_delegates.Begin(); iter != m_delegates.End(); ++iter)
						(*iter)(a1, a2, a3);
					m_locked = false;
				}
			}
		}

		int getCount() const { return m_delegates.getCount(); }

		bool UseDestructProtection;
	private:
		Apoc3D::Collections::LinkedList<DeleType> m_delegates;
		bool m_locked;
	};

	template<typename Arg1Type, typename Arg2Type, typename Arg3Type, typename Arg4Type>
	class EventDelegate4
	{
	public:
		typedef fastdelegate::FastDelegate4<Arg1Type, Arg2Type, Arg3Type, Arg4Type, void> DeleType;

		EventDelegate4() : m_locked(false), UseDestructProtection(false) { }
		~EventDelegate4() { assert(!m_locked); }

		void Bind(DeleType& v) { m_delegates.PushBack(v); }
		void Bind(void (*function_to_bind)(Arg1Type p1, Arg2Type p2, Arg3Type p3, Arg4Type p4) ) { Bind(DeleType(function_to_bind)); }
		template < class X, class Y >
		void Bind(Y *pthis, void (X::* function_to_bind)(Arg1Type p1, Arg2Type p2, Arg3Type p3, Arg4Type p4) ) { Bind(DeleType(pthis, function_to_bind)); }
		template < class X, class Y >
		void Bind(const Y *pthis, void (X::* function_to_bind)(Arg1Type p1, Arg2Type p2, Arg3Type p3, Arg4Type p4) const ) { Bind(DeleType(pthis, function_to_bind)); }


		void Unbind(DeleType& v) { assert(!m_locked); m_delegates.Remove(v); }
		void Unbind(void (*function_to_bind)(Arg1Type p1, Arg2Type p2, Arg3Type p3, Arg4Type p4) ) { Unbind(DeleType(function_to_bind)); }
		template < class X, class Y >
		void Unbind(Y *pthis, void (X::* function_to_bind)(Arg1Type p1, Arg2Type p2, Arg3Type p3, Arg4Type p4) ) { Unbind(DeleType(pthis, function_to_bind)); }
		template < class X, class Y >
		void Unbind(const Y *pthis, void (X::* function_to_bind)(Arg1Type p1, Arg2Type p2, Arg3Type p3, Arg4Type p4) const ) {  Unbind(DeleType(pthis, function_to_bind)); }


		void Reset() { assert(!m_locked); m_delegates.Clear(); }

		void Invoke(Arg1Type a1, Arg2Type a2, Arg3Type a3, Arg4Type a4)
		{
			if (m_delegates.getCount())
			{
				if (UseDestructProtection)
				{
					Apoc3D::Collections::LinkedList<DeleType> copy = m_delegates;
					for (Apoc3D::Collections::LinkedList<DeleType>::Iterator iter = copy.Begin(); iter != copy.End(); ++iter)
						(*iter)(a1, a2, a3, a4);
				}
				else
				{
					assert(!m_locked);
					m_locked = true;
					for (Apoc3D::Collections::LinkedList<DeleType>::Iterator iter = m_delegates.Begin(); iter != m_delegates.End(); ++iter)
						(*iter)(a1, a2, a3, a4);
					m_locked = false;
				}
			}
		}

		int getCount() const { return m_delegates.getCount(); }

		bool UseDestructProtection;
	private:
		Apoc3D::Collections::LinkedList<DeleType> m_delegates;
		bool m_locked;
	};

}

#endif