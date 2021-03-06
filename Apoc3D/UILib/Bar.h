#pragma once
#ifndef APOC3D_UI_BARS_SCROLLBAR_H
#define APOC3D_UI_BARS_SCROLLBAR_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
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

#include "Control.h"

namespace Apoc3D
{
	namespace UI
	{
		enum struct BarDirection
		{
			Horizontal,
			Vertical
		};

		class APAPI ScrollBar : public Control
		{
			RTTI_DERIVED(ScrollBar, Control);
		public:
			typedef EventDelegate<ScrollBar*> ScrollBarEvent;

			ScrollBar(const ScrollBarVisualSettings& settings, const Point& position, BarDirection type, int32 length);
			ScrollBar(const StyleSkin* skin, const Point& position, BarDirection type, int32 length);
			~ScrollBar();

			void Update(const AppTime* time) override;
			void Draw(Sprite* sprite) override;

			void UpdateButtonPosition();
			void Reset() { m_value = 0; }

			void ForceScroll(int32 chg);
			void SetValue(int32 val);
			
			void SetLength(int32 len);
			int32 GetLength() const;

			bool isMouseHover() const { return m_isMouseHovering; }
			const Apoc3D::Math::Rectangle& getMouseHoverArea() const { assert(m_isMouseHovering); return m_mouseHoverArea; }

			int32 getValue() const { return m_value; }

			Button* getDecrButton() const { return m_decrButton; }
			Button* getIncrButton() const { return m_incrButton; }

			int32 Maximum = 0;
			int32 Step = 1;
			int32 VisibleRange = 0;

			UIGraphic BackgroundGraphic;
			UIGraphic HandleGraphic;
			UIGraphic DisabledBackgroundGraphic;

			ControlBounds BorderPadding;
			ControlBounds HandlePadding;

			bool IsInverted = false;

			ScrollBarEvent eventValueChanged;
			
		private:
			void PostInit(int32 length);
			void DecrButton_Pressed(Button* btn);
			void IncrButton_Pressed(Button* btn);

			int32 CalculateBarLength() const;
			int32 GetScrollableLength() const;
			Apoc3D::Math::Rectangle CalculateHandleArea() const;

			bool m_isMouseHovering = false;
			bool m_isDragging = false;
			BarDirection m_type;

			int32 m_value = 0;
			float m_valueFP = 0;

			Button* m_decrButton = nullptr;
			Button* m_incrButton = nullptr;

			Apoc3D::Math::Rectangle m_mouseHoverArea;

		};

		class APAPI ProgressBar : public Control
		{
			RTTI_DERIVED(ProgressBar, Control);
		public:
			ProgressBar(const ProgressBarVisualSettings& settings, const Point& position, int32 width);
			ProgressBar(const StyleSkin* skin, const Point& position, int32 width);
			~ProgressBar();

			void Update(const AppTime* time) override;
			void Draw(Sprite* sprite) override;

			void setWidth(int32 width) { m_size.X = width; }

			Texture* Graphic = nullptr;
			Apoc3D::Math::Rectangle BackgroundRegions[3];
			Apoc3D::Math::Rectangle BarRegions[3];
			ColorValue BackgroundColor = CV_White;
			ColorValue BarColor = CV_White;

			ControlBounds Margin;

			float CurrentValue = 0;

			int32 BarStartPad = 0;
			int32 BarEndPad = 0;

			TextRenderSettings TextSettings;
			String Text;
			
		};


		class APAPI SliderBar : public Control
		{
			RTTI_DERIVED(SliderBar, Control);
		public:
			typedef EventDelegate<SliderBar*, bool> SliderBarEvent;

			SliderBar(const SliderBarVisualSettings& settings, const Point& position, BarDirection type, int32 length);
			SliderBar(const StyleSkin* skin, const Point& position, BarDirection type, int32 length);
			~SliderBar();

			virtual void Update(const AppTime* time) override;
			virtual void Draw(Sprite* sprite) override;

			void SetLength(int32 len);
			int32 GetLength() const;

			bool isMouseHover() const { return m_isMouseHovering; }
			const Apoc3D::Math::Rectangle& getMouseHoverArea() const { assert(m_isMouseHovering); return m_mouseHoverArea; }

			int32 getTickValue() const { return static_cast<int32>(CurrentValue * LargeTickDivisionCount + 0.5f); }
			void setTickValue(int32 v) { assert(LargeTickDivisionCount > 0); CurrentValue = static_cast<float>(v) / LargeTickDivisionCount; }

			float CurrentValue = 0;
			int32 LargeTickDivisionCount = 0;

			Texture* Graphic = nullptr;
			Apoc3D::Math::Rectangle BackgroundRegions[3];
			Apoc3D::Math::Rectangle BarRegions[3];
			ColorValue BackgroundColor = CV_White;
			ColorValue BarColor = CV_White;

			ControlBounds Margin;

			int32 BarStartPad = 0;
			int32 BarEndPad = 0;
			
			Point HandleOffset;
			UIGraphicSimple HandleNormalGraphic;
			UIGraphicSimple HandleHoverGraphic;
			UIGraphicSimple HandleDownGraphic;
			UIGraphicSimple HandleDisabledGraphic;
			ControlBounds HandleMargin;

			UIGraphicSimple LargeTickGraphic;

			SliderBarEvent eventValueChanged;

			static SliderBarEvent eventAnyTick;
		private:
			int32 GetScrollableLength() const;
			Apoc3D::Math::Rectangle GetHandleArea() const;

			void SetValueFromCurrentPosition(Apoc3D::Input::Mouse* mouse, const Apoc3D::Math::Rectangle& area);

			bool hasLargeTicks() const { return LargeTickDivisionCount > 0; }

			bool m_isMouseHovering = false;
			bool m_isDragging = false;

			float m_lastValueBeforeDrag = 0;

			BarDirection m_type;

			Apoc3D::Math::Rectangle m_mouseHoverArea;

		};

		
	}
}

#endif