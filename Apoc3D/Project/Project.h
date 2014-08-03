#pragma once
#ifndef APOC3D_PROJECT_H
#define APOC3D_PROJECT_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "apoc3d/Graphics/PixelFormat.h"
#include "apoc3d/Collections/List.h"
#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Graphics/RenderSystem/VertexElement.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Config;
using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	/**
	 *  Presents and provides load and save operation for project configuration file.
	 *  Includes project directory structure, and all parameters used to compile assets to
	 *  final formats.
	*/


	class ProjectItem;

	enum struct ProjectItemType
	{
		Custom,
		Folder,
		Material,
		MaterialSet,
		Texture,
		Model,
		TransformAnimation,
		MaterialAnimation,
		Effect,
		EffectList,
		CustomEffect,
		ShaderNetwork,
		Font,
		FontGlyphDist,
		UILayout,
		Copy
	};
	enum struct TextureFilterType
	{
		Nearest,
		Box,
		BSpline
	};
	enum struct TextureBuildMethod
	{
		D3D,
		Devil,
		BuiltIn
	};
	enum struct TextureCompressionType
	{
		None,
		RLE,
		LZ4
	};
	enum struct MeshBuildMethod
	{
		ASS,
		/** The method for importing FBX files
		*/
		FBX,
		D3D
	};

	/*enum struct FontStyle
	{
		Regular,
		Bold,
		Italic,
		BoldItalic,
		Strikeout
	};*/



	/**
	 *  Represent the build configuration for a specific type of assert in the project.
	 */
	class APAPI ProjectItemData
	{
	public:
		virtual ProjectItemType getType() const = 0;
		
		/**
		 * Check if the item is not built yet
		 */
		virtual bool IsNotBuilt() = 0;
		/**
		 * Check if the item's built is earlier than the given time or not
		 */
		virtual bool IsEarlierThan(time_t t) = 0;

		/** Indicates if the item's required further editing on the built file after building
		*/
		virtual bool RequiresPostEdit() { return false; }
		virtual void Parse(const ConfigurationSection* sect) = 0;
		/** Save the project item as an section in the project file.
		 *  @param savingBuild If true, build information in the section will be generated.
		 */
		virtual void Save(ConfigurationSection* sect, bool savingBuild) = 0;
		virtual List<String> GetAllOutputFiles() =0;

		virtual ~ProjectItemData() { }

	protected:
		Project* m_project;

		ProjectItemData(Project* prj)
			: m_project(prj)
		{
		}

		List<String> simpleGetAllOutputFiles(const String& destinationFile);
		bool simpleIsEarlierThan(time_t t, const String& destinationFile);
		bool simpleIsNotBuilt(const String& destinationFile);
	};
	class APAPI ProjectCustomItem : public ProjectItemData
	{
	public:
		ProjectCustomItem(Project* prj)
			: ProjectItemData(prj)
		{

		}

		String EditorExtension;
		String DestFile;
		String SourceFile;

		HashMap<String, String> Properties;

		virtual bool IsNotBuilt();

		virtual bool IsEarlierThan(time_t t);

		virtual List<String> GetAllOutputFiles();

		virtual ProjectItemType getType() const { return ProjectItemType::Custom; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);
	};

	/**
	 *  Represents the type of resource that is natively supported by the engine.
	 */
	class APAPI ProjectResource : public ProjectItemData
	{
	protected:
		ProjectResource(Project* prj)
			: ProjectItemData(prj)
		{

		}
	};
	/** 
	 *  Represents a project folder 
	 *
	 *  When building, a project folder can either be converted into a archive file, or
	 *  just as a folder as a result
	 */
	class APAPI ProjectFolder : public ProjectItemData
	{
	public:
		String PackType;
		String DestinationPack;
		bool IncludeUnpackedSubFolderItems;

		ProjectFolder(Project* prj)
			: ProjectItemData(prj), IncludeUnpackedSubFolderItems(false)
		{

		}

		List<ProjectItem*> SubItems;

		virtual bool IsNotBuilt() { return true; }
		virtual bool IsEarlierThan(time_t t) { return true; }


		virtual List<String> GetAllOutputFiles();
		
		virtual ProjectItemType getType() const { return ProjectItemType::Folder; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);
		void SavePackBuildConfig(ConfigurationSection* sect);

	private:
		void AddPackageEntries(ConfigurationSection* sect, int32& idx);
	};

	/**
	 *  A texture
	 *  When building, a texture can come from 2 type of sources.
	 *   1. For cubemaps and volume maps, a series of 2D images/textures can be used to 
	 *      assemble the final result. Each 2D texture is used as a cubemap face or a slice.
	 *   2. From image/texture files. Pixel conversion, resizing, mipmap generation can be
	 *      accomplished by the build system when specified.
	 */
	class APAPI ProjectResTexture : public ProjectResource
	{
	public:
		ProjectResTexture(Project* prj)
			: ProjectResource(prj)
		{

		}
		

		String SourceFile;
		String DestinationFile;
		TextureCompressionType CompressionType;
		bool GenerateMipmaps;
		bool Resize;
		int NewWidth;
		int NewHeight;
		int NewDepth;
		TextureFilterType ResizeFilterType;
		Apoc3D::Graphics::PixelFormat NewFormat;

		bool AssembleCubemap;
		bool AssembleVolumeMap;

		HashMap<uint, String> SubMapTable;
		HashMap<uint, String> SubAlphaMapTable;

		TextureBuildMethod Method;

		virtual ProjectItemType getType() const { return ProjectItemType::Texture; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);


		virtual List<String> GetAllOutputFiles();

		virtual bool IsNotBuilt();
		virtual bool IsEarlierThan(time_t t);

	};

	class APAPI ProjectResMaterial : public ProjectResource
	{
	public:
		ProjectResMaterial(Project* prj)
			: ProjectResource(prj)
		{

		}

		String DestinationFile;

		virtual ProjectItemType getType() const { return ProjectItemType::Material; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);

		virtual List<String> GetAllOutputFiles();

		virtual bool IsNotBuilt();
		virtual bool IsEarlierThan(time_t t);
	};

	class APAPI ProjectResMaterialSet : public ProjectResource
	{
	public:
		ProjectResMaterialSet(Project* prj)
			: ProjectResource(prj)
		{

		}

		String SourceFile;
		String DestinationLocation;
		String DestinationToken;

		virtual ProjectItemType getType() const { return ProjectItemType::MaterialSet; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);

		virtual List<String> GetAllOutputFiles();

		virtual bool IsNotBuilt();
		virtual bool IsEarlierThan(time_t t);
	};

	/**
	 *  Represents model with animation
	 *  When building, a model is converted into a .mesh file, and a .anim file.
	 *  .mesh file only contains the geometry, material information of the model, while in the anim file
	 *  frames, bones are included.
	 */
	class APAPI ProjectResModel : public ProjectResource
	{
	public:
		ProjectResModel(Project* prj)
			: ProjectResource(prj)
		{

		}

		String SrcFile;
		String DstFile;
		String DstAnimationFile;

		MeshBuildMethod Method;

		bool UseVertexFormatConversion;
		List<VertexElement> VertexElements;

		bool CollapseMeshs;

		virtual bool RequiresPostEdit() { return true; }
		virtual ProjectItemType getType() const { return ProjectItemType::Model; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);

		virtual List<String> GetAllOutputFiles();

		virtual bool IsNotBuilt();
		virtual bool IsEarlierThan(time_t t);
	};
	
	class APAPI ProjectResEffect : public ProjectResource
	{
	public:
		ProjectResEffect(Project* prj)
			: ProjectResource(prj)
		{

		}

		String VS;
		String PS;
		String GS;

		String PListFile;
		String DestFile;

		String EntryPointVS;
		String EntryPointPS;
		String EntryPointGS;

		List<String> Targets;

		virtual ProjectItemType getType() const { return ProjectItemType::Effect; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);

		virtual List<String> GetAllOutputFiles();

		virtual bool IsNotBuilt();
		virtual bool IsEarlierThan(time_t t);

	};
	class APAPI ProjectResCustomEffect : public ProjectResource
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

		virtual ProjectItemType getType() const { return ProjectItemType::CustomEffect; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);

		virtual List<String> GetAllOutputFiles();

		virtual bool IsNotBuilt();
		virtual bool IsEarlierThan(time_t t);
	};
	class APAPI ProjectResEffectList : public ProjectResource
	{
	public:
		ProjectResEffectList(Project* prj)
			: ProjectResource(prj)
		{

		}

		String DestFile;

		virtual ProjectItemType getType() const { return ProjectItemType::EffectList; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);

		virtual List<String> GetAllOutputFiles();

		virtual bool IsNotBuilt();
		virtual bool IsEarlierThan(time_t t) { return true; }
	};

	class APAPI ProjectResShaderNetwork : public ProjectResource
	{
	public:
		ProjectResShaderNetwork(Project* prj)
			: ProjectResource(prj)
		{

		}

		String SrcFile;
		String DestFile;

		virtual ProjectItemType getType() const { return ProjectItemType::ShaderNetwork; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);

		virtual List<String> GetAllOutputFiles();

		virtual bool IsNotBuilt();
		virtual bool IsEarlierThan(time_t t);
	};

	/**
	 *  Represents font assets.
	 *  Font asset can only be built from system fonts so far.
	 *  When using languages like Chinese, Korean, the generated font file could be up to MBs in size.
	 */
	class APAPI ProjectResFont : public ProjectResource
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

		List<CharRange> Ranges;
		String SourceFile;
		float Size;
		bool AntiAlias;

		String DestFile;

		virtual ProjectItemType getType() const { return ProjectItemType::Font; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);

		virtual List<String> GetAllOutputFiles();

		virtual bool IsNotBuilt();
		virtual bool IsEarlierThan(time_t t);
	};

	class APAPI ProjectResFontGlyphDist : public ProjectResource
	{
	public:
		ProjectResFontGlyphDist(Project* prj)
			: ProjectResource(prj)
		{

		}
		
		String SourceFile;
		String DestFile;

		virtual ProjectItemType getType() const { return ProjectItemType::FontGlyphDist; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);

		virtual List<String> GetAllOutputFiles();

		virtual bool IsNotBuilt();
		virtual bool IsEarlierThan(time_t t);
	};

	class APAPI ProjectResTAnim : public ProjectResource
	{
	public:
		ProjectResTAnim(Project* prj)
			: ProjectResource(prj)
		{

		}

		String SourceFile;
		String DestinationFile;
		bool Reverse;

		HashMap<String, int> ObjectIndexMapping;

		virtual ProjectItemType getType() const { return ProjectItemType::TransformAnimation; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);

		virtual List<String> GetAllOutputFiles();

		virtual bool IsNotBuilt();
		virtual bool IsEarlierThan(time_t t);
	};

	class APAPI ProjectResMAnim : public ProjectResource
	{
	public:
		ProjectResMAnim(Project* prj)
			: ProjectResource(prj)
		{

		}

		String SourceFile;
		String DestinationFile;

		virtual ProjectItemType getType() const { return ProjectItemType::MaterialAnimation; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);

		virtual List<String> GetAllOutputFiles();

		virtual bool IsNotBuilt();
		virtual bool IsEarlierThan(time_t t);
	};

	class APAPI ProjectResUILayout : public ProjectResource
	{
	public:
		ProjectResUILayout(Project* prj)
			: ProjectResource(prj)
		{

		}

		String SourceFile;
		String DestinationFile;

		virtual ProjectItemType getType() const { return ProjectItemType::UILayout; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);

		virtual List<String> GetAllOutputFiles();

		virtual bool IsNotBuilt();
		virtual bool IsEarlierThan(time_t t);
	};


	class APAPI ProjectResCopy : public ProjectResource
	{
	public:
		ProjectResCopy(Project* prj)
			: ProjectResource(prj)
		{

		}

		String SourceFile;
		String DestinationFile;

		virtual ProjectItemType getType() const { return ProjectItemType::Copy; }
		virtual void Parse(const ConfigurationSection* sect);
		virtual void Save(ConfigurationSection* sect, bool savingBuild);

		virtual List<String> GetAllOutputFiles();

		virtual bool IsNotBuilt();
		virtual bool IsEarlierThan(time_t t);
	};


	/**
	 *  Represents one asset in the project.
	 *  A ProjectItem is one to one to a ProjectItemData, which means
	 *  the specific data used in specific type of item. A texture
	 *  item will use the ProjectResTexture for the data.
	 */
	class APAPI ProjectItem
	{
	public:
		ProjectItem(Project* prj)
			: m_parent(0), m_typeData(0), m_timeStamp(0), m_project(prj)
		{

		}
		ProjectItem(Project* prj, const String& name, ProjectItemData* data, ProjectFolder* parent = nullptr)
			: m_timeStamp(0), m_project(prj), m_parent(parent), m_typeData(data), m_name(name)
		{

		}

		const String& getName() const { return m_name; }
		ProjectItemType getType() const { return m_typeData->getType(); }
		ProjectItemData* getData() const { return m_typeData; }
		Project* getProject() const { return m_project; }
		virtual void Rename(const String& newName)
		{
			m_name = newName;
		}

		void Parse(const ConfigurationSection* sect);
		ConfigurationSection* Save(bool savingBuild);

		/**
		 *  Check if the item's built version is outdated
		 */
		bool IsOutDated() const 
		{
			if (m_typeData)
			{
				return m_typeData->IsNotBuilt() || m_typeData->IsEarlierThan(m_timeStamp);
			}
			return false;
		}
		void NotifyModified();

	private:
		Project* m_project;
		ProjectFolder* m_parent;

		ProjectItemData* m_typeData;

		String m_name;
		
		/**
		 *  The time of the last modification time
		 */
		time_t m_timeStamp;
	};
	class APAPI Project
	{
	private:
		List<ProjectItem*> m_items;
		String m_name;

		String m_texturePath;
		String m_materialPath;

		String m_basePath;
		String m_outputPath;
		String m_originalOutputPath;

		ConfigurationSection* Save();
	public:
		const String& getName() const { return m_name; }
		
		/** 
		 *  Gets the relative path for textures
		 */
		const String& getTexturePath() const { return m_texturePath; }
		/**
		 *  Gets the relative path for materials
		 */
		const String& getMaterialPath() const { return m_materialPath; }
		


		/**
		 *  Gets the absolute path for placing imported assets or project items
		 */
		const String& getOutputPath() const { return m_outputPath; }

		/**
		 *  Sets the absolute path for the project's source assets. 
		 *  Once this is set, OutputPath will be changed to "build" sub folder under this path
		 */
		void setBasePath(const String& path);
		/**
		 *  Gets the absolute path for the project's source assets. 
		 */
		const String& getBasePath() const { return m_basePath; }

		const List<ProjectItem*>& getItems() const { return m_items; }

		void AddItem(ProjectItem* item) { m_items.Add(item); }

		/**
		 *  Load the project from a ConfigurationSection
		 */
		void Parse(const ConfigurationSection* sect);
		
		/**
		 *  Saves the project into a file
		 */
		void Save(const String& file);

		/**
		 *  Generate a series of build action represented by ConfigurationSection object.
		 *  The sequence is based on the dependency of project items.
		 */
		void GenerateBuildScripts(List<ConfigurationSection*>& result);


		void setTexturePath(const String& path) { m_texturePath = path; }
	};



	namespace ProjectTypeUtils
	{
		ProjectItemType APAPI ParseProjectItemType(const String& str);
		String APAPI ToString(ProjectItemType type);
		void APAPI FillProjectItemTypeNames(List<String>& names);
		bool APAPI SupportsProjectItemType(const String& str);

		TextureFilterType APAPI ParseTextureFilterType(const String& str);
		String APAPI ToString(TextureFilterType flt);
		void APAPI FillTextureFilterTypeNames(List<String>& names);

		TextureBuildMethod APAPI ParseTextureBuildMethod(const String& str);
		String APAPI ToString(TextureBuildMethod method);
		void APAPI FillTextureBuildMethodNames(List<String>& names);

		TextureCompressionType APAPI ParseTextureCompressionType(const String& str);
		String APAPI ToString(TextureCompressionType ct);
		void APAPI FillTextureCompressionTypeNames(List<String>& names);

		MeshBuildMethod APAPI ParseModelBuildMethod(const String& str);
		String APAPI ToString(MeshBuildMethod method);
		void APAPI FillModelBuildMethodNames(List<String>& names);

		/*FontStyle ParseFontStyle(const String& str);
		String ToString(FontStyle type);
		void FillFontStyleNames(List<String>& names);
		*/
	};
}
#endif