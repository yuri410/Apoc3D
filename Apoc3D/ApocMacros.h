#pragma once

// check without complete type
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
class _InheritCheck < void, Derived >
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


#define RTTI_BASE \
	public: \
		static uintptr _getTypeID() { static char dummy; return (uintptr)&dummy; } \
		virtual bool CheckRuntimeType(uintptr id) const { return id == _getTypeID(); }

#define RTTI_DERIVED(Type, ParentType) \
	private: \
		static_assert(_InheritCheck<ParentType, Type>::isDerivedFrom, "BaseType is not the base."); \
	public: \
		static uintptr _getTypeID() { static char dummy; return (uintptr)&dummy; } \
		virtual bool CheckRuntimeType(uintptr id) const override { return id == Type::_getTypeID() || ParentType::CheckRuntimeType(id); }


#define SINGLETON_DECL(T) \
	 public: \
		static void Initialize(); static void Finalize(); \
		static T& getSingleton() \
		{ \
			assert(s_initialized); \
			return reinterpret_cast<T&>(s_instance); \
		}  \
		static bool isInitialized() { return s_initialized; } \
	private: \
		static bool s_initialized; \
		static char s_instance[];


#define SINGLETON_IMPL(T)  \
	char T::s_instance[sizeof(T)]; \
	bool T::s_initialized = false; \
	void T::Initialize() \
	{ \
		assert(!s_initialized); \
		new(s_instance)T(); \
		s_initialized = true; \
	} \
	void T::Finalize() \
	{ \
		assert(s_initialized); \
		getSingleton().~T(); \
		memset(s_instance, 0, sizeof(T)); \
		s_initialized = false; \
	}


#if defined(_MSC_VER)

#define FORCE_INLINE __forceinline
#define NO_INLINE __declspec(noinline)

#elif defined(__GCC__)

#define FORCE_INLINE  __attribute__((always_inline))
#define NO_INLINE __attribute__((noinline))

#else

#define FORCE_INLINE
#define NO_INLINE

#endif


#define DELETE_AND_NULL( ptr ) { delete ptr; ptr = nullptr; }
