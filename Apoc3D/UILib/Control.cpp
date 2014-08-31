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

#include "Control.h"
#include "FontManager.h"
#include "StyleSkin.h"
#include "Menu.h"

namespace Apoc3D
{
	namespace UI
	{
		void Control::Initialize(RenderDevice* device)
		{
			if (m_skin)
			{
				if (m_fontRef == nullptr)
					m_fontRef = m_skin->ContentTextFont;
			}
			else
			{
			}
		}
		Point Control::GetAbsolutePosition() const
		{
			if (m_owner)
			{
				Point result = m_owner->GetAbsolutePosition();
				result.X += Position.X;
				result.Y += Position.Y;
				return result;
			}
			return Position;
		}

		void ControlCollection::Add(Control* ctrl)
		{
			ctrl->setOwner(m_owner);
			m_controls.Add(ctrl);
		}
		void ControlCollection::Remove(Control* ctrl)
		{
			ctrl->setOwner(0);
			m_controls.Remove(ctrl);
		}
		void ControlCollection::RemoveAt(int index)
		{
			m_controls[index]->setOwner(0);
			m_controls.RemoveAt(index);
		}

		void ControlCollection::Clear()
		{
			m_controls.Clear();
		}
		void ControlCollection::DestroyAndClear()
		{
			m_controls.DeleteAndClear();
		}



		ControlContainer::ControlContainer()
			: Control(), m_controls(this), m_menu(0), m_menuOffset(0,20)
		{

		}
		ControlContainer::~ControlContainer()
		{
		}
		void ControlContainer::setMenu(Menu* m)
		{
			 m_menu = m;
			 m->setOwner(this);
		}
		void ControlContainer::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);
			for (int i=0;i<m_controls.getCount();i++)
			{
				m_controls[i]->Initialize(device);
			}
		}

		void ControlContainer::Draw(Sprite* sprite)
		{
			int overlay = 0;
			for (int i=0;i<m_controls.getCount();i++)
			{
				Control* ctrl = m_controls[i];
				if (ctrl->IsOverriding())
				{
					overlay = i;
				}
				if (ctrl->Enabled)
				{
					ctrl->Draw(sprite);
				}
			}
			if (overlay)
			{
				m_controls[overlay]->DrawOverlay(sprite);
			}

			if (m_menu && m_menu->Visible)
			{
				m_menu->Draw(sprite);
			}
		}
		void ControlContainer::Update(const GameTime* time)
		{
			bool skip = false;
			for (int i=0;i<m_controls.getCount();i++)
			{
				if (m_controls[i]->IsOverriding())
				{
					m_controls[i]->Update(time);
					skip = true;
				}
			}
			if (!skip)
			{
				for (int i=0;i<m_controls.getCount();i++)
				{
					if (m_controls[i]->Enabled)
					{
						m_controls[i]->Update(time);
					}
				}
			}
		}

	}
}