#include "MAnimBuild.h"


#include "Config/ConfigurationSection.h"
#include "BuildConfig.h"
#include "IOLib/Streams.h"
#include "IOLib/ModelData.h"
#include "CompileLog.h"
#include "Vfs/File.h"
#include "Vfs/PathUtils.h"
#include "Vfs/ResourceLocation.h"
#include "BuildEngine.h"
#include "Graphics/Animation/AnimationData.h"
#include "Collections/FastList.h"
#include "Config/ConfigurationSection.h"
#include "Config/XmlConfiguration.h"
#include "Vfs/ResourceLocation.h"
#include "Vfs/PathUtils.h"
#include "Utility/StringUtils.h"
#include "IOLib/MaterialData.h"
#include "IOLib/Streams.h"
#include "CompileLog.h"
#include "BuildEngine.h"

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

		FileLocation* fl = new FileLocation(config.SrcFile);
		XMLConfiguration* script = new XMLConfiguration(fl);

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

				if (maxTime<frameTime)
					maxTime = frameTime;

				keyFrames.Add(MaterialAnimationKeyframe(frameTime, materialID));
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

		mtrlClip.insert(std::make_pair(takeName, clip));
		
		delete script;
		delete fl;


		AnimationData* data = new AnimationData();
		data->setMaterialAnimationClips(mtrlClip);
		
		FileOutStream* fs = new FileOutStream(config.DstFile);

		data->Save(fs);

		delete data;

		CompileLog::WriteInformation(config.SrcFile, L">");
	}
}