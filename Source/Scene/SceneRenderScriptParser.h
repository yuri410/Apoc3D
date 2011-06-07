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
#ifndef SCENERENDERSCRIPTPARSER_H
#define SCENERENDERSCRIPTPARSER_H

#include "Common.h"
#include "ScenePassTypes.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Graphics::EffectSystem;

using namespace std;

class TiXmlNode;
class TiXmlElement;

namespace Apoc3D
{
	namespace Scene
	{
		//template class APAPI unordered_map<String, String>;
		typedef unordered_map<String, String> BlockArgs;
		class APAPI SceneRenderScriptParser
		{
		private:
			String m_sceneName;
			RenderDevice* m_renderDevice;
			
			
			void FillInstructions(const string& cmd, std::vector<SceneInstruction>& instructions);
			void FillFunctionCall(const TiXmlElement* node, std::vector<SceneInstruction>& instructions);

			void ParseGlocalVarNode(const TiXmlElement* node);

			void BuildInstructions(const TiXmlElement* node, ScenePassData* data);
			void BuildPass(const TiXmlElement* node);
			void BuildNode(const TiXmlNode* node);
		public:
			FastList<ScenePassData> PassData;
			FastList<SceneVariable*> GlobalVars;

			SceneRenderScriptParser(RenderDevice* dev);

			void Parse(const ResourceLocation* rl);
		};
	}
}
#endif