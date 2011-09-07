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

#define APOC3D_DEFAULT 0
#define APOC3D_SSE 1
#define APOC3D_MATH_IMPL APOC3D_DEFAULT

#define APOC3D_PLATFORM_WINDOWS 0
#define APOC3D_PLATFORM_MAC 1
#define APOC3D_PLATFORM_LINUX 2

#define APOC3D_PLATFORM_NAME L"unknown"

#if defined( __WIN32__ ) || defined( _WIN32 )
#   define APOC3D_PLATFORM APOC3D_PLATFORM_WINDOWS
#	undef APOC3D_PLATFORM_NAME
#	define APOC3D_PLATFORM_NAME L"windows"
#elif defined( __APPLE_CC__)
#   define APOC3D_PLATFORM APOC3D_PLATFORM_MAC
#	undef APOC3D_PLATFORM_NAME
#	define APOC3D_PLATFORM_NAME L"mac"
#else
#   define APOC3D_PLATFORM APOC3D_PLATFORM_LINUX
#	undef APOC3D_PLATFORM_NAME
#	define APOC3D_PLATFORM_NAME L"linux"
#endif
#define APAPI
//#if APOC3D_DLLEX
//#define APAPI __declspec( dllexport )
//#else
//#define APAPI __declspec( dllimport )
//#endif


//#define VER(x,y,z,w) D3DCOLOR_ARGB(x,y,z,w);

#include <cassert>



//#pragma warning(disable:4251)
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

#define MaxUInt16 std::numeric_limits<uint16>::max();
#define MinUInt16 std::numeric_limits<uint16>::min();

#define MaxUInt32 std::numeric_limits<uint32>::max();
#define MinUInt32 std::numeric_limits<uint32>::min();

#define MaxUInt64 std::numeric_limits<uint64>::max();
#define MinUInt64 std::numeric_limits<uint64>::min();




#define MaxInt16 std::numeric_limits<int16>::max();
#define MinInt16 std::numeric_limits<int16>::min();

#define MaxInt32 std::numeric_limits<int32>::max();
#define MinInt32 std::numeric_limits<int32>::min();

#define MaxInt64 std::numeric_limits<int64>::max();
#define MinInt64 std::numeric_limits<int64>::min();





// Forward Declarations
namespace Apoc3D
{
	void* memcpy_sse( char* pDest, const char* pSrc, size_t nBytes );

	template class APAPI fastdelegate::FastDelegate0<void>;
	typedef fastdelegate::FastDelegate0<void> EventHandler;
	
	typedef std::wstring String;

//#	define StringCompare(a, b) !a.compare(b)

	class Project;
	class ProjectItemData;
	class ProjectCustomItem;
	class ProjectResource;
	class ProjectFolder;
	class ProjectResTexture;
	class ProjectResModel;
	class ProjectResEffect;
	class ProjectResFont;
	class ProjectItem;


	namespace Collections
	{
		template<typename T> class List;
		template<typename T> class List2D;
		template<typename T> class FastList;
		template<typename T> class FastQueue;
		template<typename T, typename S> class FastMap;
		template<typename T> class ExistTable;
		template<typename T> class Stack;
	}
	namespace Core
	{		
		class Resource;

		template<class T> 
		class ResourceHandle;

		class ResourceManager;
		class GameTime;
		

		class Plugin;
		class PluginManager;

		class IParsable;

		class Log;

		namespace Streaming
		{
			class ResourceOperation;

			class AsyncProcessor;
			class GenerationTable;
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

		class GaussBlurFilter;
	};
	namespace IO
	{
		class BinaryReader;
		class BinaryWriter;

		class TaggedDataReader;
		class TaggedDataWriter;

		class Stream;
		class FileStream;
		class FileOutStream;
		class MemoryStream;
		class MemoryOutStream;
		class VirtualStream;

		class TextureLevelData;
		class TextureData;

		class MeshData;
		class ModelData;
		class MaterialData;
	};
	namespace Input
	{
		class InputAPIManager;
		class InputAPIFactory;

		class Mouse;
		class Keyboard;
	}
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

	namespace Scene
	{
		class ScenePass;
		class SceneProcedure;
		class SceneRenderer;

		class SceneManager;
		class SceneNode;
		class SceneObject;
		class BatchData;
	};
	namespace Graphics
	{
		class Camera;
		class FpsCamera;
		class ChaseCamera;

		class Renderable;
		class GeometryData;
		class RenderOperation;
		class RenderOperationBuffer;
		class Material;
		
		class Mesh;
		class Model;
		class ModelSharedData;

		class Patch;
		class ParticleSettings;
		class ParticleSystem;

		namespace Animation
		{
			class ModelAnimationClip;
			class MaterialAnimationClip;

			class ModelKeyframe;
			class MaterialAnimationKeyframe;

			class AnimationData;

			class ModelAnimationPlayerBase;
			class RootAnimationPlayer;
			class RigidAnimationPlayer;
			class SkinnedAnimationPlayer;
			class MaterialAnimationPlayer;
		}

		namespace EffectSystem
		{
			class Effect;
			class EffectManager;
			class MicroEffect;
			class MicroEffectCodeParser;
		};
		namespace RenderSystem
		{
			class GraphicsAPIManager;
			class GraphicsAPIFactory;

			struct RenderParameters;
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


			class DataBox;
			class DataRectangle;
		}
	};
	namespace Platform
	{
		class Library;
	}
	namespace UI
	{
		class Font;
		class FontManager;

		class ControlContainer;
		class Form;
		class Control;
		class Button;
		class ButtonRow;
		class ButtonGroup;
		class RadioButton;
		class PictureBox;
		class Label;
		class ScrollBar;
		class TreeView;
		class TreeViewNode;
		class ListBox;
		class TextBox;
		class CheckBox;
		class CheckboxGroup;
		class ComboBox;

		class Menu;
		class MenuItem;
		class SubMenu;

		class Console;

		class StyleSkin;
	}
};

#endif