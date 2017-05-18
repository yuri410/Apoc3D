#pragma once
#ifndef APOC3D_XMLCONFIGURATIONFORMAT_H
#define APOC3D_XMLCONFIGURATIONFORMAT_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2012-2017 Tao Xin
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

#include "Configuration.h"
#include "apoc3d/Collections/List.h"

using namespace Apoc3D::VFS;

class TiXmlDocument;
class TiXmlNode;

namespace Apoc3D
{
	namespace Config
	{
		/** XML format configuration storing as described in Configuration */
		class APAPI XMLConfigurationFormat : public ConfigurationFormat
		{
		public:
			static XMLConfigurationFormat Instance;

			virtual void Load(const ResourceLocation& rl, Configuration* config) override;
			virtual void Save(Configuration* config, Stream& strm) override;

			List<String> GetSupportedFileSystemExtensions() override;

		private:
			/** Saves a ConfigurationSection sub tree to the tiny xml node, recursively. */
			void SaveNode(TiXmlNode* node, ConfigurationSection* parent);

			/** Read xml node recursively, while add data into ConfigurationSection */
			void BuildNode(Configuration* config, const TiXmlNode* node, ConfigurationSection* parent, const TiXmlDocument& doc);
			void BuildXml(Configuration* config, const TiXmlDocument* doc);
		};
	}
}
#endif