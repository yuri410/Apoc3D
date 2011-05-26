/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/


#ifndef COMMON_H
#define COMMON_H

typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

typedef unsigned char byte;
typedef uint16 ushort;
typedef uint32 uint;
typedef uint64 ulong;
typedef unsigned long long BatchHandle;
typedef BatchHandle HashHandle;

//#include "..\lib\boost\thread\tss.hpp"
//#include "..\lib\boost\thread\recursive_mutex.hpp"
//#include "..\lib\boost\thread\condition.hpp"
//#include "..\lib\boost\thread\thread.hpp"
//
//#define DEFAULT_MUTEX_NAME mutex
//#define MUTEX mutable boost::recursive_mutex DEFAULT_MUTEX_NAME;
//#define LOCK_MUTEX boost::recursive_mutex::scoped_lock ogreAutoMutexLock(DEFAULT_MUTEX_NAME);
//#define MUTEX(name) mutable boost::recursive_mutex name;
//#define STATIC_MUTEX(name) static boost::recursive_mutex name;
//#define STATIC_MUTEX_INSTANCE(name) boost::recursive_mutex name;
//#define LOCK_MUTEX(name) boost::recursive_mutex::scoped_lock ogrenameLock(name);
//#define LOCK_MUTEX_NAMED(mutexName, lockName) boost::recursive_mutex::scoped_lock lockName(mutexName);
//
//#define SHARED_MUTEX mutable boost::recursive_mutex *DEFAULT_MUTEX_NAME;
//#define LOCK_SHARED_MUTEX assert(DEFAULT_MUTEX_NAME); boost::recursive_mutex::scoped_lock ogreAutoMutexLock(*DEFAULT_MUTEX_NAME);
//#define NEW_SHARED_MUTEX assert(!DEFAULT_MUTEX_NAME); DEFAULT_MUTEX_NAME = new boost::recursive_mutex();
//#define DELETE_SHARED_MUTEX assert(DEFAULT_MUTEX_NAME); delete OGRE_AUTO_MUTEX_NAME;
//#define COPY_SHARED_MUTEX(from) assert(!DEFAULT_MUTEX_NAME); DEFAULT_MUTEX_NAME = from;
//#define SET_SHARED_MUTEX_NULL DEFAULT_MUTEX_NAME = 0;
//#define MUTEX_CONDITIONAL(mutex) if (mutex)
//#define THREAD_SYNCHRONISER(sync) boost::condition sync;
//#define THREAD_WAIT(sync, lock) sync.wait(lock);
//#define THREAD_NOTIFY_ONE(sync) sync.notify_one(); 
//#define THREAD_NOTIFY_ALL(sync) sync.notify_all(); 
//// Thread-local pointer
//#define THREAD_POINTER(T, var) boost::thread_specific_ptr<T> var
//#define THREAD_POINTER_SET(var, expr) var.reset(expr)
//#define THREAD_POINTER_DELETE(var) var.reset(0)
//#define THREAD_POINTER_GET(var) var.get()
#define APOC3D_DEFAULT 0
#define APOC3D_SSE 1
#define APOC3D_MATH_IMPL APOC3D_DEFAULT

#define APOC3D_PLATFORM_WINDOWS 0
#define APOC3D_PLATFORM_MAC 1
#define APOC3D_PLATFORM_LINUX 2

#if defined( __WIN32__ ) || defined( _WIN32 )
#   define APOC3D_PLATFORM APOC3D_PLATFORM_WINDOWS

#elif defined( __APPLE_CC__)
#   define APOC3D_PLATFORM APOC3D_PLATFORM_MAC

#else
#   define APOC3D_PLATFORM APOC3D_PLATFORM_LINUX
#endif

#if APOC3D_DLLEX
	#define APAPI __declspec( dllexport )
#else
	#define APAPI __declspec( dllimport )
#endif


//#define VER(x,y,z,w) D3DCOLOR_ARGB(x,y,z,w);



#include <cassert>


#pragma warning(push)
#pragma warning(disable:4251)
#include <string>
#include <vector>
#include <list>
#include <map>

#include <queue>
#include <deque>
#include <unordered_set>
#include <unordered_map>

#include <algorithm>
#include <functional>
#include <limits>

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>


#include "FastDelegate\FastDelegate.h"

#pragma warning(pop)

// Forward Declarations
namespace Apoc3D
{
	void* memcpy_sse( char* pDest, const char* pSrc, size_t nBytes );
	
	
	typedef std::wstring String;
	//class APAPI wstring;
#	define StringCompare(a, b) !a.compare(b)
	//const int32 MaxInt32 = 0x7fffffff;

	namespace Core
	{		
		class Resource;
		class ResourceManager;
		class GameTime;

		class IParsable;

		template class APAPI fastdelegate::FastDelegate1<Resource*, void>;
		typedef fastdelegate::FastDelegate0<void> ResourceEventHandler;

		namespace Streaming
		{
			class AsyncProcessor;
		};
	};
	namespace Config
	{
		class Configuration;
		class ConfigurationSection;

		class ConfigurationManager;
		
	};
	namespace Math
	{	
		class Color4;

		class Matrix;
		class Quaternion;		

		class Plane;

		class Ray;

		class Point;
		class Size;

		class Rectangle;
		class RectangleF;
		class Box;

		class BoundingSphere;		
		class BoundingBox;
		class Frustum;

		class Random;
		class Randomizer;
	};
	namespace IO
	{
		class BinaryReader;
		class BinaryWriter;

		class TaggedDataReader;
		class TaggedDataWriter;

		class Stream;
		class FileStream;
		class MemoryStream;
		class MemoryOutStream;
		class VirtualStream;

		class TextureLevelData;
		class TextureData;
	};
	namespace VFS
	{

		class Archive;
		class ArchiveFactory;


		class FileLocateRule;
		class FileSystem;

		class ResourceLocation;
		class FileLocation;
		class MemoryLocation;
	};
	namespace EffectSystem
	{
		class Effect;
		class EffectManager;
		class EffectAtom;
	};
	namespace Scene
	{
		class SceneManager;
		class SceneNode;
		class SceneObject;
		class BatchData;

	};
	namespace Graphics
	{
		class Camera;

		class Renderable;
		class GeometryData;
		class RenderOperation;
		class RenderOperationBuffer;
		class Material;
		class SceneRenderer;
		class GraphicsDeviceManager;
		class ScenePass;
		class SceneProcedure;
		class SceneRenderer;

		namespace RenderSystem
		{
			class GraphicsAPIManager;
			class GraphicsAPIFactory;

			struct PresentParameters;
			class DeviceContent;
			
			template class APAPI fastdelegate::FastDelegate1<const Apoc3D::Core::GameTime*, void>;
			typedef fastdelegate::FastDelegate0<void> UpdateEventHandler;


			class Capabilities;

			class RenderView;
			class RenderWindow;
			class RenderWindowHandler;

			class ObjectFactory;
			class RenderStateManager;

			class RenderDevice;

			

			class HardwareBuffer;
			class VertexBuffer;
			class IndexBuffer;
			class DepthBuffer;

			class RenderTarget;

			class Texture;
			class VertexElement;
			class VertexDeclaration;

			class VertexShader;
			class PixelShader;

			class StateBlock;


			class Sprite;

			class FPSCounter;

		}
	};
};

#endif