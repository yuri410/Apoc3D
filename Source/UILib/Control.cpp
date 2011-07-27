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

#include "Control.h"
#include "FontManager.h"
#include "StyleSkin.h"

namespace Apoc3D
{
	namespace UI
	{
		void Control::Initialize(RenderDevice* device)
		{
			if (m_skin)
			{
				m_fontRef = FontManager::getSingleton().getFont(m_skin->ControlFontName);
			}
			else
			{
			}
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


		ControlContainer::ControlContainer()
			: Control(), m_menu(0), m_menuOffset(0,20)
		{
			m_controls = new ControlCollection(this);
		}
		ControlContainer::~ControlContainer()
		{
			delete m_controls;
		}
		void ControlContainer::Initialize(RenderDevice* device)
		{
			for (int i=0;i<m_controls->getCount();i++)
			{
				m_controls->operator[](i)->Initialize(device);
			}
		}

		void ControlContainer::Draw(Sprite* sprite)
		{
			int overlay = 0;
			for (int i=0;i<m_controls->getCount();i++)
			{
				if (m_controls->operator[](i)->Enabled)
				{
					m_controls->operator[](i)->Draw(sprite);
				}
			}
			if (overlay)
			{

			}


		}
		void ControlContainer::Update(const GameTime* const time)
		{
			bool skip = false;
			for (int i=0;i<m_controls->getCount();i++)
			{
				if (m_controls->operator[](i)->IsOverriding())
				{
					m_controls->operator[](i)->Update(time);
					skip = true;
				}
			}
			if (!skip)
			{
				for (int i=0;i<m_controls->getCount();i++)
				{
					if (m_controls->operator[](i)->Enabled)
					{
						m_controls->operator[](i)->Update(time);
					}
				}
			}
		}

	}
}