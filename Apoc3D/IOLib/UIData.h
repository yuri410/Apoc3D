#pragma once

#ifndef AP_IO_UIDATA_H
#define AP_IO_UIDATA_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "apoc3d/Common.h"
#include "apoc3d/Math/Point.h"

namespace Apoc3D
{
	namespace IO
	{
		enum UIControlType
		{
			UICT_Unknown,
			UICT_Button,
			UICT_TextBox,
			UICT_Label
		};

		struct UIControlData
		{
			String Name;
			String Text;
			Apoc3D::Math::Point Position;
			Apoc3D::Math::Point Size;

			UIControlType Type;

			bool Enabled;
			bool Visible;
		};
		struct UIButtonData : public UIControlData
		{

		};

		struct UITextBoxData
		{

		};

		struct UILabelData
		{

		};

		class APAPI UILayoutData
		{
		public:
		};
	}
}
#endif