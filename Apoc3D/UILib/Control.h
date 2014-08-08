#pragma once
#ifndef APOC3D_CONTROL_H
#define APOC3D_CONTROL_H

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


#include "UICommon.h"

#include "apoc3d/Math/Point.h"
#include "apoc3d/math/Rectangle.h"
#include "apoc3d/Collections/List.h"

using namespace Apoc3D;
using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Config;
using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace UI
	{
		class APAPI Control
		{
			RTTI_BASE;
		public:
			Control()
				: Position(Point::Zero), Size(0,0), m_owner(0), m_skin(0), Enabled(true), Visible(true), UserData(0), m_fontRef(0)
			{

			}
			Control(const Point& position)
				: Position(position), Size(0,0), m_owner(0), m_skin(0), Enabled(true), Visible(true), UserData(0), m_fontRef(0)
			{

			}
			Control(const Point& position, const String& text)
				: Position(position), Text(text), Size(15,15), m_owner(0), m_skin(0), Enabled(true), Visible(true), UserData(0), m_fontRef(0)
			{

			}
			Control(const Point& position, const String& text, const Point& size)
				: Position(position), Text(text), Size(size), m_owner(0), m_skin(0), Enabled(true), Visible(true), UserData(0), m_fontRef(0)
			{

			}
			virtual ~Control() { }

			virtual void Initialize(RenderDevice* device);
			virtual void Update(const GameTime* const time)
			{
				//m_area = Apoc3D::Math::Rectangle(Position.X, Position.Y, Size.X, Size.Y);
			}
			virtual void Draw(Sprite* sprite) = 0;
			virtual void DrawOverlay(Sprite* sprite) { }

			virtual void SetSkin(const StyleSkin* skin) { m_skin = skin; }

			virtual void Load(ConfigurationSection* data) { };
			virtual void Save(ConfigurationSection* data) { };

		
			Point GetAbsolutePosition() const;

			/**
			 *  Gets a bool indicating if the control is blocking its parent
			 */
			virtual bool IsOverriding() { return false; }

			Apoc3D::Math::Rectangle getAbsoluteArea() const
			{
				Point pos = GetAbsolutePosition();

				return Apoc3D::Math::Rectangle(pos.X, pos.Y, Size.X, Size.Y);
			}
			Apoc3D::Math::Rectangle getArea() const { return Apoc3D::Math::Rectangle(Position.X, Position.Y, Size.X, Size.Y); }

			const StyleSkin* getSkin() const { return m_skin; }


			Font* getFontRef() const { return m_fontRef; }
			void setFontRef(Font* fnt) { m_fontRef = fnt; }

			ControlContainer* getOwner() const { return m_owner; }
			void setOwner(ControlContainer* val) { m_owner = val; }


			bool Enabled;
			bool Visible;

			/** 
			 *  Specifies the position of the control in screen coordinate.
			 *	This coordinate is relative to owner containers. 
			 *	Except those have no owner or root containers.
			 */
			Point Position;
			Point Size;

			String Name;
			String Text;

			String TooltipText;

			void* UserData;

			UIEventHandler eventMouseOver;
			UIEventHandler eventMouseOut;
			UIEventHandler eventPress;
			UIEventHandler eventRelease;

		protected:
			ControlContainer* m_owner;
			const StyleSkin* m_skin;
			Font* m_fontRef;

			//Apoc3D::Math::Rectangle m_area;

			virtual void OnMouseOver() { eventMouseOver.Invoke(this); }
			virtual void OnMouseOut() { eventMouseOut.Invoke(this); }
			virtual void OnPress() { eventPress.Invoke(this); }
			virtual void OnRelease() { eventRelease.Invoke(this); }
		};

		class APAPI ControlCollection
		{
		public:
			ControlCollection(ControlContainer* owner)
				: m_owner(owner), ActiveControl(0)
			{

			}

			
			int getCount() const { return m_controls.getCount(); }
			Control* operator [](int index) const { return m_controls[index]; }

			void Add(Control* ctrl);
			void Remove(Control* ctrl);

			void RemoveAt(int index);
			void Clear();

			void DestroyAndClear();

			Control* ActiveControl;

		protected:
			List<Control*> m_controls;

			ControlContainer* m_owner;

		};


		class APAPI ControlContainer : public Control
		{
			RTTI_DERIVED(ControlContainer, Control);
		public:
			ControlContainer();
			virtual ~ControlContainer();

			virtual void Initialize(RenderDevice* device);
			virtual void Update(const GameTime* const time);

			virtual void Draw(Sprite* sprite);

			ControlCollection& getControls() { return m_controls; }
			int getCount() const { return m_controls.getCount(); }
			Control* operator [](int index) const { return m_controls[index]; }

			Menu* getMenu() const { return m_menu; }
			void setMenu(Menu* m);

		protected:
			ControlCollection m_controls;
			Menu* m_menu;
			Point m_menuOffset;

		};
	}
}

#endif