#pragma once

template <class Base, class Derived>
class _InheritCheck
{
	struct Temp 
	{
		static long long isBaseClass(void*);
		static char      isBaseClass(Base*);
	};
public:
	static const bool isDerivedFrom = sizeof(Temp::isBaseClass((Derived*)0)) == sizeof(char);
};

template <class Derived>
class _InheritCheck<void, Derived>
{
public:
	static const bool isDerivedFrom = false;
};

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

#define RTTI_UpcastableDerived(Type, ParentType) \
	public: \
	_RTTI_StaticTypeId \
	static_assert(_InheritCheck<ParentType, Type>::isDerivedFrom, "BaseType is not the base."); \
	virtual bool CheckRuntimeType(uint32 id) const { return id == getTypeID() || ParentType::CheckRuntimeType(id); }
