#include "Form.h"
#include "Menu.h"
#include "Button.h"
#include "StyleSkin.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "Vfs/FileLocateRule.h"
#include "Vfs/FileSystem.h"
#include "Graphics/TextureManager.h"

using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace UI
	{

		Form::Form(BorderStyle border, const String& title)
			: m_titleOffset(12,2), m_minimumSize(100, 40), m_minimizedSize(100, 20),
			m_maximumSize(0,0), m_isMinimized(false), m_isMaximized(false),
			m_previousPosition(0,0), m_previousSize(0,0), m_minimizedPos(0,0), 
			m_hasMinimizeButton(true), m_hasMaximizeButton(true), 
			m_dragArea(0,0,0,0), m_resizeArea(0,0,15,15), m_isDragging(false), m_isResizeing(false),
			m_isMinimizing(false), m_isInReiszeArea(false), m_posOffset(0,0), m_oldSize(0,0), m_initialized(false), m_lastClickTime(0),
			m_borderStyle(border), m_state(FWS_Normal), m_title(title), m_menu(0), m_menuOffset(0,20)
		{
			Size = Point(200,100);
			Visible = false;
		}

		Form::~Form()
		{
			delete m_btClose;
			delete m_btMaximize;
			delete m_btMinimize;
			delete m_btRestore;

			if (UIRoot::getTopMostForm() == this)
			{
				UIRoot::setTopMostForm(0);
			}
		}

		void Form::Show()
		{
			m_state = FWS_Normal;

			if (m_previousPosition!= Point::Zero)
				Position = m_previousPosition;
			if (m_previousSize!=Point::Zero)
				Size = m_previousSize;

			Visible = true;
			Focus();
		}
		void Form::Hide()
		{
			Visible = false;
		}
		void Form::Close()
		{
			if (m_menu)
			{
				m_menu->Close();
			}

			if (UIRoot::getTopMostForm() == this)
				UIRoot::setTopMostForm(0);
		}
		void Form::Focus()
		{
			if (UIRoot::getTopMostForm() && UIRoot::getTopMostForm() != this && 
				UIRoot::getTopMostForm()->getMenu() && UIRoot.getTopMostForm()->getMenu()->getState() == MENU_Open)
			{
				return;
			}

			UIRoot::getForms().Remove(this);
			UIRoot::getForms().Insert(0, this);
			UIRoot::setTopMostForm(this);

		}
		void Form::Unfocus()
		{
			if (m_menu && m_menu->getState() != MENU_Closed)
			{
				m_menu->Close();
			}
		}
		void Form::Minimize()
		{
			Point minPos;
			bool ret = UIRoot::GetMinimizedPosition(m_device, this, minPos);
			if (ret)
				m_minimizedPos = minPos;
			else
				return;


			if (m_state == FWS_Normal)
			{
				m_previousSize = Position;
				m_previousSize = Size;
			}
			
			if (m_menu)
			{
				m_menu->Close();
				m_menu->Visible = false;
			}

			m_state = FWS_Minimized;
			m_isMinimizing = true;
		}
		void Form::Maximize()
		{
			if (m_state == FWS_Normal)
			{
				m_previousPosition = Position;
				m_previousSize = Size;
			}

			if (m_menu)
			{
				m_menu->Close();
			}

			Focus();

			m_maximumSize = UIRoot::GetMaximizedSize(m_device, this);
			m_state = FWS_Maximized;
		}
		void Form::Restore()
		{
			Focus();

			if (m_menu)
			{
				m_menu->Visible = true;
				m_menu->Close();
			}

			m_state = FWS_Normal;
		}

		void Form::Initialize(RenderDevice* device)
		{
			m_device = device;

			Apoc3D::Math::Rectangle rect = UIRoot::GetUIArea(device);
			m_maximumSize = Point(rect.Width, rect.Height);

			if (m_menu)
			{
				m_minimumSize.Y = 60;
				m_menu->setOwner(this);
				m_menu->Position = m_menuOffset;
				m_menu->Initialize(m_device);
			}

			InitializeButtons(device);

			ControlContainer::Initialize(device);

			m_initialized = true;
		}

		void Form::InitializeButtons(RenderDevice* device)
		{
			m_btClose = new Button(Point(Size.X = 22, 4), L"");
			m_btClose->setNormalTexture(m_skin->FormCloseButton);
			m_btClose->setOwner(this);
			m_btClose->Initialize(device);
			m_btClose->eventRelease().bind(this, Form::btClose_Release);

			
			if (m_hasMinimizeButton)
			{
				m_btMinimize = new Button(Point(0,0),L"");
				m_btMinimize->setNormalTexture(m_skin->FormMinimizeButton);
				m_btMinimize->setOwner(this);
				m_btMinimize->Initialize(device);

				m_btMinimize->eventRelease().bind(this, Form::btMinimize_Release);
			}
			if (m_hasMaximizeButton)
			{
				m_btMaximize = new Button(Point(0,0),L"");
				m_btMaximize->setNormalTexture(m_skin->FormMaximizeButton);
				m_btMaximize->setOwner(this);
				m_btMaximize->Initialize(device);
				m_btMaximize->eventRelease().bind(this, Form::btMaximize_Release);
			}
			if (m_hasMinimizeButton || m_hasMaximizeButton)
			{
				m_btRestore = new Button(Point(0,0), L"");
				m_btRestore->setNormalTexture(m_skin->FormRestoreButton);
				m_btRestore->setOwner(this);
				m_btRestore->Initialize(device);
				m_btRestore->eventRelease().bind(this, Form::btRestore_Release);
			}
		}


		void Form::btClose_Release(Control* sender)
		{
			Close();
		}
		void Form::btMinimize_Release(Control* sender)
		{
			Minimize();
		}
		void Form::btMaximize_Release(Control* sender)
		{
			Maximize();
		}
		void Form::btRestore_Release(Control* sender)
		{
			Restore();
		}

		void Form::Update(const GameTime* const time)
		{
			UpdateState();
			UpdateTopMost();

			if (m_state == FWS_Normal && !m_isMaximized && !m_isMinimized)
			{
				if (!m_isResizeing)
				{
					CheckDragging();
				}
				if (!m_isDragging && m_borderStyle == FBS_Sizable)
				{
					CheckResize();
				}
			}

			CheckDoubleClick(time);

			if (m_state != FWS_Minimized && !m_isMinimized)
			{
				if (Size.X < m_minimumSize.X)
					Size.X = m_minimumSize.X;
				if (Size.Y < m_minimumSize.Y)
					Size.Y = m_minimumSize.Y;
			}

			if (UIRoot::getTopMostForm() == this && (m_isMinimized && UIRoot::getActiveForm() == this))
			{
				if (m_menu && m_menu->Visible)
				{
					m_menu->Update(time);
				}

				if (m_btClose)
				{
					m_btClose->Update(time);
				}

				if (m_btMaximize && ((m_hasMaximizeButton && !m_isMaximized)||(m_hasMinimizeButton && m_isMinimized)))
				{
					m_btMaximize->Update(time);
				}
				if (m_btMinimize && ((m_hasMinimizeButton && !m_isMinimized)||(m_hasMaximizeButton && m_isMaximized)))
				{
					m_btMinimize->Update(time);
				}

				if (m_btRestore && ((m_hasMinimizeButton && m_isMinimized)||(m_hasMaximizeButton && m_isMaximized)))
				{
					m_btRestore->Update(time);
				}

				if (!m_menu || !m_menu->Visible || m_menu->getState() == FWS_Maximized)
				{
					if (m_state == FWS_Normal || m_state == FWS_Maximized)
					{
						bool skip = false;
						for (int i=0;i<m_controls->getCount();i++)
						{
							
						}
						if (!skip)
						{
							for (int i=0;i<m_controls->getCount();i++)
							{
								if (m_controls->operator[](i)->Enabled)
								{
									m_controls->operator[](i)->Update(time);
								}
							}
						}
					}
				}
			}
			UpdateActive();
			if (UIRoot::getActiveForm()!=this)
			{
				if (m_isResizeing) m_isResizeing = false;
				if (m_isDragging) m_isDragging = false;
			}
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		Apoc3D::Math::Rectangle UIRoot::GetUIArea(RenderDevice* device)
		{
			Viewport vp = device->getViewport();

			Apoc3D::Math::Rectangle rect;
			rect.X = (int)(UIArea.X * vp.Width);
			rect.Y = (int)(UIArea.Y * vp.Height);
			rect.Width = (int)(UIArea.Width * vp.Width);
			rect.Height = (int)(UIArea.Height * vp.Height);
			return rect;
		}

		bool UIRoot::GetMinimizedPosition(RenderDevice* dev, Form* form, Point& pos)
		{
			for (int i=0;i<m_forms.getCount();i++)
			{
				if (m_forms[i] != form && m_forms[i]->isMinimizing())
				{
					return false;
				}
			}

			Apoc3D::Math::Rectangle rect = GetUIArea(dev);

			for (int y = rect.Height -20;y>0;y-=20)
			{
				for (int x=0;x<rect.Width-99;x+=100)
				{
					bool isOccupied = false;
					for (int i=0;i<m_forms.getCount();i++)
					{
						if (m_forms[i] != form && m_forms[i]->Visible &&
							m_forms[i]->Position.X == x && m_forms[i]->Position.Y == y)
						{
							isOccupied = true;
							break;
						}
					}

					if (!isOccupied)
					{
						pos = Point(x,y);
						return true;
					}
				}
			}
			pos = Point::Zero;
			return true;
		}

		Point UIRoot::GetMaximizedSize(RenderDevice* dev, Form* form)
		{
			Apoc3D::Math::Rectangle rect = GetUIArea(dev);

			for (int i=0;i<m_forms.getCount();i++)
			{
				if (m_forms[i] != form && m_forms[i]->Visible && m_forms[i]->getState() == Form::FWS_Minimized)
					if (m_forms[i]->Position.Y < rect.getBottom())
						rect.Height = m_forms[i]->Position.Y - rect.Y;
			}

			return Point(rect.Width, rect.Height);
		}
	}
}