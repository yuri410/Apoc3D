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

// This is aka downcast. Base to Derived.
// Called upcast here to match naming of other libraries, and it is shorter.

template <typename Derived, typename Base>
Derived up_cast(Base* o)
{
	static_assert(std::is_pointer<Derived>::value, "up_cast: Type T needs to be pointer.");

	static_assert(std::is_const<std::remove_pointer<Derived>::type>::value == std::is_const<Base>::value && 
		std::is_volatile<std::remove_pointer<Derived>::type>::value == std::is_volatile<Base>::value
		, "up_cast: CV mismatch");

	static_assert(std::is_base_of<Base, std::remove_pointer<Derived>::type>::value, "Impossible up_cast");

	if (o == nullptr)
		return nullptr;

	if (o->CheckRuntimeType(std::remove_pointer<Derived>::type::_getTypeID()))
	{
		return (Derived)o;
	}
	return nullptr;
}

#define RTTI_UpcastableBase public: virtual bool CheckRuntimeType(uintptr id) const { return false; }

#define RTTI_UpcastableDerived(Type, ParentType) \
		static_assert(_InheritCheck<ParentType, Type>::isDerivedFrom, "BaseType is not the base."); \
	public: \
		static uintptr _getTypeID() { static char dummy; return (uintptr)&dummy; } \
		virtual bool CheckRuntimeType(uintptr id) const override { return id == _getTypeID() || ParentType::CheckRuntimeType(id); }
