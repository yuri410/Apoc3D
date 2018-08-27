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
		using InvokerType = Ret(*)(void*, Args&&...);
		using RawFunctionType = Ret(*)(Args...);

	public:
		FunctorReference() { }

		FunctorReference(std::nullptr_t) { }

		template <typename F>
		FunctorReference(const F& lambda)
		{
			m_instance = const_cast<F*>(&lambda);
			m_invoker = &Invoker_ConstMethod<F, &F::operator()> ;
		}

		template <typename F> // prevent const in T
		FunctorReference(typename std::enable_if<std::is_const<F>::value, F>::type& lambda)
		{
			m_instance = const_cast<F*>(&lambda);
			m_invoker = &Invoker_Method < F, &F::operator() > ;
		} 

		FunctorReference(RawFunctionType functionPtr)
			: m_rawFunction(functionPtr) { }


		template <class ClassType, Ret(ClassType::*method_ptr)(Args...)>
		FunctorReference& Bind(ClassType* ct)
		{
			m_instance = const_cast<ClassType*>(ct);
			m_invoker = &Invoker_Method<ClassType, method_ptr>;
			return *this;
		}

		template <class ClassType, Ret(ClassType::*method_ptr)(Args...) const>
		FunctorReference& Bind(const ClassType* ct)
		{
			m_instance = const_cast<ClassType*>(ct);
			m_invoker = &Invoker_ConstMethod<ClassType, method_ptr> ;
			return *this;
		}

		Ret operator()(Args... args) const
		{
			if (m_rawFunction)
				return m_rawFunction(std::forward<Args>(args)...);

			return m_invoker(m_instance, std::forward<Args>(args)...);
		}

		bool isNull() const { return m_instance == nullptr && m_rawFunction == nullptr; }
	private:
		RawFunctionType m_rawFunction = nullptr;
		void* m_instance = nullptr;
		InvokerType m_invoker = 0;

		template <class ClassType, Ret(ClassType::*method_ptr)(Args...)>
		static Ret Invoker_Method(void* object_ptr, Args&&... args)
		{
			return (static_cast<ClassType*>(object_ptr)->*method_ptr)(std::forward<Args>(args)...);
		}

		template <class ClassType, Ret(ClassType::*method_ptr)(Args...) const>
		static Ret Invoker_ConstMethod(void* object_ptr, Args&&... args)
		{
			return (static_cast<const ClassType*>(object_ptr)->*method_ptr)(std::forward<Args>(args)...);
		}
	};

}

#endif