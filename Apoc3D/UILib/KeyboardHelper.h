#pragma once
#ifndef APOC3D_KEYBOARDHELPER_H
#define APOC3D_KEYBOARDHELPER_H

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

#include "apoc3d/Input/Keyboard.h"
#include "apoc3d/Collections/List.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Input;

namespace Apoc3D
{
	namespace UI
	{
		struct APAPI KeyboardEventsArgs
		{
			bool ShiftDown;
			bool ControlDown;
			bool AltDown;
			bool CapsLock;

			KeyboardEventsArgs()
				: ShiftDown(false), ControlDown(false), AltDown(false), CapsLock(false)
			{ }
		};
		
		typedef EventDelegate1<String> PasteEventHandler;
		typedef EventDelegate2<KeyboardKeyCode, KeyboardEventsArgs> KeyboardEventHandler;

		class APAPI KeyboardHelper
		{
		public:
			KeyboardHelper()
				: m_pasting(false), m_pressingTime(0), m_timerStarted(false), m_currentKey(KEY_UNASSIGNED), m_previousKey(KEY_UNASSIGNED)
			{ }

			void Update(const GameTime* time);

			KeyboardEventHandler eventKeyPress;
			KeyboardEventHandler eventKeyRelease;
			PasteEventHandler eventKeyPaste;

		private:
			KeyboardKeyCode m_currentKey;
			KeyboardKeyCode m_previousKey;
			bool m_pasting;

			bool m_timerStarted;
			float m_pressingTime;
		};

		class APAPI TextEditState
		{
		public:
			TextEditState(const Point& cursorPos, bool multiline);
			~TextEditState();

			void Update(const GameTime* time);

			void Add(const String& text);

			void SetText(const String& text);
			const String& getText() const { return m_text; }
			const Collections::List<String>& getLines() const { return m_lines; }
			const String& getCurrentLine() const { return m_lines[m_cursorLocation.Y]; }

			const Point& getCursorPosition() const { return m_cursorLocation; }
			int32 getLineCount() const { return m_multiline ? m_lines.getCount() : 1; }

			void MoveCursorToEnd();
			void MoveCursorTo(const Point& cp);

			KeyboardEventHandler& eventKeyPress() { return m_keyboard.eventKeyPress; }
			KeyboardEventHandler& eventKeyRelease() { return m_keyboard.eventKeyRelease; }
			PasteEventHandler& eventKeyPaste() { return m_keyboard.eventKeyPaste; }

			bool AllowMouseSelection = false;
			bool AllowSelection = false;

			EventDelegate0 eventEnterPressed;
			EventDelegate0 eventContentChanged;
			EventDelegate0 eventUpPressedSingleline;
			EventDelegate0 eventDownPressedSingleline;

		private:
			void Keyboard_OnPress(KeyboardKeyCode code, KeyboardEventsArgs e);
			void Keyboard_OnPaste(String value);

			KeyboardHelper m_keyboard;

			String m_text;
			Collections::List<String> m_lines;	// used for multiline editing

			Point m_cursorLocation;
			bool m_multiline = false;
			bool m_isDraggingSelecting = false;

			bool m_hasSelection = false;
			Point m_selectionStart;
			Point m_selectionEnd;

			Point m_textSelectionStart;
		};
	}
}

#endif