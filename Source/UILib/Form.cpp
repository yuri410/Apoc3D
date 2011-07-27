#include "Form.h"
#include "Menu.h"
#include "Graphics/RenderSystem/RenderDevice.h"

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

			InitializeButtons();

			ControlContainer::Initialize(device);

			m_initialized = true;
		}

		void Form::InitializeButtons()
		{

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