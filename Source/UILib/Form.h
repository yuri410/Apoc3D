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

#ifndef FORM_H
#define FORM_H

#include "UICommon.h"
#include "Control.h"
#include "Collections/FastList.h"

using namespace  Apoc3D::Collections;

namespace Apoc3D
{
	namespace UI
	{
		class Form : public ControlContainer
		{
		public:
			enum BorderStyle
			{
				FBS_None,
				FBS_Fixed,
				FBS_Sizable
			};
			enum WindowState
			{
				FWS_Normal,
				FWS_Minimized,
				FWS_Maximized
			};

		private:
			Point m_titleOffset;
			Point m_minimumSize;
			Point m_minimizedSize;
			Point m_maximumSize;

			bool m_isMinimized;
			bool m_isMaximized;

			Point m_previousPosition;
			Point m_previousSize;
			Point m_minimizedPos;

			String m_title;

			UIEventHandler m_eResized;



			float m_borderAlpha;

			Button* m_btClose;
			Button* m_btMinimize;
			Button* m_btMaximize;
			Button* m_btRestore;

			bool m_hasMinimizeButton;
			bool m_hasMaximizeButton;

			Rectangle m_dragArea;
			Rectangle m_resizeArea;
			
			bool m_isDragging;
			bool m_isResizeing;

			bool m_isMinimizing;
			bool m_isInReiszeArea;

			Point m_posOffset;
			Point m_oldSize;

			bool m_initialized;

			float m_lastClickTime;

			BorderStyle m_borderStyle;
			WindowState m_state;


			void InitializeButtons();

			void DrawTitle(Sprite* sprite);
			void DrawButtons(Sprite* sprite);

			void UpdateTopMost();
			void UpdateActive();
			void UpdateState();
			void CheckDragging();
			void CheckDoubleClick(const GameTime* const time);
			void CheckResize();
			void ToggleWindowState();

		public:
			bool isResized() const { return m_isResizeing; }
			bool isDragged() const { return m_isDragging; }
			bool isMinimizing() const { return m_isMinimizing; }

			bool getHasMinimizeButton() const { return m_hasMinimizeButton; }
			void setHasMinimizeButton(bool val) { m_hasMinimizeButton = val; }

			bool getHasMaximizeButton() const { return m_hasMaximizeButton; }
			void setHasMaximizeButton(bool val) { m_hasMaximizeButton = val; }

			const String& getTitle() const { return m_title; }
			void setTitle(const String& txt) { m_title = txt; }



			UIEventHandler& eventResized() { return m_eResized; }

			Form();
			Form(BorderStyle border);
			virtual ~Form();

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


			
		};

		class UIRoot
		{
		private:
			static FastList<Form*> m_forms;
			static Form* m_activeForm;
			static Form* m_topMostForm;

			

		};
	}
}
#endif