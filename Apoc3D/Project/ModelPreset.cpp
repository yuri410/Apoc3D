#include "ModelPreset.h"

#include "apoc3d/Config/Configuration.h"
#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/IOLib/MaterialData.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace ProjectSystem
	{
		ModelPresetMaterial::~ModelPresetMaterial()
		{
			DELETE_AND_NULL(Data);
		}

		ModelPresetMaterial::ModelPresetMaterial(ModelPresetMaterial&& o)
			: Data(o.Data), NamePattern(std::move(o.NamePattern))
		{
			o.Data = nullptr;
		}
		ModelPresetMaterial& ModelPresetMaterial::operator=(ModelPresetMaterial&& o)
		{
			if (this != &o)
			{
				NamePattern = std::move(o.NamePattern);
				Data = o.Data;

				o.Data = nullptr;
			}
			return *this;
		}

		void ModelPreset::Parse(Configuration& conf)
		{
			ConfigurationSection* sect = conf[L"VertexFormatConversion"];
			
			if (UseVertexFormatConversion = (sect && sect->getSubSectionCount() > 0))
			{
				for (const ConfigurationSection* ent : sect->getSubSections())
				{
					VertexElementUsage usage = VertexElementUsageConverter[ent->getName()];
					int index = 0;
					if (usage == VEU_TextureCoordinate)
					{
						index = StringUtils::ParseInt32(ent->getValue());
					}

					// the vertex elements here only has usage and index. 
					// They only store info here, not for normal use in graphics
					ConversionVertexElements.Add(VertexElement(0, VEF_Count, usage, index));
				}
			}

			UseTextureNameConversion = conf[L"TextureNameConversion"] != nullptr;
			CollapseMeshs = conf[L"CollapseMeshs"] != nullptr;

			sect = conf[L"MateiralBase"];
			if (sect)
			{
				for (ConfigurationSection* ss : sect->getSubSections())
				{
					ModelPresetMaterial mpm;
					if (!ss->tryGetAttribute(L"NamePattern", mpm.NamePattern))
						mpm.NamePattern = L"*";

					mpm.Data = new MaterialData();
					mpm.Data->Parse(ss);

					BaseData.Add(std::move(mpm));
				}
			}
		}

		IO::MaterialData* ModelPreset::SearchMaterial(const String& name) const
		{
			for (const ModelPresetMaterial& mpm : BaseData)
			{
				if (StringUtils::Match(name, mpm.NamePattern))
					return mpm.Data;
			}
			return nullptr;
		}
	}
}
