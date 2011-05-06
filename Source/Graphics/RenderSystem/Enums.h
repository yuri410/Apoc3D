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


#ifndef GR_ENUMS_H
#define GR_ENUMS_H

#pragma once

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			/* 
			*/
			enum BufferUsage
			{
				BU_Static = 1,
				BU_Dynamic = 2,
				BU_WriteOnly = 4,
				BU_Discardable = 8
			};
			/* 
			*/
			enum CullMode
			{
				/// <summary>
				///  Do not cull back faces.
				/// </summary>
				None = 1,
				/// <summary>
				///  Cull back faces with clockwise vertices.
				/// </summary>
				Clockwise = 2,
				/// <summary>
				///  Cull back faces with counterclockwise vertices.
				/// </summary>
				CounterClockwise = 3,
			}
			/* 
			*/
			enum IndexBufferType    
			{
				Bit16,
				Bit32
			};
		}
	}
}
#endif