#pragma once
#ifndef APOC3D_UICOMMON_H
#define APOC3D_UICOMMON_H

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
#include "apoc3d/Math/Rectangle.h"
#include "apoc3d/Math/ColorValue.h"
#include "apoc3d/EventDelegate.h"
#include "apoc3d/Collections/List.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace UI
	{
		typedef EventDelegate1<Control*> UIEventHandler;
		typedef EventDelegate1<MenuItem*> MenuItemEventHandler;

		enum struct TextHAlign
		{
			Left,
			Center,
			Right
		};
		enum struct TextVAlign
		{
			Top,
			Middle,
			Bottom
		};

		struct APAPI UIGraphic
		{
			Texture* Graphic = nullptr;
			ColorValue ModColor = CV_White;
			Collections::FixedList<Apoc3D::Math::Rectangle, 9> SourceRects;

			UIGraphic() { }

			UIGraphic(Texture* tex);
			UIGraphic(Texture* tex, ColorValue modColor);

			UIGraphic(Texture* tex, const Apoc3D::Math::Rectangle& srcRect);
			UIGraphic(Texture* tex, const Apoc3D::Math::Rectangle& srcRect, ColorValue modColor);

			UIGraphic(Texture* tex, const Apoc3D::Math::Rectangle(&srcRect)[3]);
			UIGraphic(Texture* tex, const Apoc3D::Math::Rectangle(&srcRect)[3], ColorValue modColor);

			UIGraphic(Texture* tex, const Apoc3D::Math::Rectangle(&srcRect)[9]);
			UIGraphic(Texture* tex, const Apoc3D::Math::Rectangle(&srcRect)[9], ColorValue modColor);

			void Draw(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect, bool vertical) const;
			void Draw(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect) const;
			void DrawVert(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect) const;

			bool isSet() const { return Graphic != nullptr; }

			bool isSimple() const { return SourceRects.getCount() == 1; }
			bool is3patch() const { return SourceRects.getCount() == 3; }
			bool is9patch() const { return SourceRects.getCount() == 9; }

			int32 getWidth() const;
			int32 getHeight() const;
			Point getSize() const;
		};

		struct APAPI UIGraphicSimple 
		{
			Texture* Graphic = nullptr;
			ColorValue ModColor = CV_White;
			Collections::FixedList<Apoc3D::Math::Rectangle, 1> SourceRects;

			UIGraphicSimple() { }

			UIGraphicSimple(Texture* tex);
			UIGraphicSimple(Texture* tex, ColorValue modColor);

			UIGraphicSimple(Texture* tex, const Apoc3D::Math::Rectangle& srcRect);
			UIGraphicSimple(Texture* tex, const Apoc3D::Math::Rectangle& srcRect, ColorValue modColor);

			void Draw(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect) const;
			void DrawCentered(Sprite* sprite, const Point& pos, const Point& parentSize) const;
			void DrawCentered(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect) const;

			bool isSet() const { return Graphic != nullptr; }

			int32 getWidth() const;
			int32 getHeight() const;
			Point getSize() const;
		};


		struct APAPI ControlBounds
		{
			enum SideIndex
			{
				SI_Left,
				SI_Top,
				SI_Right,
				SI_Bottom
			};

			int32 Left = 0;
			int32 Right = 0;
			int32 Top = 0;
			int32 Bottom = 0;

			ControlBounds()  { }
			ControlBounds(int32 left, int32 right, int32 top, int32 bottom) : Left(left), Right(right), Top(top), Bottom(bottom) { }
			ControlBounds(const Apoc3D::Math::Rectangle& graphicalArea, const Apoc3D::Math::Rectangle& hotArea);

			int32 getHorizontalSum() const { return Left + Right; }
			int32 getVerticalSum() const { return Top + Bottom; }

			Apoc3D::Math::Rectangle InflateRect(const Apoc3D::Math::Rectangle& rect) const;
			Apoc3D::Math::Rectangle ShrinkRect(const Apoc3D::Math::Rectangle& rect) const;

			int32 operator[](SideIndex idx) const;

			void SetFromLeftTopRightBottom(int32 padding[4]);
			void SetZero();
		};

		struct APAPI TextRenderSettings
		{
			bool HasTextShadow = false;

			Point TextShadowOffset = Point(2, 1);
			ColorValue TextShadowColor = CV_White;

			ColorValue TextColor = CV_Black;

			TextHAlign HorizontalAlignment = TextHAlign::Center;
			TextVAlign VerticalAlignment = TextVAlign::Middle;

			ControlBounds TextPadding;

			void Draw(Sprite* sprite, Font* font, const String& text, const Point& pos, const Point& size, int32 alpha) const;
			void Draw(Sprite* sprite, Font* font, const String& text, const Apoc3D::Math::Rectangle& area, int32 alpha) const;

			void DrawBG(Sprite* sprite, Font* font, const String& text, int32 selStart, int32 selEnd, const Point& pos, const Point& size, ColorValue bgcv) const;
			void DrawBG(Sprite* sprite, Font* font, const String& text, int32 selStart, int32 selEnd, const Apoc3D::Math::Rectangle& area, ColorValue bgcv) const;

			//void Draw(Sprite* sprite, Font* font, const List<String>& lines, const Point& pos, const Point& size, int32 alpha) const;
			//void Draw(Sprite* sprite, Font* font, const List<String>& lines, const Apoc3D::Math::Rectangle& area, int32 alpha) const;
		private:
			Point GetTextOffset(const Point& textSize, const Point& areaSize) const;
			Point GetTextOffset(Font* font, const String& text, const Point& size) const;
		};

		struct ButtonVisualSettings
		{
			Font* FontRef = nullptr;

			bool HasDisabledGraphic = false;
			bool HasNormalGraphic = false;
			bool HasMouseHoverGraphic = false;
			bool HasMouseDownGraphic = false;
			bool HasOverlayIcon = false;
			bool HasDisabledOverlayIcon = false;
			bool HasHotZonePadding = false;

			UIGraphic DisabledGraphic;
			UIGraphic NormalGraphic;
			UIGraphic MouseHoverGraphic;
			UIGraphic MouseDownGraphic;

			UIGraphicSimple OverlayIcon;
			UIGraphicSimple DisabledOverlayIcon;

			ControlBounds ContentPadding;
		};

		struct ScrollBarVisualSettings
		{
			bool HasBackgroundGraphic = false;
			bool HasHandleGraphic = false;
			bool HasDisabledBackgroundGraphic = false;
			bool HasDisabledHandleGraphic = false;
			bool HasBorderPadding = false;
			bool HasHandlePadding = false;

			UIGraphic BackgroundGraphic;
			UIGraphic HandleGraphic;
			UIGraphic DisabledBackgroundGraphic;
			UIGraphic DisabledHandleGraphic;

			ButtonVisualSettings DecrButton;
			ButtonVisualSettings IncrButton;

			ControlBounds BorderPadding;
			ControlBounds HandlePadding;

		};


		/**
		 *  The interfaces for System Forms and Panels. And it is also responsible
		 *  for drawing and updating them.
		 *  Any system forms should be added through this interface.
		 */
		namespace SystemUI
		{
			APAPI void Initialize(RenderDevice* device);
			APAPI void Finalize();

			APAPI void Add(ControlContainer* cc);
			APAPI void Remove(ControlContainer* cc);
			APAPI void RemoveForm(const String& name);
			APAPI void RemoveContainer(const String& name);
			APAPI void BringToFirst(ControlContainer* cc);

			APAPI void Draw();
			APAPI void Update(const Core::GameTime* time);

			APAPI Point ClampFormMovementOffset(Form* frm, const Point& vec);
			APAPI Texture* GetWhitePixel();

			/** Return the area to display UI in the viewport. In screen coordinates. */
			APAPI Apoc3D::Math::Rectangle GetUIArea(RenderDevice* device);

			APAPI bool GetMinimizedPosition(RenderDevice* dev, Form* form, Point& pos);
			APAPI Point GetMaximizedSize(RenderDevice* dev, Form* form);
			APAPI Apoc3D::Math::Rectangle GetMaximizedRect(RenderDevice* dev, Form* form);
			
			APAPI const List<Form*>& getForms();

			/** Specifies the area to display UI in the viewport. In unified coordinates. */
			APAPI extern RectangleF& UIArea;
			APAPI extern RectangleF& MaximizedArea;
			
			/**
			 *  The active form is the one that is currently being dragged/clicked.
			 *  Used to mark the first form being touched.
			 */
			APAPI extern Form*& ActiveForm;
			APAPI extern MenuBar*& MainMenu;
			APAPI extern Form*& TopMostForm;
			APAPI extern Form*& ModalForm;

			APAPI extern float DoubleClickInterval;
		}

		namespace GUIUtils
		{
			struct ScrollBarPositioning
			{
				Point Position;
				int32 Width = 0;

				int32 Length = 0;

				ScrollBarPositioning(int32 width) : Width(width) { }
			};

			APAPI void CalculateScrollBarPositions(const Apoc3D::Math::Rectangle& area, ScrollBarPositioning* vs, ScrollBarPositioning* hs);
		}

		APAPI void guiGenerateRegion9Rects(const Apoc3D::Math::Rectangle& dstRect, Apoc3D::Math::Rectangle (&srcRects)[9], Apoc3D::Math::Rectangle (&destRects)[9]);
		APAPI void guiGenerateRegion3Rects(const Point& pos, int32 width, Apoc3D::Math::Rectangle (&srcRects)[3], Apoc3D::Math::Rectangle (&destRects)[3]);
		APAPI void guiGenerateRegion3VertRects(const Point& pos, int32 width, Apoc3D::Math::Rectangle (&srcRects)[3], Apoc3D::Math::Rectangle (&destRects)[3]);

		
		APAPI void guiDrawRegion9(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect, ColorValue cv,
			Texture* texture, const Apoc3D::Math::Rectangle (&srcRects)[9], Apoc3D::Math::Rectangle* centerDstRegion = nullptr);

		APAPI void guiDrawRegion3(Sprite* sprite, const Point& pt, int width, ColorValue cv,
			Texture* texture, const Apoc3D::Math::Rectangle (&srcRects)[3], Apoc3D::Math::Rectangle* centerDstRegion = nullptr);

		APAPI void guiDrawRegion3Vert(Sprite* sprite, const Point& pos, int height, ColorValue cv,
			Texture* texture, const Apoc3D::Math::Rectangle (&srcRects)[3], Apoc3D::Math::Rectangle* centerDstRegion = nullptr);

		// Draw horizontal region 3 with a clipping width
		APAPI void guiDrawRegion3Clipped(Sprite* sprite, const Point& pos, int32 maxWidth, int32 clipWidth,
			ColorValue cv, Texture* texture, const Apoc3D::Math::Rectangle (&srcRects)[3]);

		// Assumed to include the border regions
		// No clips do fewer calculations, but can not draw stuff that are smaller than the start/end regions
		APAPI bool guiDrawRegion9Noclip(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect, ColorValue cv,
			Texture* texture, const Apoc3D::Math::Rectangle (&srcRects)[9], Apoc3D::Math::Rectangle* centerDstRegion = nullptr);

		APAPI bool guiDrawRegion3Noclip(Sprite* sprite, const Point& pt, int width, ColorValue cv,
			Texture* texture, const Apoc3D::Math::Rectangle (&srcRects)[3], Apoc3D::Math::Rectangle* centerDstRegion = nullptr);

		APAPI bool guiDrawRegion3VertNoclip(Sprite* sprite, const Point& pos, int height, ColorValue cv,
			Texture* texture, const Apoc3D::Math::Rectangle (&srcRects)[3], Apoc3D::Math::Rectangle* centerDstRegion = nullptr);

		APAPI void guiOmitLineText(Font* fnt, int32 widthlimit, String& line);

		
	}
}

#endif