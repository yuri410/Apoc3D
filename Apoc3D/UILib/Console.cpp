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

#include "Console.h"
#include "Form.h"
#include "Text.h"
#include "Button.h"
#include "PictureBox.h"
#include "FontManager.h"
#include "Bar.h"
#include "StyleSkin.h"

#include "apoc3d/Core/CommandInterpreter.h"
#include "apoc3d/Graphics/TextureManager.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Input/Mouse.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Library/tinythread.h"

#include <ctime>

using namespace Apoc3D::Utility;
using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace UI
	{
		Console::Console(RenderDevice* device,StyleSkin* skin,const Point& position, const Point& size)
			: m_skin(skin), m_needsUpdateLineInfo(false), m_contendLineCount(0), m_currentSelectedPreviousCommands(-1)
		{
			m_logLock = new tthread::mutex();

			m_form = new Form(skin, device, FBS_Sizable, L"Console");
			m_form->Position = position;
			m_form->setSize(size);

			int32 lineHeight = skin->ContentTextFont->getLineHeightInt();

			m_inputText = new TextBox(skin, Point(10, size.Y - 40), size.X - 100, L"");
			m_inputText->eventEnterPressed.Bind(this, &Console::TextBox_ReturnPressed);
			m_inputText->eventUpPressedSingleline.Bind(this, &Console::TextBox_UpPressed);
			m_inputText->eventDownPressedSingleline.Bind(this, &Console::TextBox_DownPressed);
			m_form->getControls().Add(m_inputText);

			m_submit = new Button(skin, size - Point(100, 32), L"Submit");
			m_submit->SetSizeY(lineHeight);
			m_submit->eventRelease.Bind(this, &Console::Submit_Pressed);
			

			m_form->getControls().Add(m_submit);

			m_pictureBox = new PictureBox(skin, Point(10, 5 + skin->FormTitle->Height), 1);
			m_pictureBox->eventPictureDraw.Bind(this, &Console::PictureBox_Draw);
			m_form->getControls().Add(m_pictureBox);

			m_scrollBar = new ScrollBar(skin, m_pictureBox->Position + Point(m_pictureBox->getWidth(), 0),
				BarDirection::Vertical, m_pictureBox->getHeight());
			
			m_scrollBar->IsInverted = true;
			m_form->getControls().Add(m_scrollBar);

			m_form->setMinimumSize(Point(400,300));
			m_form->eventResized.Bind(this, &Console::Form_Resized);

			RegisterCommands();

			{
				LogSet* log = LogManager::getSingleton().getLogSet(LOG_System);
				for (LogSet::Iterator iter = log->begin(); iter != log->end(); iter++)
				{
					Log_New(*iter);
				}
			}
			
			
			LogManager::getSingleton().eventNewLogWritten.Bind(this, &Console::Log_New);
			SystemUI::Add(m_form);
			m_form->Show();
		}

		Console::~Console()
		{
			LogManager::getSingleton().eventNewLogWritten.Unbind(this, &Console::Log_New);
			SystemUI::Remove(m_form);

			UnregisterCommands();

			delete m_form;
			delete m_pictureBox;
			delete m_submit;
			delete m_inputText;
			delete m_logLock;
			delete m_scrollBar;
		}

		void Console::Update(const AppTime* time)
		{
			const Point& size = m_form->getSize();
			m_inputText->Position.Y = size.Y - 35;
			m_inputText->setWidth(size.X - 100);
			
			m_submit->Position = size - Point(75, 35);
			m_pictureBox->setSize(size - Point(20, 75));

			//m_scrollBar->Position = Point(m_pictureBox->Position.X + m_pictureBox->Size.X - 14, m_pictureBox->Position.Y);
			m_scrollBar->Position = m_pictureBox->Position + Point(m_pictureBox->getWidth() - m_scrollBar->getWidth(), 0);
			m_scrollBar->SetLength(m_pictureBox->getHeight());

			m_logLock->lock();
			while (m_queuedNewLogs.getCount() > 0)
			{
				LogEntry e = m_queuedNewLogs.Dequeue();
				eventNewLogReceived.Invoke(&e);

				m_logs.PushBack(e);
				m_needsUpdateLineInfo = true;

				while (m_logs.getCount() > MaxLogEntries)
				{
					m_logs.PopFront();
				}
			}
			m_logLock->unlock();


			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			if (mouse->getDZ() && m_pictureBox->getAbsoluteArea().Contains(mouse->GetPosition()))
			{
				m_scrollBar->SetValue(m_scrollBar->getValue() + mouse->getDZ() / 60);
			}
		}
		void Console::TextBox_ReturnPressed(TextBox* ctrl)
		{
			Submit_Pressed(nullptr);
		}
		void Console::TextBox_UpPressed(TextBox* ctrl)
		{
			if (m_previousCommands.getCount()>0)
			{
				if (m_currentSelectedPreviousCommands == -1)
				{
					m_currentSelectedPreviousCommands = m_previousCommands.getCount()-1;
				}
				else if (m_currentSelectedPreviousCommands>0)
				{
					m_currentSelectedPreviousCommands--;
				}

				m_inputText->SetText(m_previousCommands[m_currentSelectedPreviousCommands]);
			}
			
		}
		void Console::TextBox_DownPressed(TextBox* ctrl)
		{
			if (m_previousCommands.getCount()>0)
			{
				if (m_currentSelectedPreviousCommands == -1)
				{
					m_currentSelectedPreviousCommands = 0;
				}
				else if (m_currentSelectedPreviousCommands<m_previousCommands.getCount()-1)
				{
					m_currentSelectedPreviousCommands++;
				}

				m_inputText->SetText(m_previousCommands[m_currentSelectedPreviousCommands]);
			}
		}

		void Console::setPosition(const Point& pt)
		{
			m_form->Position = pt;
		}
		const Point& Console::getSize() const
		{
			return m_form->getSize();
		}
		void Console::Maximize()
		{
			m_form->Maximize();
		}
		void Console::Minimize()
		{
			m_form->Minimize();
		}
		void Console::Restore()
		{
			m_form->Restore();
		}
		void Console::Close()
		{
			m_form->Close();
		}

		void Console::Submit_Pressed(Button* ctrl)
		{
			const String& c = m_inputText->getText();

			if (c.size())
			{
				// this is OK. this is just sent to the GUI only.
				LogEntry le(0, 0, c, LOGLVL_Default, LOG_Command);
				Log_New(le);

				CommandInterpreter::getSingleton().RunLine(c, true);
			}

			{
				int32 idx = m_previousCommands.IndexOf(c);
				if (idx != -1)
					m_previousCommands.RemoveAt(idx);

				m_previousCommands.Add(c);
				m_currentSelectedPreviousCommands = -1;
			}
			
			m_inputText->SetText(L"");
		}

		void Console::PictureBox_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
		{
			if (m_form->getState() == Form::FWS_Minimized)
				return;


			sprite->Draw(SystemUI::GetWhitePixel(), *dstRect, nullptr, CV_PackLA(25, 255));

			Font* font = m_form->getFont();
			float lineSpacing = font->getLineHeight() + font->getLineGap();

			int textWidth = dstRect->Width - 5 - m_scrollBar->getWidth();

			if (m_needsUpdateLineInfo)
			{
				m_contendLineCount = 0;
				int counter = 0;
				for (const LogEntry& e : m_logs)
				{
					String str = e.ToString();

					m_entryInfo[counter].LineCount = font->CalculateLineCount(str, textWidth);
					m_entryInfo[counter].Message = str;
					m_contendLineCount += m_entryInfo[counter].LineCount;

					ColorValue color = 0;
					switch (e.Level)
					{
						case LOGLVL_Fatal:
							color = CV_Purple;
							break;
						case LOGLVL_Error:
							color = CV_Red;
							break;
						case LOGLVL_Warning:
							color = CV_Orange;
							break;
						case LOGLVL_Infomation:
							color = CV_White;
							break;
						case LOGLVL_Default:
							color = 0xffa3ff91; // light green
							break;
					}
					m_entryInfo[counter].Color = color;
					counter++;
				}
				m_needsUpdateLineInfo = false;
			}


			int windowLineCount = static_cast<int>(dstRect->Height / lineSpacing);
			m_scrollBar->Maximum = m_contendLineCount - windowLineCount;
			m_scrollBar->VisibleRange = windowLineCount;

			int startIndex = m_logs.getCount() - 1;
			if (startIndex >= 0)
			{
				for (int i = 0; i < m_scrollBar->getValue() && startIndex >= 0;)
				{
					i += m_entryInfo[startIndex].LineCount;
					startIndex--;
				}
			}

			int x = dstRect->X + 5;
			int y = dstRect->Y + dstRect->Height - 5;
			for (int i = startIndex; i >= 0 && y >= 0; i--)
			{
				y -= m_entryInfo[i].LineCount * static_cast<int>(lineSpacing);

				font->DrawString(sprite, m_entryInfo[i].Message, x, y, textWidth, m_entryInfo[i].Color);
			}
		}

		void Console::Log_New(LogEntry e)
		{
			m_logLock->lock();
			m_queuedNewLogs.Enqueue(e);
			m_logLock->unlock();
		}

		void Console::Form_Resized(Control* ctrl)
		{
			m_needsUpdateLineInfo = true;
		}

		void Console::RegisterCommands()
		{
			//ConsoleCommandSet* s = new ConsoleCommandSet(this);
			CommandInterpreter::getSingleton().RegisterCommand({ L"clear", 0, CommandHandler(this, &Console::ClearCommand) });
		}

		void Console::UnregisterCommands()
		{
			CommandInterpreter::getSingleton().UnregisterCommand({ L"clear", 0, CommandHandler(this, &Console::ClearCommand) });
		}

		void Console::ClearCommand(const List<String>& args)
		{
			m_logs.Clear();
		}

	}
}