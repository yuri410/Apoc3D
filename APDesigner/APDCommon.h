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

#ifndef APDCOMMON_H
#define APDCOMMON_H

#include "apoc3d/Engine.h"

#pragma comment(lib, "Apoc3D.lib")

#ifdef APOC3D_DYNLIB
#	ifdef APOC3D_IDE_DYLIB
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

#endif