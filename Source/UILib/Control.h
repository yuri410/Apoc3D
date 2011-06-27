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

#ifndef CONTROL_H
#define CONTROL_H

#include "UICommon.h"

#include "Math/Math.h"
#include "Math/Point.h"
#include "Graphics/Renderable.h"

using namespace Apoc3D;
using namespace Apoc3D::Core;
using namespace Apoc3D::Config;
using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace UI
	{
		class Control
		{
		private:
			UIEventHandler m_eventMouseOver;
			UIEventHandler m_eventMouseOut;
			UIEventHandler m_eventMousePress;
			UIEventHandler m_eventMouseRelease;

		protected:
			ControlContainer* m_owner;
			const StyleSkin* m_skin;
			Font* m_fontRef;

			virtual void OnMouseOver() {  if (!m_eventMouseOver.empty()) m_eventMouseOver(this); }
			virtual void OnMouseOut() {  if (!m_eventMouseOver.empty()) m_eventMouseOut(this); }
			virtual void OnPress() {  if (!m_eventMouseOver.empty()) m_eventMousePress(this); }
			virtual void OnRelease() {  if (!m_eventMouseOver.empty()) m_eventMouseRelease(this); }
		public:
			
			String Text;
			//ColorValue BackColor;
			//ColorValue ForeColor;
			Point Position;
			Point Size;


			bool Enabled;
			bool Visible;

			String TooltipText;

			ControlContainer* getOwner() const { return m_owner; }
			void setOwner(ControlContainer* val) { m_owner = val; }
			UIEventHandler& eventMouseOver() { return m_eventMouseOver; }
			UIEventHandler& eventMouseOut() { return m_eventMouseOut; }
			UIEventHandler& eventPress() { return m_eventMousePress; }
			UIEventHandler& eventRelease() { return m_eventMouseRelease; }



			Apoc3D::Math::Rectangle getArea() const
			{
				return Apoc3D::Math::Rectangle(Position.X, Position.Y, Size.X, Size.Y);
			}

			Control()
				: Position(Point::Zero), m_owner(0), m_skin(0), Enabled(true)
			{

			}
			Control(const Point& position)
				: Position(position), m_owner(0), m_skin(0), Enabled(true)
			{

			}
			Control(const Point& position, const String& text)
				: Position(position), Text(text), m_owner(0), m_skin(0), Enabled(true)
			{

			}
			Control(const Point& position, const String& text, const Point& size)
				: Position(position), Text(text), Size(size), m_owner(0), m_skin(0), Enabled(true)
			{

			}

			virtual void Initialize(RenderDevice* device);
			virtual void Update(const GameTime* const time) { }
			virtual void Draw(Sprite* sprite) = 0;

			virtual void SetSkin(const StyleSkin* skin) { m_skin = skin; }

			virtual void Load(ConfigurationSection* data);
			virtual void Save(ConfigurationSection* data);

		};

		class ControlContainer : public Control
		{

		};
	}
}

#endif