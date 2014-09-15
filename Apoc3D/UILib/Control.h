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
		class DoubleClickChecker
		{
		public:
			void Update(const GameTime* time);
			bool Check(Input::Mouse* mouse = nullptr);
		private:
			float m_timeSinceLastClick = FLT_MAX;
		};

		class APAPI Control
		{
			RTTI_BASE;
		public:
			Control();
			Control(const Point& position);
			Control(const Point& position, const Point& size);

			Control(const StyleSkin* skin);
			Control(const StyleSkin* skin, const Point& position);
			Control(const StyleSkin* skin, const Point& position, const Point& size);

			virtual ~Control();

			//virtual void Initialize(RenderDevice* device);
			virtual void Update(const GameTime* time) { }
			virtual void Draw(Sprite* sprite) = 0;
			virtual void DrawOverlay(Sprite* sprite) { }

			//virtual void SetSkin(const StyleSkin* skin) { m_skin = skin; }

			Point GetAbsolutePosition() const { return Position + BaseOffset; }

			/**
			 *  Gets a bool indicating if the control is blocking its parent
			 */
			virtual bool IsOverriding() { return false; }

			Apoc3D::Math::Rectangle getAbsoluteArea() const { return Apoc3D::Math::Rectangle(GetAbsolutePosition(), m_size); }
			Apoc3D::Math::Rectangle getArea() const { return Apoc3D::Math::Rectangle(Position, m_size); }

			const Point& getSize() const { return m_size; }
			int32 getWidth() const { return m_size.X; }
			int32 getHeight() const { return m_size.Y; }

			Font* getFont() const { return m_fontRef; }
			virtual void SetFont(Font* fontRef);

			bool Enabled = true;
			bool Visible = true;

			bool ParentFocused = true;

			/** 
			 *  Specifies the position of the control in screen coordinate.
			 *	This coordinate is relative to owner containers. 
			 *	Except those have no owner or root containers.
			 */
			Point Position = Point(0,0);
			Point BaseOffset = Point(0,0);

			String Name;
			String TooltipText;

			void* UserPointer = nullptr;
			int32 UserInt32 = 0;

		protected:
			Font* m_fontRef = nullptr;

			Point m_size;

			template <typename T>
			void UpdateEvents_StandardButton(bool& mouseHover, bool& mouseDown, const Apoc3D::Math::Rectangle area,
				void (T::*onMouseHover)(), void (T::*onMouseOut)(), void (T::*onPress)(), void (T::*onRelease)());

		private:
			void Initialze(const StyleSkin* skin);
		};

		class APAPI ScrollableControl : public Control
		{
			RTTI_DERIVED(ScrollableControl, Control);
		public:

			Apoc3D::Math::Rectangle GetContentArea() const;

			void EnableAllScrollBars() { EnableHScrollBar = EnableVScrollBar = true; }

			bool EnableVScrollBar = false;
			bool EnableHScrollBar = false;

			
		protected:
			ScrollableControl();
			ScrollableControl(const Point& position);
			ScrollableControl(const Point& position, const Point& size);

			ScrollableControl(const StyleSkin* skin);
			ScrollableControl(const StyleSkin* skin, const Point& position);
			ScrollableControl(const StyleSkin* skin, const Point& position, const Point& size);

			virtual ~ScrollableControl();

			void InitScrollbars(const StyleSkin* skin);
			void InitScrollBars(const ScrollBarVisualSettings& hss, const ScrollBarVisualSettings& vss);

			void UpdateScrollBarsLength(const Apoc3D::Math::Rectangle& area);
			void UpdateScrollBarsGeneric(const Apoc3D::Math::Rectangle& area, const GameTime* time);
			void DrawScrollBars(Sprite* sprite);

			ScrollBar* m_vscrollbar = nullptr;
			ScrollBar* m_hscrollbar = nullptr;

			bool m_alwaysShowHS = false;
			bool m_alwaysShowVS = false;
		};

		typedef List<Control*> ControlList;

		class APAPI ControlContainer : public Control
		{
			RTTI_DERIVED(ControlContainer, Control);
		public:
			ControlContainer(const StyleSkin* skin);
			virtual ~ControlContainer();

			//virtual void Initialize(RenderDevice* device);
			virtual void Update(const GameTime* time);

			virtual void Draw(Sprite* sprite);

			ControlList& getControls() { return m_controls; }

			MenuBar* MenuBar = nullptr;
		protected:
			ControlList m_controls;
			
			//Point m_menuOffset;

		};
	}
}

#endif