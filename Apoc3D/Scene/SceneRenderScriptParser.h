#pragma once
#ifndef APOC3D_SCENERENDERSCRIPTPARSER_H
#define APOC3D_SCENERENDERSCRIPTPARSER_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "ScenePassTypes.h"

#include "apoc3d/Collections/HashMap.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Graphics::EffectSystem;

class TiXmlNode;
class TiXmlElement;
class TiXmlDocument;

namespace Apoc3D
{
	namespace Scene
	{
		//template class APAPI unordered_map<String, String>;
		typedef HashMap<String, String> BlockArgs;
		class APAPI SceneRenderScriptParser
		{
		public:
			List<ScenePassData> PassData;
			List<SceneVariable*> GlobalVars;

			SceneRenderScriptParser(RenderDevice* dev);

			/**
			 *  Build the script from xml config.
			 */
			void Parse(const ResourceLocation& rl);

			const String& getSceneName() const { return m_sceneName; }

		private:
			String m_sceneName;
			RenderDevice* m_renderDevice;
			
			/**
			 *  Build instructions for expressions
			 */
			void FillInstructions(const std::string& cmd, List<SceneInstruction>& instructions);
			/**
			 *  Build instructions for commands.
			 */
			void FillFunctionCall(const TiXmlElement* node, List<SceneInstruction>& instructions, const TiXmlDocument& doc);

			/**
			 *  Parse and add all the global vars to the list
			 */
			void ParseGlocalVarNode(const TiXmlElement* node, const TiXmlDocument& doc);

			/**
			 *  Build instructions for the RenderQuad command
			 */
			void FillRenderQuad(const TiXmlElement* node, List<SceneInstruction>& instructions, const TiXmlDocument& doc);
			/**
			 *  Build instructions for code blocks
			 */
			void BuildInstructions(const TiXmlElement* node, ScenePassData* data, const TiXmlDocument& doc);

			void BuildPass(const TiXmlElement* node, const TiXmlDocument& doc);
			void BuildNode(const TiXmlNode* node, const TiXmlDocument& doc);

			
		};
	}
}
#endif