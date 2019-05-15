#pragma once
#ifndef APOC3D_CONSOLE_H
#define APOC3D_CONSOLE_H

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

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Collections/Queue.h"
#include "apoc3d/Math/ColorValue.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Core;
using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	namespace UI
	{
		class APAPI Console
		{
		public:
			typedef EventDelegate<LogEntry*> ConsoleNewLog;
			
			
			static const int MaxLogEntries = 500;

			Console(RenderDevice* device, StyleSkin* skin, const Point& position, const Point& size);
			~Console();

			void Minimize();
			void Restore();
			void Maximize();
			void Close();

			void Update(const AppTime* time);

			void setPosition(const Point& pt);
			const Point& getSize() const;

			ConsoleNewLog eventNewLogReceived;

		private:
			void TextBox_ReturnPressed(TextBox* ctrl);
			void TextBox_UpPressed(TextBox* ctrl);
			void TextBox_DownPressed(TextBox* ctrl);

			void Submit_Pressed(Button* ctrl);

			

			void Form_Resized(Control* ctrl);
			void PictureBox_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect);
			void Log_New(LogEntry e);

			void RegisterCommands();
			void UnregisterCommands();

			void ClearCommand(const List<String>& args);
			//void Console_CommandSubmited(String cmd, List<String>* args);

			Form* m_form;
			TextBox* m_inputText;
			Button* m_submit;
			PictureBox* m_pictureBox;
			StyleSkin* m_skin;
			ScrollBar* m_scrollBar;
			LinkedList<LogEntry> m_logs;
			Queue<LogEntry> m_queuedNewLogs;

			List<String> m_previousCommands;
			int32 m_currentSelectedPreviousCommands;

			bool m_needsUpdateLineInfo;
			int m_contendLineCount;
			struct 
			{
				int LineCount;
				String Message;
				ColorValue Color;
			} m_entryInfo[MaxLogEntries];


			std::mutex m_logLock;

		};
	}
}

#endif