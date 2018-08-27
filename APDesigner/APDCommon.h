#pragma once
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

#include "apoc3d/Engine.h"

#pragma comment(lib, "Apoc3D.lib")

#ifdef APOC3D_DYNLIB
#	ifdef APOC3D_IDE_DYLIB_EXPORT
#		define APDAPI __declspec( dllexport )
#	else
#		define APDAPI __declspec( dllimport )
#	endif
#else
#	pragma comment(lib, "Apoc3D.D3D9RenderSystem.lib")
#	pragma comment(lib, "Apoc3D.WindowsInput.lib")
#	define APDAPI
#endif

namespace APDesigner
{
	class ResourcePane;
	class PropertyPane;
	class ToolsPane;
	class AtomManagerDialog;
	class OrphanDetector;

	class ObjectTools;
	class ObjectPropertyEditor;

	class Document;

	class MainWindow;
	
	class ShaderNetDocument;
	class ShaderGraph;
	class GraphNode;
	class QuadTreeNode;

	class ShaderAtomType;
	class ShaderAtomDocument;

	class ShaderAtomTypeData;
	class ShaderDocumentData;

	class ColorField;
	class PassFlagDialog;

	class EditorExtension;
	class IndenpendentEditorExtension;
	class IndenpendentEditor;

}