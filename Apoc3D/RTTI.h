#pragma once

#define _RTTI_StaticTypeId static uintptr getTypeID() { static char dummy; return (uintptr)&dummy; }
#define _RTTI_Upcaster \
	template <typename T> \
	T* Upcast() const \
	{ \
		if (CheckRuntimeType(T::getTypeID())) \
			return (T*)this; \
		return nullptr; \
	}

#define RTTI_UpcastableBase \
	_RTTI_Upcaster \
	virtual bool CheckRuntimeType(uint32 id) const { return false; }

#define RTTI_UpcastableDerived(ParentType) \
	_RTTI_StaticTypeId \
	virtual bool CheckRuntimeType(uint32 id) const { return id == getTypeID() || ParentType::CheckRuntimeType(id); }
