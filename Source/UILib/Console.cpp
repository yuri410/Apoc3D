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
#include "Console.h"
#include "Form.h"
#include "Label.h"
#include "Button.h"
#include "PictureBox.h"
#include "FontManager.h"
#include "Scrollbar.h"
#include "Graphics/RenderSystem/Sprite.h"
#include "StyleSkin.h"
#include "Utility/StringUtils.h"

using namespace Apoc3D::Utility;
using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace UI
	{
		Console::Console(RenderDevice* device,StyleSkin* skin,const Point& position, const Point& size)
			: m_skin(skin)
		{
			m_form = new Form(FBS_Sizable, L"Console");
			m_form->Position = position;
			m_form->Size = size;
			m_form->SetSkin(skin);

			m_inputText = new TextBox(Point(10, size.Y - 40), size.X - 100, L"");
			m_inputText->SetSkin(skin);
			m_form->getControls().Add(m_inputText);

			m_submit = new Button(Point(size.X - 100, size.Y - 30), L"Submit");
			m_submit->SetSkin(skin);
			m_submit->eventRelease().bind(this,&Console::Submit_Pressed);
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
		}

		void Console::Update(const GameTime* const time)
		{
			const Point& size = m_form->Size;
			m_inputText->Position.Y = size.Y - 45;
			m_inputText->setWidth(size.X - 100);
			
			m_submit->Position = Point(size.X - 75, size.Y - 45);
			m_pictureBox->Size = size-Point(20, 80);

			//m_scrollBar->Position = Point(m_pictureBox->Position.X + m_pictureBox->Size.X - 14, m_pictureBox->Position.Y);
			m_scrollBar->setPosition(Point(m_pictureBox->Position.X + m_pictureBox->Size.X - 12, m_pictureBox->Position.Y));
			m_scrollBar->setHeight(m_pictureBox->Size.Y);


			m_logLock.lock();
			while (m_queuedNewLogs.getCount()>0)
			{
				LogEntry e = m_queuedNewLogs.Dequeue();
				m_logs.push_back(e);

				while (m_logs.size()>200)
				{
					m_logs.erase(m_logs.begin());
				}
			}
			m_logLock.unlock();

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
				List<String> args;

				int lastPos = -1;
				bool isInQuote = false;
				bool isSingleQuote = false;
				for (size_t i=0;i<c.size();i++)
				{
					wchar_t ch = c[i];
					if (ch == '\'')
					{
						if (!isInQuote)
						{
							isInQuote = true;
							isSingleQuote = true;
						}
						else if (isSingleQuote)
						{
							isInQuote = false;
						}
					}
					else if (ch == '"')
					{
						if (!isInQuote)
						{
							isInQuote = true;
							isSingleQuote = false;
						}
						else if (!isSingleQuote)
						{
							isInQuote = false;
						}
					}
					else if (ch == ' ' && !isInQuote)
					{
						if (i && (int)i-1 != lastPos)
						{
							args.Add(c.substr(lastPos+1, i-lastPos-1));
							lastPos = i;
						}
						else
						{
							lastPos = i;
						}
						
					}
				}
				if (lastPos != c.size()-1)
				{
					args.Add(c.substr(lastPos+1, c.size()-lastPos-1));
				}

				LogEntry le(0, c, LOGLVL_Default, LOG_Command);
				m_logs.push_back(le);

				if (!m_eCommandSubmited.empty())
				{
					String cmd = args[0];
					StringUtils::ToLowerCase(cmd);
					m_eCommandSubmited(cmd, &args);
				}
			}

			m_inputText->setText(L"");
		}

		void Console::PictureBox_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
		{
			if (m_form->getState() == Form::FWS_Minimized)
				return;

			int lineCount = dstRect->Height / m_form->getFontRef()->getLineHeight();
			m_scrollBar->setMax(m_logs.size() - lineCount);
			
			//int lineCount = dstRect->Height / m_form->getFontRef()->getLineHeight();
			//int maxLineCount = 200;

			sprite->Draw(m_skin->WhitePixelTexture, *dstRect,0, PACK_COLOR(25,25,25,255));

			Font* font = m_form->getFontRef();

			int x = dstRect->X + 5;
			int y = dstRect->Y + dstRect->Height - 5;

			std::list<LogEntry>::reverse_iterator iter = m_logs.rbegin();
			for (int i=0;i<m_scrollBar->getValue();i++)
			{
				iter++;
			}
			for (;iter!=m_logs.rend()&&y>0;iter++)
			{
				const LogEntry& e = *iter;

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
					color = CV_Green;
					break;
				}

				String str = e.ToString();
				Point size = font->MeasureString(str,dstRect->Width- 10);// font->DrawString(sprite, e.ToString(), x,y,dstRect->Width - 10, color);
				

				y -= size.Y-font->getLineHeight();

				font->DrawString(sprite, e.ToString(), x,y,dstRect->Width - 10, color);
			}
		}

		void Console::Log_New(LogEntry e)
		{
			m_logLock.lock();
			m_queuedNewLogs.Enqueue(e);
			m_logLock.unlock();
		}
	}
}