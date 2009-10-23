

namespace Apoc {

	#if defined( _MSC_VER )
	#   define APOC_COMPILER_MSVC
	#elif defined( __GNUC__ )
	#   define APOC_COMPILER_GNUC
	#elif defined( __BORLANDC__ )
	#   define APOC_COMPILER_BCC
	#else
	#   pragma error " Abort! No known compiler. Abort!"
	#endif

	#ifdef _DEBUG
	#define DEBUG_MODE 1
	#else
	#defien DEBUG_MODE 0
	#endif

	#if defined( __WIN32__ ) || defined( _WIN32 )
	#   define APOC_PLATFORM_WIN32
	#elif defined( __APPLE_CC__)
	#   define APOC_PLATFORM_APPLE
	#else
	#   define APOC_PLATFORM_LINUX
	#endif

	#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) || defined(__ia64__) || defined(__s390__) || defined(__s390x__)
	#   define APOC_ARCH_64
	#else
	#   define APOC_ARCH_32
	#endif

	// Integer formats of fixed bit width
	typedef unsigned int uint32;
	typedef unsigned int uint;
	typedef unsigned short uint16;
	typedef unsigned short ushort;
	typedef unsigned char uint8;
	typedef unsigned char byte;


	// define uint64 type
	#ifdef APOC_COMPILER_MSVC
		typedef unsigned __int64 uint64;
	#else
		typedef unsigned long long uint64;
	#endif


}
}