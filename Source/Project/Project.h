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
#include "Common.h"

#include "Graphics/PixelFormat.h"
#include "Collections/FastList.h"
#include "Collections/FastMap.h"

using namespace Apoc3D;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Config;
using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	/** Holds, load and save project configuration file.
		Include project directory structure, and all parameters used to compile assets to
		final formats.
	*/


	class ProjectItem;

	enum ProjectItemType
	{
		PRJITEM_Custom,
		PRJITEM_Folder,
		PRJITEM_Texture,
		PRJITEM_Model,
		PRJITEM_Animation,
		PRJITEM_Effect,
		PRJITEM_EffectList,
		PRJITEM_CustomEffect,
		PRJITEM_Font
	};
	
	/** Represent the build configuration for a specific type of assert in the project.
	*/
	class ProjectItemData
	{
	private:
		ProjectItemType m_type;
	protected:
		Project* m_project;

		ProjectItemData(Project* prj)
			: m_project(prj)
		{
		}
	public:
		virtual ProjectItemType getType() const = 0;
		virtual bool IsNotBuilt() = 0;
		virtual bool IsEarlierThan(time_t t) = 0;
		virtual bool RequiresPostEdit() { return false; }
		virtual void Parse(const ConfigurationSection* sect) = 0;
		virtual void Save(ConfigurationSection* sect, bool savingBuild) = 0;
		virtual std::vector<String> GetAllOutputFiles() =0;

		virtual ~ProjectItemData() { }
	};
	class ProjectCustomItem : public ProjectItemData
	{
	public:
		ProjectCustomItem(Project* prj)
			: ProjectItemData(prj)
		{

		}

		String DestFile;

		virtual bool IsNotBuilt();
		virtual bool IsEarlierThan(time_t t);

		virtual std::vector<String> GetAllOutputFiles();

		virtual ProjectItemType getType() const { return PRJITEM_Custom; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);
	};
	class ProjectResource : public ProjectItemData
	{
	protected:
		ProjectResource(Project* prj)
			: ProjectItemData(prj)
		{

		}
	};
	class ProjectFolder : public ProjectItemData
	{
	public:
		String PackType;
		String DestinationPack;

		ProjectFolder(Project* prj)
			: ProjectItemData(prj)
		{

		}

		FastList<ProjectItem*> SubItems;

		virtual bool IsNotBuilt() { return true; }
		virtual bool IsEarlierThan(time_t t) { return true; }


		virtual std::vector<String> GetAllOutputFiles();
		
		virtual ProjectItemType getType() const { return PRJITEM_Folder; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);
		void SavePackBuildConfig(ConfigurationSection* sect);
	};

	class ProjectResTexture : public ProjectResource
	{
	public:
		ProjectResTexture(Project* prj)
			: ProjectResource(prj)
		{

		}
		enum TextureFilterType
		{
			TFLT_Nearest,
			TFLT_Box,
			TFLT_BSpline
		};
		enum TextureBuildMethod
		{
			TEXBUILD_D3D,
			TEXBUILD_Devil,
			TEXBUILD_BuiltIn
		};
		static TextureFilterType ParseFilterType(const String& str);
		static TextureBuildMethod ParseBuildMethod(const String& str);
		static String ToString(TextureFilterType flt);
		static String ToString(TextureBuildMethod method);

		String SourceFile;
		String DestinationFile;
		bool GenerateMipmaps;
		bool Resize;
		int NewWidth;
		int NewHeight;
		int NewDepth;
		TextureFilterType ResizeFilterType;
		Apoc3D::Graphics::PixelFormat NewFormat;

		bool AssembleCubemap;
		bool AssembleVolumeMap;

		FastMap<uint, String> SubMapTable;
		FastMap<uint, String> SubAlphaMapTable;

		TextureBuildMethod Method;

		virtual ProjectItemType getType() const { return PRJITEM_Texture; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);


		virtual std::vector<String> GetAllOutputFiles();

		virtual bool IsNotBuilt();
		virtual bool IsEarlierThan(time_t t);

	};

	class ProjectResModel : public ProjectResource
	{
	public:
		ProjectResModel(Project* prj)
			: ProjectResource(prj)
		{

		}

		enum MeshBuildMethod
		{
			MESHBUILD_ASS,
			MESHBUILD_FBX,
			MESHBUILD_D3D
		};

		static MeshBuildMethod ParseBuildMethod(const String& str);
		static String ToString(MeshBuildMethod method);


		String SrcFile;
		String DstFile;
		String DstAnimationFile;

		MeshBuildMethod Method;

		virtual bool RequiresPostEdit() { return true; }
		virtual ProjectItemType getType() const { return PRJITEM_Model; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);

		virtual std::vector<String> GetAllOutputFiles();

		virtual bool IsNotBuilt();
		virtual bool IsEarlierThan(time_t t);

	};
	

	class ProjectResEffect : public ProjectResource
	{
	public:
		ProjectResEffect(Project* prj)
			: ProjectResource(prj)
		{

		}

		String SrcVSFile;
		String SrcPSFile;
		String PListFile;
		String DestFile;
		String EntryPointVS;
		String EntryPointPS;
		String Profile;

		virtual ProjectItemType getType() const { return PRJITEM_Effect; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);

		virtual std::vector<String> GetAllOutputFiles();

		virtual bool IsNotBuilt();
		virtual bool IsEarlierThan(time_t t);

	};
	class ProjectResCustomEffect : public ProjectResource
	{
	public:
		ProjectResCustomEffect(Project* prj)
			: ProjectResource(prj)
		{

		}

		String SrcVSFile;
		String SrcPSFile;
		String DestFile;
		String EntryPointVS;
		String EntryPointPS;
		String Profile;

		virtual ProjectItemType getType() const { return PRJITEM_CustomEffect; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);

		virtual std::vector<String> GetAllOutputFiles();

		virtual bool IsNotBuilt();
		virtual bool IsEarlierThan(time_t t);
	};
	class ProjectResEffectList : public ProjectResource
	{
	public:
		ProjectResEffectList(Project* prj)
			: ProjectResource(prj)
		{

		}

		String DestFile;

		virtual ProjectItemType getType() const { return PRJITEM_EffectList; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);

		virtual std::vector<String> GetAllOutputFiles();

		virtual bool IsNotBuilt();
		virtual bool IsEarlierThan(time_t t) { return true; }
	};
	class ProjectResFont : public ProjectResource
	{
	public:
		ProjectResFont(Project* prj)
			: ProjectResource(prj)
		{

		}
		struct CharRange
		{
			int MinChar;
			int MaxChar;
		};
		enum FontStyle
		{
			FONTSTYLE_Regular,
			FONTSTYLE_Bold,
			FONTSTYLE_Italic,
			FONTSTYLE_BoldItalic,
			FONTSTYLE_Strikeout

		};

		FastList<CharRange> Ranges;
		FontStyle Style;
		String Name;
		float Size;

		String DestFile;

		virtual ProjectItemType getType() const { return PRJITEM_Font; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);

		virtual std::vector<String> GetAllOutputFiles();

		virtual bool IsNotBuilt();
		virtual bool IsEarlierThan(time_t t);
	};

	/** Represents one asset in the project.
	*/
	class ProjectItem
	{
	private:
		Project* m_project;
		ProjectFolder* m_parent;

		ProjectItemData* m_typeData;

		String m_name;
		
		/** The time of last build config modification of this item
		*/
		time_t m_timeStamp;
	public:
		ProjectItem(Project* prj)
			: m_parent(0), m_typeData(0), m_timeStamp(0), m_project(prj)
		{

		}

		const String& getName() const { return m_name; }
		ProjectItemType getType() const { return m_typeData->getType(); }
		ProjectItemData* getData() const { return m_typeData; }
		virtual void Rename(const String& newName)
		{
			m_name = newName;
		}

		void Parse(const ConfigurationSection* sect);
		ConfigurationSection* Save(bool savingBuild);

		bool IsOutDated() const 
		{
			if (m_typeData)
			{
				return m_typeData->IsNotBuilt() || m_typeData->IsEarlierThan(m_timeStamp);
			}
			return false;
		}
		void NotifyModified()
		{
			m_timeStamp = time(0);
		}
		//void Build();
		
	};
	class Project
	{
	private:
		FastList<ProjectItem*> m_items;
		String m_name;

		String m_texturePath;
		String m_basePath;
		String m_outputPath;

		ConfigurationSection* Save();
	public:
		const String& getName() const { return m_name; }
		const String& getBasePath() const { return m_basePath; }
		const String& getTexturePath() const { return m_texturePath; }
		const String& getOutputPath() const { return m_outputPath; }
		void setBasePath(const String& path);

		const FastList<ProjectItem*>& getItems() const { return m_items; }

		void Parse(const ConfigurationSection* sect);
		
		void Save(const String& file);

		void GenerateBuildScripts(FastList<ConfigurationSection*>& result);
	};


}