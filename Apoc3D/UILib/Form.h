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

#ifndef APOC3D_FORM_H
#define APOC3D_FORM_H

#include "UICommon.h"

#include "Control.h"

#include "apoc3d/Collections/FastList.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace UI
	{
		enum BorderStyle
		{
			FBS_None,
			FBS_Fixed,
			FBS_Sizable,
			FBS_Pane
		};
		class APAPI Border
		{
		private:
			Point m_shadowOffset;
			
			Apoc3D::Math::Rectangle m_dstRect[9];
			const StyleSkin* m_skin;
			BorderStyle m_style;

			void UpdateRects(const Point& position, const Point& size);
			void DrawUpper(Sprite* sprite);
			void DrawMiddle(Sprite* sprite);
			void DrawLower(Sprite* sprite);
			void DrawShadow(Sprite* sprite, const Point& pos, float alpha);

		public:
			Border(BorderStyle style, const StyleSkin* skin);
			void Draw(Sprite* sprite, const Point& pt, const Point& size, float shadowAlpha);
			

		};
		class APAPI Form : public ControlContainer
		{
		public:
			
			enum WindowState
			{
				FWS_Normal,
				FWS_Minimized,
				FWS_Maximized
			};

		public:

			Form(BorderStyle border = FBS_Sizable, const String& title = L"");
			virtual ~Form();

			void ShowModal();
			virtual void Show();
			virtual void Hide();
			virtual void Close();
			virtual void Focus();
			virtual void Unfocus();
			virtual void Minimize();
			virtual void Maximize();
			virtual void Restore();

			virtual void Initialize(RenderDevice* device);
			
			virtual void Update(const GameTime* const time);
			virtual void Draw(Sprite* sprite);


			RenderDevice* getRenderDevice() const { return m_device; }

			void setMinimumSize(const Point& size) { m_minimumSize = size; }
			void setMaximumSize(const Point& size) { m_maximumSize = size; }

			bool isResized() const { return m_isResizeing; }
			bool isDragged() const { return m_isDragging; }
			bool isMinimizing() const { return m_isMinimizing; }

			bool getHasMinimizeButton() const { return m_hasMinimizeButton; }
			void setHasMinimizeButton(bool val) { m_hasMinimizeButton = val; }

			bool getHasMaximizeButton() const { return m_hasMaximizeButton; }
			void setHasMaximizeButton(bool val) { m_hasMaximizeButton = val; }

			const String& getTitle() const { return m_title; }
			void setTitle(const String& txt) { m_title = txt; }

			WindowState getState() const { return m_state; }
			BorderStyle getBorderStyle() const { return m_borderStyle; }

			UIEventHandler& eventResized() { return m_eResized; }
			UIEventHandler& eventClosed() { return m_eClosed; }

		private:
			void InitializeButtons(RenderDevice* device);

			void DrawTitle(Sprite* sprite);
			void DrawButtons(Sprite* sprite);

			void UpdateTopMost();
			void UpdateActive();
			void UpdateState();
			void CheckDragging();
			void CheckDoubleClick(const GameTime* const time);
			void CheckResize();
			void ToggleWindowState();

			void btClose_Release(Control* sender);
			void btMinimize_Release(Control* sender);
			void btMaximize_Release(Control* sender);
			void btRestore_Release(Control* sender);

			RenderDevice* m_device;

			Point m_titleOffset;
			Point m_minimumSize;
			Point m_minimizedSize;
			Point m_maximumSize;

			bool m_isMinimized;
			bool m_isMaximized;

			Point m_previousPosition;
			Point m_previousSize;
			/**
			 *  The position to display form title when minimized
			 */
			Point m_minimizedPos;
			Point m_maximizedPos;

			String m_title;

			UIEventHandler m_eResized;
			UIEventHandler m_eClosed;


			Border* m_border;
			float m_borderAlpha;

			Button* m_btClose;
			Button* m_btMinimize;
			Button* m_btMaximize;
			Button* m_btRestore;

			bool m_hasMinimizeButton;
			bool m_hasMaximizeButton;

			/**
			 *  Specify the form's drag area, where can be dragged to move the form
			 */
			Apoc3D::Math::Rectangle m_dragArea;
			/**
			 *  Specify the form's resize area, where can be dragged to resize the form
			 */
			Apoc3D::Math::Rectangle m_resizeArea;
			
			bool m_isDragging;
			bool m_isResizeing;

			/** 
			 *  Tells if the form is on the minimizing animation process.
			 */
			bool m_isMinimizing;
			/**
			 *  Tells if current cursor position is in the form's resize area, where can be dragged to resize the form
			 */
			bool m_isInReiszeArea;

			/** Specify the cursor position difference since last frame when dragging or resizing the form.
			*/
			Point m_posOffset;
			Point m_oldSize;

			bool m_initialized;

			float m_lastClickTime;


			BorderStyle m_borderStyle;
			WindowState m_state;
		};

		/**
		 *  The manager class for Forms and Panels. And it is also responsible
		 *  for drawing and updating them.
		 *  Any forms should be added to this class.
		 */
		class APAPI UIRoot
		{
		public:
			/** 
			 *  Specifies the area to display UI in the viewport. In unified coordinates.
			 */
			static RectangleF UIArea;

			static RectangleF MaximizedArea;

			/**
			 *  Return the area to display UI in the viewport. In screen coordinates.
			 */
			static Apoc3D::Math::Rectangle GetUIArea(RenderDevice* device);

			static FastList<Form*>& getForms() { return m_forms; }

			/**
			 *  The active form is the one that is currently being dragged/clicked.
			 *  Used to mark the first form being touched.
			 */
			static Form* getActiveForm() { return m_activeForm; }
			static void setActiveForm(Form* frm) { m_activeForm = frm; }
			
			static Menu* getMainMenu() { return m_mainMenu; }
			static void setMainMenu(Menu* mm) { m_mainMenu = mm; }

			static Form* getTopMostForm() { return m_topMostForm; }
			static void setTopMostForm(Form* frm) { m_topMostForm = frm; }
			static Form* getModalForm() { return m_modalForm; }
			static void setModalForm(Form* frm) { m_modalForm = frm; }

			static bool GetMinimizedPosition(RenderDevice* dev, Form* form, Point& pos);
			static Point GetMaximizedSize(RenderDevice* dev, Form* form);
			static Apoc3D::Math::Rectangle GetMaximizedRect(RenderDevice* dev, Form* form);
			static bool IsObstructed(Control* control, const Point& point);
			
			static void Initialize(RenderDevice* device);
			static void Finalize();

			static void Add(ControlContainer* cc);
			static void Remove(ControlContainer* cc);
			static void RemoveForm(const String& name);
			static void RemoveContainer(const String& name);

			static void Draw();
			static void Update(const GameTime* const time);

			static Point ClampFormMovementOffset(Form* frm, const Point& vec);

		private:
			static FastList<Form*> m_forms;
			static FastList<ControlContainer*> m_containers;
			static Form* m_activeForm;
			static Form* m_topMostForm;
			static SubMenu* m_contextMenu;
			static Sprite* m_sprite;
			static Menu* m_mainMenu;
			static Form* m_modalForm;
			static int m_modalAnim;

			static void Form_SizeChanged(Control* ctl);
		};
	}
}
#endif