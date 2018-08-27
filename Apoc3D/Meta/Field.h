#pragma once
#ifndef APOC3D_FIELDDATA_H
#define APOC3D_FIELDDATA_H

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

#include "apoc3d/IOLib/TaggedData.h"

namespace Apoc3D
{
	template <typename ClassType>
	struct DictionaryFieldInterface
	{
		virtual void Read(ClassType* inst, IO::TaggedDataReader* data) = 0;
		virtual void Write(ClassType* inst, IO::TaggedDataWriter* data) = 0;
	};

	template <typename ClassType, typename T>
	class DictionaryDataField : public DictionaryFieldInterface<ClassType>
	{
	public:
		DictionaryDataField(const IO::TaggedDataKey& key, T ClassType::* ptr)
			: m_key(key), m_member(ptr) { }

		void Read(ClassType* inst, IO::TaggedDataReader* data) override
		{
			data->TryGetAuto(m_key, inst->*m_member);
		}
		void Write(ClassType* inst, IO::TaggedDataWriter* data) override
		{
			data->AddAuto(m_key, inst->*m_member);
		}

	private:
		IO::TaggedDataKey m_key;
		T ClassType::* m_member;
	};

	template <typename ClassType>
	class DictionaryDataFieldStorage
	{
		static const int32 StorageSize = sizeof(DictionaryDataField<ClassType, int32>);
	public:
		typedef DictionaryFieldInterface<ClassType> InterfaceType;

		template <typename T>
		DictionaryDataFieldStorage(DictionaryDataField<ClassType, T>& fld)
		{
			static_assert(sizeof(m_storage) >= sizeof(fld), "Can't hold object");
			new (m_storage)DictionaryDataField<ClassType, T>(fld);
		}

		InterfaceType* operator->() const { return (InterfaceType*)m_storage; }

	private:
		char m_storage[StorageSize];
	};

#define DICT_FIELD(classType, exp) DictionaryDataField<classType, decltype(exp)>({#exp, std::integral_constant<uint32, Utility::FNVHash32Const(#exp)>::value }, &classType::exp)

}

#endif
