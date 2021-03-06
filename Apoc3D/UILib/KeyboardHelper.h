#pragma once
#ifndef APOC3D_KEYBOARDHELPER_H
#define APOC3D_KEYBOARDHELPER_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
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
			bool ShiftDown = false;
			bool ControlDown = false;
			bool AltDown = false;

			bool CapsLock = false;
			bool NumLock = false;

			KeyboardEventsArgs() { }
		};
		
		typedef EventDelegate<String> PasteEventHandler;
		typedef EventDelegate<KeyboardKeyCode, KeyboardEventsArgs> KeyboardEventHandler;

		class APAPI KeyboardHelper
		{
		public:
			KeyboardHelper() { }

			void Update(const AppTime* time);

			bool isShiftDown() const { return m_shiftDown; }

			KeyboardEventHandler eventKeyPress;
			KeyboardEventHandler eventKeyRelease;
			PasteEventHandler eventKeyPaste;

			EventDelegate<> eventKeyboardSelectionStart;
			EventDelegate<> eventKeyboardSelectionEnd;
		private:
			KeyboardKeyCode m_currentKey = KEY_UNASSIGNED;
			KeyboardKeyCode m_previousKey = KEY_UNASSIGNED;
			bool m_pasting = false;
			bool m_shiftDown = false;

			bool m_timerStarted = false;
			float m_pressingTime = 0;
		};

		class APAPI TextEditState
		{
		public:
			TextEditState(bool multiline, const Point& cursorPos = Point::Zero);
			~TextEditState();

			void Update(const AppTime* time);

			void Add(const String& text);
			void SetText(const String& text, bool keepCursor);
			bool GetLineSelectionRegion(int32 line, int32& start, int32& end);

			const String& getText() const { return m_text; }
			const Collections::List<String>& getLines() const { return m_lines; }
			const String& getCurrentLine() const { return m_lines[m_cursorLocation.Y]; }

			const Point& getCursorPosition() const { return m_cursorLocation; }
			int32 getLineCount() const { return m_multiline ? m_lines.getCount() : 1; }

			void MoveCursorToEnd();
			void MoveCursorTo(const Point& cp);

			void StartExternalSelection();
			void EndExternalSelection();

			void ClearSelectionRegion();

			void SetSelectionEndFromCursor();
			void SetSelectionFromCursorWord();

			void GetFixedSelectionRegion(Point& fixedStart, Point& fixedEnd);

			bool isExternalSelecting() const { return m_isExternalSelecting; }
			bool hasSelection() const { return m_selectionStart != m_selectionEnd; }
			
			KeyboardEventHandler& eventKeyPress() { return m_keyboard.eventKeyPress; }
			KeyboardEventHandler& eventKeyRelease() { return m_keyboard.eventKeyRelease; }
			PasteEventHandler& eventKeyPaste() { return m_keyboard.eventKeyPaste; }

			bool AllowMouseSelection = false;
			bool AllowSelection = false;

			EventDelegate<> eventEnterPressed;
			EventDelegate<> eventContentChanged;
			EventDelegate<> eventUpPressedSingleline;
			EventDelegate<> eventDownPressedSingleline;

		private:
			bool isKeyboardSelecting() const { return m_keyboard.isShiftDown(); }

			void ClampCursorPos();
			void ClampCursorPos(Point& cp);

			void Keyboard_OnPress(KeyboardKeyCode code, KeyboardEventsArgs e);
			void Keyboard_OnPaste(String value);

			void EraseSelectedText();

			String& GetLines(String*& prevLine, String*& nextLine);

			void Sync();

			KeyboardHelper m_keyboard;

			String m_text;
			Collections::List<String> m_lines;	// used for multiline editing

			Point m_cursorLocation;
			bool m_multiline = false;
			bool m_changed = false;

			bool m_isExternalSelecting = false;
			Point m_selectionStart;
			Point m_selectionEnd;

			Point m_textSelectionStart;
		};
	}
}

#endif