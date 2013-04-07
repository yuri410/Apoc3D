/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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
#include "Console.h"
#include "Form.h"
#include "Label.h"
#include "Button.h"
#include "PictureBox.h"
#include "FontManager.h"
#include "Scrollbar.h"
#include "StyleSkin.h"

#include "apoc3d/Core/CommandInterpreter.h"
#include "apoc3d/Graphics/TextureManager.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Math/Math.h"
#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Input/Mouse.h"
#include "apoc3d/Utility/StringUtils.h"

#include "tthread/tinythread.h"

using namespace Apoc3D::Utility;
using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace UI
	{
		Console::Console(RenderDevice* device,StyleSkin* skin,const Point& position, const Point& size)
			: m_skin(skin), m_needsUpdateLineInfo(false), m_contendLineCount(0)
		{
			m_logLock = new tthread::mutex();

			m_form = new Form(FBS_Sizable, L"Console");
			m_form->Position = position;
			m_form->Size = size;
			m_form->SetSkin(skin);

			int32 lineHeight = static_cast<int32>( FontManager::getSingleton().getFont(skin->ControlFontName)->getLineHeight());

			m_inputText = new TextBox(Point(10, size.Y - 40), size.X - 100, L"");
			m_inputText->SetSkin(skin);
			m_inputText->eventReturnPressed().bind(this, &Console::TextBox_ReturnPressed);
			m_form->getControls().Add(m_inputText);

			m_submit = new Button(Point(size.X - 100, size.Y - 30-2), L"Submit");
			m_submit->SetSkin(skin);
			m_submit->eventRelease().bind(this,&Console::Submit_Pressed);
			m_submit->Size.Y = lineHeight;

			m_form->getControls().Add(m_submit);

			m_pictureBox = new PictureBox(Point(10,10+17), 1);
			m_pictureBox->SetSkin(skin);
			m_pictureBox->eventPictureDraw().bind(this, &Console::PictureBox_Draw);
			m_form->getControls().Add(m_pictureBox);

			m_scrollBar = new ScrollBar(Point(m_pictureBox->Position.X + m_pictureBox->Size.X - 12, m_pictureBox->Position.Y),
				ScrollBar::SCRBAR_Vertical, m_pictureBox->Size.Y);
			m_scrollBar->SetSkin(skin);
			m_scrollBar->setIsInverted(true);
			m_form->getControls().Add(m_scrollBar);

			m_form->setMinimumSize(Point(400,300));
			m_form->Initialize(device);
			m_form->eventResized().bind(this, &Console::Form_Resized);

			RegisterCommands();

			{
				Log* log = LogManager::getSingleton().getLogSet(LOG_System);
				for (Log::Iterator iter = log->begin();iter!=log->end();iter++)
				{
					Log_New(*iter);
				}
			}
			
			
			LogManager::getSingleton().eventNewLogWritten().bind(this, &Console::Log_New);
			UIRoot::Add(m_form);
			m_form->Show();
		}

		Console::~Console()
		{
			LogManager::getSingleton().eventNewLogWritten().clear();
			UIRoot::Remove(m_form);

			delete m_form;
			delete m_pictureBox;
			delete m_submit;
			delete m_inputText;
			delete m_logLock;
		}

		void Console::Update(const GameTime* const time)
		{
			const Point& size = m_form->Size;
			m_inputText->Position.Y = size.Y - 35;
			m_inputText->setWidth(size.X - 100);
			
			m_submit->Position = Point(size.X - 75, size.Y - 35);
			m_pictureBox->Size = size-Point(20, 75);

			//m_scrollBar->Position = Point(m_pictureBox->Position.X + m_pictureBox->Size.X - 14, m_pictureBox->Position.Y);
			m_scrollBar->setPosition(Point(m_pictureBox->Position.X + m_pictureBox->Size.X - 12, m_pictureBox->Position.Y));
			m_scrollBar->setHeight(m_pictureBox->Size.Y);

			m_logLock->lock();
			while (m_queuedNewLogs.getCount()>0)
			{
				LogEntry e = m_queuedNewLogs.Dequeue();
				m_logs.PushBack(e);
				m_needsUpdateLineInfo = true;

				while (m_logs.Size()>MaxLogEntries)
				{
					m_logs.PopFront();
				}
			}
			m_logLock->unlock();


			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			if (mouse->getDZ() && m_pictureBox->getAbsoluteArea().Contains(mouse->GetCurrentPosition()))
			{
				m_scrollBar->setValue(Math::Clamp(m_scrollBar->getValue() + mouse->getDZ() / 60, 0, m_scrollBar->getMax()));
			}
		}
		void Console::TextBox_ReturnPressed(Control* ctrl)
		{
			Submit_Pressed(ctrl);
		}
		void Console::setPosition(const Point& pt)
		{
			m_form->Position = pt;
		}
		const Point& Console::getSize() const
		{
			return m_form->Size;
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

		void Console::Submit_Pressed(Control* ctrl)
		{
			const String& c = m_inputText->Text;

			if (c.size())
			{
				LogEntry le(0, c, LOGLVL_Default, LOG_Command);
				Log_New(le);

				CommandInterpreter::getSingleton().RunLine(c, true);
			}

			m_inputText->setText(L"");
		}

		void Console::PictureBox_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
		{
			if (m_form->getState() == Form::FWS_Minimized)
				return;

			
			sprite->Draw(m_skin->WhitePixelTexture, *dstRect,0, PACK_COLOR(25,25,25,255));

			Font* font = m_form->getFontRef();
			float lineSpacing = font->getLineHeight() + m_form->getFontRef()->getLineGap();

			int textWidth = dstRect->Width - m_scrollBar->getBarWidth() - 15;

			if (m_needsUpdateLineInfo)
			{
				m_contendLineCount = 0;
				int counter = 0;
				for (LinkedList<LogEntry>::Iterator iter = m_logs.Begin(); iter != m_logs.End(); iter++ )
				{
					const LogEntry& e = *iter;
					String str = e.ToString();
					
					m_entryInfo[counter].LineCount = font->CalculateLineCount(str,textWidth);
					m_entryInfo[counter].Message = str;
					m_contendLineCount+= m_entryInfo[counter].LineCount;

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
						color = 0xffa3ff91;//CV_Green;
						break;
					}
					m_entryInfo[counter].Color = color;
					counter++;
				}
				m_needsUpdateLineInfo = false;
			}
			

			int windowLineCount = static_cast<int>( dstRect->Height / lineSpacing);
			m_scrollBar->setMax(m_contendLineCount - windowLineCount);

			int startIndex = (int)m_logs.Size() - 1;
			if (startIndex>=0)
			{
				for (int i=0;i<m_scrollBar->getValue() && startIndex>=0;)
				{
					i += m_entryInfo[startIndex].LineCount;
					startIndex--;
				}
			}

			int x = dstRect->X + 5;
			int y = dstRect->Y + dstRect->Height - 5;
			for (int i=startIndex; i>=0 && y>=0 ;i--)
			{
				y -= m_entryInfo[i].LineCount * static_cast<int>(lineSpacing);

				font->DrawString(sprite, m_entryInfo[i].Message, x,y,textWidth, m_entryInfo[i].Color);
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

		//////////////////////////////////////////////////////////////////////////

		class ClearCommand : public Command
		{
		public:
			ClearCommand(Console* c);

			virtual void Execute(const Apoc3D::Collections::List<String>& args);

		private:
			Console* m_console;
		};

		class ConsoleCommandSet : public Command
		{
		public:
			ConsoleCommandSet(Console* c);

			virtual void Execute(const Apoc3D::Collections::List<String>& args);
		
		private:
			ClearCommand m_clearCmd;
			Console* m_console;
		};


		void Console::RegisterCommands()
		{
			ConsoleCommandSet* s = new ConsoleCommandSet(this);
			CommandInterpreter::getSingleton().RegisterCommand(s);
		}

		//////////////////////////////////////////////////////////////////////////
		
		ConsoleCommandSet::ConsoleCommandSet(Console* c)
			: m_console(c), m_clearCmd(c)
		{
			m_desc.Name = L"Console Command Set";
			m_desc.CommandName = L"console";
			m_desc.NumOfParameters = 0;
			m_desc.SubCommands.Add(new ClearCommand(c));
		}

		void ConsoleCommandSet::Execute(const Apoc3D::Collections::List<String>& args)
		{
			LogManager::getSingleton().Write(LOG_CommandResponse, m_desc.Name, LOGLVL_Infomation);
		}

		ClearCommand::ClearCommand(Console* c)
			: m_console(c)
		{
			m_desc.Name = L"Console Clear";
			m_desc.CommandName = L"clear";
			m_desc.NumOfParameters = 0;
		}
		void ClearCommand::Execute(const Apoc3D::Collections::List<String>& args)
		{
			m_console->m_logs.Clear();
		}


	}
}