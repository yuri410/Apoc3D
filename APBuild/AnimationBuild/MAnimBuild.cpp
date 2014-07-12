#include "MAnimBuild.h"

#include "../BuildConfig.h"
#include "../CompileLog.h"
#include "../BuildEngine.h"

#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/IOLib/ModelData.h"
#include "apoc3d/Vfs/File.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Graphics/Animation/AnimationData.h"
#include "apoc3d/Collections/List.h"
#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/IOLib/MaterialData.h"
#include "apoc3d/IOLib/Streams.h"

using namespace Apoc3D::VFS;
using namespace Apoc3D::IO;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Utility;

namespace APBuild
{
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	void MAnimBuild::Build(const ConfigurationSection* sect)
	{
		MaterialAnimBuildConfig config;
		config.Parse(sect);

		if (!File::FileExists(config.SrcFile))
		{
			CompileLog::WriteError(config.SrcFile, L"Could not find source file.");
			return;
		}

		EnsureDirectory(PathUtils::GetDirectory(config.DstFile));

		AnimationData::MtrlClipTable mtrlClip;

		FileLocation fl(config.SrcFile);
		Configuration* script = XMLConfigurationFormat::Instance.Load(fl);

		ConfigurationSection* animSect = script->get(L"Animation");

		String takeName = animSect->getAttribute(L"Name");
		float frameRate = animSect->GetAttributeSingle(L"FrameRate");

		FastList<MaterialAnimationKeyframe> keyFrames;
		float maxTime = 0;
		for (ConfigurationSection::SubSectionEnumerator e = animSect->GetSubSectionEnumrator(); e.MoveNext();)
		{
			ConfigurationSection* subSect = *e.getCurrentValue();

			String type = subSect->getAttribute(L"Type");

			StringUtils::ToLowerCase(type);
			if (type == L"singleframe")
			{
				float frameTime = subSect->GetAttributeInt(L"Time") / 1000.0f;
				int materialID = subSect->GetAttributeInt(L"Material");
				uint flags = 0;
				String temp;
				if (subSect->tryGetAttribute(L"Flags", temp) && temp != L"none")
				{
					flags = MaterialAnimationKeyframe::MKF_Hidden;
				}

				if (maxTime<frameTime)
					maxTime = frameTime;

				keyFrames.Add(MaterialAnimationKeyframe(frameTime, materialID, flags));
			}
			else
			{
				float startFrameTime = subSect->GetAttributeInt(L"StartTime") / 1000.0f;
				int startMaterialID = subSect->GetAttributeInt(L"StartMaterial");
				int count = subSect->GetAttributeInt(L"Count");

				for (int i=0;i<count;i++)
				{
					float ft = startFrameTime+i/frameRate;
					if (maxTime<ft)
						maxTime = ft;

					keyFrames.Add(MaterialAnimationKeyframe(ft, startMaterialID+i));
				}
			}
		}

		maxTime += 1/frameRate;

		MaterialAnimationClip* clip = new MaterialAnimationClip(maxTime, keyFrames);

		mtrlClip.Add(takeName, clip);
		
		delete script;


		AnimationData* data = new AnimationData();
		data->setMaterialAnimationClips(mtrlClip);
		
		FileOutStream* fs = new FileOutStream(config.DstFile);

		data->Save(fs);

		delete data;

		CompileLog::WriteInformation(config.SrcFile, L">");
	}
}