/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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
#include "MeshBuild.h"

#include "BuildSystem.h"
#include "BuildConfig.h"

#include "AIImporter.h"
#include "XImporter.h"
#include "FbxConverter.h"

namespace APBuild
{
	void BuildByFBX(ProjectResModel& config)
	{
		FbxConverter::Import(config);

	}
	void BuildByASS(ProjectResModel& config)
	{
		ModelData* data = AIImporter::Import(config);

		MeshBuild::PostProcess(data, config);

		data->Save(FileOutStream(config.DstFile));
		delete data;

	}
	void BuildByD3D(ProjectResModel& config)
	{
		ModelData* data = XImporter::Import(config);

		MeshBuild::PostProcess(data, config);

		FileOutStream fs(config.DstFile);
		if (config.CompactBuild)
			data->SaveLite(fs);
		else
			data->Save(fs);
		delete data;
	}

	void MeshBuild::Build(const String& hierarchyPath, const ConfigurationSection* sect)
	{
		ProjectResModel config(nullptr, nullptr);
		config.Parse(sect);
		
		if (!File::FileExists(config.SrcFile))
		{
			BuildSystem::LogError(config.SrcFile, L"Could not find source file.");
			return;
		}
		BuildSystem::EnsureDirectory(PathUtils::GetDirectory(config.DstFile));
		BuildSystem::EnsureDirectory(PathUtils::GetDirectory(config.DstAnimationFile));

		switch (config.Method)
		{
			case MeshBuildMethod::ASS: BuildByASS(config); break;
			case MeshBuildMethod::FBX: BuildByFBX(config); break;
			case MeshBuildMethod::D3D: BuildByD3D(config); break;
		}

		BuildSystem::LogEntryProcessed(config.DstFile, hierarchyPath);
	}

	void MeshBuild::PostProcess(ModelData* data, ProjectResModel& config)
	{
		if (config.CompactBuild)
			config.CollapseMeshs = true;
		
		if (config.PresetFile.size())
		{
			if (File::FileExists(config.PresetFile))
			{
				Configuration pc;
				XMLConfigurationFormat::Instance.Load(FileLocation(config.PresetFile), &pc);

				ModelPreset mp;
				mp.Parse(pc);

				config.CollapseMeshs |= mp.CollapseMeshs;
				if (mp.UseVertexFormatConversion)
				{
					config.ConversionVertexElements = mp.ConversionVertexElements;
					config.UseVertexFormatConversion = true;
				}

				ExecuteMaterialConversion(data, mp, config);
			}
			else
			{
				BuildSystem::LogError(config.PresetFile, L"Could not find model preset file.");
			}
		}

		MeshBuild::ConvertVertexData(data, config);
		MeshBuild::CollapseMeshs(data, config);

		
	}

	void MeshBuild::ConvertVertexData(ModelData* data, const ProjectResModel& config)
	{
		if (config.UseVertexFormatConversion)
		{
			for (MeshData* md : data->Entities)
			{
				List<VertexElement> newElements; // to new data buffer
				List<VertexElement> extractingElements; // from source

				for (const VertexElement& srcElement : md->VertexElements)
				{
					for (const VertexElement& targetElement : config.ConversionVertexElements)
					{
						if (srcElement.getUsage() == targetElement.getUsage())
						{
							if (targetElement.getUsage() == VEU_TextureCoordinate &&
								targetElement.getIndex() == srcElement.getIndex())
							{
								extractingElements.Add(srcElement);
							}
							else if (targetElement.getUsage() != VEU_TextureCoordinate)
							{
								extractingElements.Add(srcElement);
							}
						}
					}
				}

				// this will also keep newElements is one to one to extractingElements
				int newOffset = 0;
				for (const VertexElement& ve : extractingElements)
				{
					VertexElement newVe(newOffset, ve.getType(), ve.getUsage(), ve.getIndex());
					newElements.Add(newVe);

					newOffset += ve.getSize();
				}
				int newVertexSize = newOffset;

				char* newVertexData = new char[newVertexSize*md->VertexCount];

				for (uint j = 0; j < md->VertexCount; j++)
				{
					int dstOffset = newVertexSize * j;
					int srcOffset = md->VertexSize*j;

					for (int k = 0; k < newElements.getCount(); k++)
					{
						const VertexElement& vextr = extractingElements[k];
						const VertexElement& vstore = newElements[k];

						memcpy(newVertexData + dstOffset + vstore.getOffset(),
							md->VertexData + srcOffset + vextr.getOffset(),
							vextr.getSize());
					}
				}

				delete[] md->VertexData;
				md->VertexData = newVertexData;
				md->VertexElements = newElements;
				md->VertexSize = newVertexSize;
			}
		}
	}
	void MeshBuild::CollapseMeshs(ModelData* data, const ProjectResModel& config)
	{
		if ((config.CollapseMeshs || config.CollapseAll) && data->Entities.getCount() > 1)
		{
			MeshData* firstMd = data->Entities[0];
			MaterialData* firstMtrl = firstMd->Materials.getMaterial(0, 0);

			uint totalVertexCount = 0;
			uint totalMaterialCount = 0;
			uint refVertexSize = firstMd->VertexSize;
			int totalFaceCount = 0; // for preserving container space only

			int* indexShifters = new int[data->Entities.getCount()];
			int* materialShifters = new int[data->Entities.getCount()];

			for (int i = 0; i < data->Entities.getCount(); i++)
			{
				MeshData* md = data->Entities[i];


				indexShifters[i] = totalVertexCount;
				materialShifters[i] = totalMaterialCount;

				totalVertexCount += md->VertexCount;
				totalMaterialCount += md->Materials.getMaterialCount();
				totalFaceCount += md->Faces.getCount();

				if (refVertexSize != md->VertexSize)
				{
					BuildSystem::LogError(config.SrcFile,
						L"The mesh collapse only works with meshes that have the same vertex format.");
					delete[] indexShifters;
					delete[] materialShifters;
					return;
				}
			}

			MeshData* newMD = new MeshData();
			newMD->VertexData = new char[totalVertexCount*firstMd->VertexSize];
			newMD->VertexCount = totalVertexCount;
			newMD->VertexElements = firstMd->VertexElements;
			newMD->VertexSize = refVertexSize;
			newMD->Faces.ResizeDiscard(totalFaceCount);
			newMD->Materials.Resize(config.CollapseAll ? 1 : (int32)totalMaterialCount);
			newMD->BoundingSphere.Center = Vector3::Zero;
			newMD->BoundingSphere.Radius = 0;


			int vertexBufferOffset = 0;
			for (int i = 0; i < data->Entities.getCount(); i++)
			{
				MeshData* md = data->Entities[i];

				memcpy(newMD->VertexData + vertexBufferOffset, md->VertexData, refVertexSize*md->VertexCount);
				vertexBufferOffset += refVertexSize*md->VertexCount;

				for (int j = 0; j < md->Faces.getCount(); j++)
				{
					MeshFace f = md->Faces[j];

					f.IndexA += indexShifters[i];
					f.IndexB += indexShifters[i];
					f.IndexC += indexShifters[i];

					if (config.CollapseAll)
						f.MaterialID = 0;
					else
						f.MaterialID += materialShifters[i];

					newMD->Faces.Add(f);
				}

				if (config.CollapseAll)
				{
					newMD->Materials.Add(new MaterialData(*firstMtrl));
				}
				else
				{
					for (int32 j = 0; j < md->Materials.getMaterialCount(); j++)
					{
						Apoc3D::IO::MaterialData* mtrlData = md->Materials.getMaterial(j);
						newMD->Materials.Add(new MaterialData(*mtrlData));

						for (int32 k = 1; k < md->Materials.getFrameCount(j); k++)
						{
							Apoc3D::IO::MaterialData* mtrlData = md->Materials.getMaterial(j, k);
							newMD->Materials.AddFrame(new MaterialData(*mtrlData), j);
						}
					}
				}

				BoundingSphere result;
				BoundingSphere::Merge(result, newMD->BoundingSphere, md->BoundingSphere);
				newMD->BoundingSphere = result;
			}

			delete[] indexShifters;
			delete[] materialShifters;


			data->Entities.DeleteAndClear();

			data->Entities.Add(newMD);
		}
	}

	void MeshBuild::ExecuteMaterialConversion(ModelData* data, const ModelPreset& preset, const ProjectResModel& config)
	{
		MaterialData defaultMtrlSetup;
		defaultMtrlSetup.SetDefaults();

		if (preset.CopyEntireMaterial)
		{
			for (MeshData* md : data->Entities)
			{
				MaterialData* selectedNewMtrl = preset.SearchMaterial(md->Name);

				if (selectedNewMtrl == nullptr)
				{
					BuildSystem::LogWarning(config.SrcFile, L"No preset material found for mesh part '" + md->Name + L"'.");
					continue;
				}

				for (MaterialData* mtrl : md->Materials)
				{
					new (mtrl)MaterialData(*selectedNewMtrl);
				}
			}
		}
		else
		{
			if (preset.UseTextureNameConversion)
			{
				for (MeshData* md : data->Entities)
				{
					for (MaterialData* mtrl : md->Materials)
					{
						for (auto& e : mtrl->TextureName)
						{
							String& tn = e.Value;
							if (tn.size() && tn.find('.', 0) != String::npos)
							{
								tn = PathUtils::GetFileNameNoExt(tn);
								tn.append(L".tex");
							}
						}
					}
				}
			}

			for (MeshData* md : data->Entities)
			{
				MaterialData* selectedNewMtrl = preset.SearchMaterial(md->Name);

				if (selectedNewMtrl == nullptr)
				{
					BuildSystem::LogWarning(config.SrcFile, L"No preset material found for mesh part '" + md->Name + L"'.");
					continue;
				}

				for (MaterialData* mtrl : md->Materials)
				{
					if (selectedNewMtrl->ExternalRefName != defaultMtrlSetup.ExternalRefName) mtrl->ExternalRefName = selectedNewMtrl->ExternalRefName;

					if (selectedNewMtrl->EffectName.getCount() > 0)
					{
						mtrl->EffectName = selectedNewMtrl->EffectName;
					}

					if (selectedNewMtrl->CustomParametrs.getCount() > 0)
					{
						mtrl->CustomParametrs = selectedNewMtrl->CustomParametrs;
					}

					if (!preset.UseTextureNameConversion && selectedNewMtrl->TextureName.getCount() > 0)
					{
						mtrl->TextureName = selectedNewMtrl->TextureName;
					}

					if (selectedNewMtrl->PassFlags != defaultMtrlSetup.PassFlags) mtrl->PassFlags = selectedNewMtrl->PassFlags;

					if (selectedNewMtrl->Priority != defaultMtrlSetup.Priority) mtrl->Priority = selectedNewMtrl->Priority;

					if (selectedNewMtrl->UsePointSprite != defaultMtrlSetup.UsePointSprite) mtrl->UsePointSprite = selectedNewMtrl->UsePointSprite;

					if (selectedNewMtrl->SourceBlend != defaultMtrlSetup.SourceBlend) mtrl->SourceBlend = selectedNewMtrl->SourceBlend;
					if (selectedNewMtrl->DestinationBlend != defaultMtrlSetup.DestinationBlend) mtrl->DestinationBlend = selectedNewMtrl->DestinationBlend;
					if (selectedNewMtrl->BlendFunction != defaultMtrlSetup.BlendFunction) mtrl->BlendFunction = selectedNewMtrl->BlendFunction;
					if (selectedNewMtrl->IsBlendTransparent != defaultMtrlSetup.IsBlendTransparent) mtrl->IsBlendTransparent = selectedNewMtrl->IsBlendTransparent;

					if (selectedNewMtrl->Cull != defaultMtrlSetup.Cull) mtrl->Cull = selectedNewMtrl->Cull;

					if (selectedNewMtrl->DepthWriteEnabled != defaultMtrlSetup.DepthWriteEnabled) mtrl->DepthWriteEnabled = selectedNewMtrl->DepthWriteEnabled;
					if (selectedNewMtrl->DepthTestEnabled != defaultMtrlSetup.DepthTestEnabled) mtrl->DepthTestEnabled = selectedNewMtrl->DepthTestEnabled;

					if (selectedNewMtrl->Ambient != defaultMtrlSetup.Ambient) mtrl->Ambient = selectedNewMtrl->Ambient;
					if (selectedNewMtrl->Diffuse != defaultMtrlSetup.Diffuse) mtrl->Diffuse = selectedNewMtrl->Diffuse;
					if (selectedNewMtrl->Emissive != defaultMtrlSetup.Emissive) mtrl->Emissive = selectedNewMtrl->Emissive;
					if (selectedNewMtrl->Specular != defaultMtrlSetup.Specular) mtrl->Specular = selectedNewMtrl->Specular;
					if (selectedNewMtrl->Power != defaultMtrlSetup.Power) mtrl->Power = selectedNewMtrl->Power;
				}
			}
		}

	}
}