#pragma once
#ifndef APOC3D_FUNCTORREFERENCE_H
#define APOC3D_FUNCTORREFERENCE_H

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

namespace Apoc3D
{
	template <typename T>
	class FunctorReference;  // no impl

	template <class Ret, class ...Args>
	class FunctorReference < Ret(Args...) >
	{
		using InvokerType = Ret(*)(void*, Args&& ...);

		using RawFunctionType = Ret(*)(Args...);

		template <class C>
		struct MemberFunctorType
		{
			C* m_obj;
			Ret(C::* m_func)(Args...);
		};

		template <class C>
		struct MemberFunctorConstType
		{
			const C* m_obj;
			Ret(C::* m_func)(Args...) const;
		};

	public:
		FunctorReference() { }

		FunctorReference(std::nullptr_t) { }

		template <typename F>
		FunctorReference(const F& lambda)
		{
			m_invoker = &InvokerCT_ConstMember< F, & F::operator() >;
			m_instance = const_cast<F*>(&lambda);
		}

		template <typename F> // prevent const in T
		FunctorReference(typename std::enable_if<!std::is_const<F>::value, F>::type& lambda)
		{
			m_invoker = &InvokerCT_Member< F, & F::operator() >;
			m_instance = const_cast<F*>(&lambda);
		}

		FunctorReference(RawFunctionType functionPtr)
		{
			m_rawFunction = functionPtr;
		}

		template <class ClassType>
		FunctorReference(ClassType* ct, Ret(ClassType::* func)(Args...))
		{
			static_assert(std::is_trivial<MemberFunctorType<ClassType>>::value, "");
			static_assert(sizeof(m_mbrFunctor) == sizeof(MemberFunctorType<ClassType>), "");

			m_invoker = &Invoker_Member< MemberFunctorType<ClassType> >;
			*reinterpret_cast<MemberFunctorType<ClassType>*>(m_mbrFunctor) = { ct, func };
			m_instance = m_mbrFunctor;
		}

		template <class ClassType>
		FunctorReference(const ClassType* ct, Ret(ClassType::* func)(Args...) const)
		{
			static_assert(std::is_trivial<MemberFunctorConstType<ClassType>>::value, "");
			static_assert(sizeof(m_mbrFunctor) == sizeof(MemberFunctorConstType<ClassType>), "");

			m_invoker = &Invoker_Member< MemberFunctorConstType<ClassType> >;
			*reinterpret_cast<MemberFunctorConstType<ClassType>*>(m_mbrFunctor) = { ct, func };
			m_instance = m_mbrFunctor;
		}

		Ret operator()(Args... args) const
		{
			if (m_rawFunction)
				return m_rawFunction(std::forward<Args>(args)...);

			return m_invoker(m_instance, std::forward<Args>(args)...);
		}

		bool isNull() const { return m_invoker == nullptr && m_rawFunction == nullptr; }

		explicit operator bool() const noexcept { return m_invoker || m_rawFunction; }
		
		bool operator==(std::nullptr_t) const noexcept { return isNull(); }
		bool operator!=(std::nullptr_t) const noexcept { return !isNull(); }

	private:
		struct Dummy {};

		InvokerType m_invoker = nullptr;
		RawFunctionType m_rawFunction = nullptr;

		void* m_instance = nullptr;
		char  m_mbrFunctor[sizeof(MemberFunctorType<Dummy>)];

		template <class ClassType, Ret(ClassType::* func)(Args...)>
		static Ret InvokerCT_Member(void* instance, Args&& ... args)
		{
			return (static_cast<ClassType*>(instance)->*func)(std::forward<Args>(args)...);
		}

		template <class ClassType, Ret(ClassType::* func)(Args...) const>
		static Ret InvokerCT_ConstMember(void* instance, Args&& ... args)
		{
			return (static_cast<const ClassType*>(instance)->*func)(std::forward<Args>(args)...);
		}

		template <typename FunctorType>
		static Ret Invoker_Member(void* instance, Args&& ... args)
		{
			return (static_cast<FunctorType*>(instance)->m_obj->*
					static_cast<FunctorType*>(instance)->m_func)(std::forward<Args>(args)...);
		}

		//static Ret Invoker_Function(void* instance, Args&& ... args)
		//{
		//	return static_cast<RawFunctionType>(instance)(std::forward<Args>(args)...);
		//}

	};

}

#endif