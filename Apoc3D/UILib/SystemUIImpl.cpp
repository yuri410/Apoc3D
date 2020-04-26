#include "SystemUIImpl.h"
#include "Form.h"
#include "Menu.h"
#include "Button.h"
#include "StyleSkin.h"
#include "FontManager.h"

#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/RenderSystem/RenderStateManager.h"
#include "apoc3d/Graphics/TextureManager.h"
#include "apoc3d/Vfs/FileLocateRule.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Input/Mouse.h"
#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Core/AppTime.h"

using namespace Apoc3D::VFS;
using namespace Apoc3D::Input;

namespace Apoc3D
{
	namespace UI
	{
		SystemUIImpl::SystemUIImpl()
			: m_forms(10), m_containers(10)
		{

		}
		SystemUIImpl::~SystemUIImpl()
		{

		}

		Apoc3D::Math::Rectangle SystemUIImpl::GetUIArea(RenderDevice* device)
		{
			Viewport vp = device->getViewport();

			Apoc3D::Math::Rectangle rect;
			rect.X = (int)(UIArea.X * vp.Width);
			rect.Y = (int)(UIArea.Y * vp.Height);
			rect.Width = (int)(UIArea.Width * vp.Width);
			rect.Height = (int)(UIArea.Height * vp.Height);
			return rect;
		}

		bool SystemUIImpl::GetMinimizedPosition(RenderDevice* dev, Form* form, Point& pos)
		{
			for (int i = 0; i < m_forms.getCount(); i++)
			{
				if (m_forms[i] != form && m_forms[i]->isMinimizing())
				{
					return false;
				}
			}

			Apoc3D::Math::Rectangle rect = GetUIArea(dev);

			for (int y = rect.Height - 20; y > 0; y -= 20)
			{
				for (int x = 0; x < rect.Width - 99; x += 100)
				{
					bool isOccupied = false;
					for (int i = 0; i < m_forms.getCount(); i++)
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
						pos = Point(x, y);
						return true;
					}
				}
			}
			pos = Point::Zero;
			return true;
		}

		Apoc3D::Math::Rectangle SystemUIImpl::GetMaximizedRect(RenderDevice* dev, Form* form)
		{
			Point size = GetMaximizedSize(dev, form);

			if (m_mainMenu)
			{
				size.Y -= m_mainMenu->getSize().Y;
				Apoc3D::Math::Rectangle result(0, m_mainMenu->getSize().Y, size.X, size.Y);
				return result;
			}
			else
			{
				Apoc3D::Math::Rectangle result(0, 0, size.X, size.Y);
				return result;
			}
		}

		Point SystemUIImpl::GetMaximizedSize(RenderDevice* dev, Form* form)
		{
			Viewport vp = dev->getViewport();

			Apoc3D::Math::Rectangle rect;
			rect.X = (int)(UIArea.X * MaximizedArea.X * vp.Width);
			rect.Y = (int)(UIArea.Y * MaximizedArea.Y * vp.Height);
			rect.Width = (int)(UIArea.Width * MaximizedArea.Width * vp.Width);
			rect.Height = (int)(UIArea.Height * MaximizedArea.Height * vp.Height);

			for (Form* frm : m_forms)
			{
				if (frm != form && frm->Visible && frm->getState() == Form::FWS_Minimized && frm->Position.Y < rect.getBottom())
				{
					rect.Height = frm->Position.Y - rect.Y;
				}
			}

			return Point(rect.Width, rect.Height);
		}
		

		Point SystemUIImpl::ClampFormMovementOffset(Form* frm, const Point& vec)
		{
			if (frm != m_modalForm)
			{
				RenderDevice* dev = frm->getRenderDevice();

				Apoc3D::Math::Rectangle rect = GetMaximizedRect(dev, frm);
				Apoc3D::Math::Rectangle formArea = frm->getAbsoluteArea();
				formArea.Width = 80;
				formArea.Height = 20;

				Point result = vec;

				if (formArea.X + result.X < rect.X)
					result.X = rect.X - formArea.X;

				if (formArea.Y + result.Y < rect.Y)
					result.Y = rect.Y - formArea.Y;

				if (formArea.getRight() + result.X > rect.getRight())
					result.X = rect.getRight() - formArea.getRight();

				if (formArea.getBottom() + result.Y > rect.getBottom())
					result.Y = rect.getBottom() - formArea.getBottom();

				//Apoc3D::Math::Rectangle formArea(newPos.X, newPos.Y, newSize.X, newSize.Y);

				return result;
			}
			return vec;
		}
		
		void SystemUIImpl::Form_SizeChanged(Form* form)
		{
			Apoc3D::Math::Rectangle area = GetUIArea(form->getRenderDevice());

			Point frmSize = form->getSize();

			{
				//If a form is out of the working area,
				//we need to put it back where the user can see it.
				if (!area.Contains(form->getAbsoluteArea()))
				{
					if (form->Position.X + frmSize.X > area.Width)
						form->Position.X = area.Width - frmSize.X;

					if (form->Position.X + frmSize.Y > area.Height)
						form->Position.Y = area.Height - frmSize.Y;
				}

				//If a form was maximized
				if (form->getState() == Form::FWS_Maximized)
				{
					//resize it
					//form->Size.X = area.Width;
					//form->Size.Y = area.Height;
					form->setSize(area.getSize());
				}
			}
		}



		void SystemUIImpl::Initialize(RenderDevice* device)
		{
			ObjectFactory* fac = device->getObjectFactory();
			m_sprite = fac->CreateSprite();

			m_white1x1 = fac->CreateTexture(1, 1, 1, TU_Static, FMT_A8R8G8B8);
			m_white1x1->FillColor(CV_White);
		}

		void SystemUIImpl::Finalize()
		{
			DELETE_AND_NULL(m_sprite);
			DELETE_AND_NULL(m_white1x1);
		}

		void SystemUIImpl::Draw()
		{
			FontManager::getSingleton().StartFrame();
			
			m_sprite->Begin((Sprite::SpriteSettings)(Sprite::SPRMix_ManageStateAlphaBlended | Sprite::SPR_UseTransformStack));

			// first background forms
			for (int i = m_forms.getCount() - 1; i > -1; i--)
			{
				Form* frm = m_forms[i];
				if (frm->Visible &&
					frm->getState() != Form::FWS_Minimized && frm->IsBackgroundForm)
				{
					frm->Draw(m_sprite);
				}
			}


			// regular forms
			for (int i = m_forms.getCount() - 1; i > -1; i--)
			{
				Form* frm = m_forms[i];
				if (frm->Visible && frm != TopMostForm &&
					frm->getState() != Form::FWS_Minimized && !frm->IsBackgroundForm)
				{
					frm->Draw(m_sprite);
				}
			}

			if (TopMostForm && TopMostForm->Visible && !TopMostForm->IsBackgroundForm)
			{
				TopMostForm->Draw(m_sprite);
			}

			// minimized always last
			for (int i = m_forms.getCount() - 1; i >= 0; i--)
			{
				Form* frm = m_forms[i];
				if (frm->getState() == Form::FWS_Minimized &&
					frm->Visible && frm != TopMostForm)
				{
					frm->Draw(m_sprite);
				}
			}

			if (m_contextMenu && m_contextMenu->Visible && m_contextMenu->getState() != MENU_Closed)
			{
				m_contextMenu->Draw(m_sprite);
			}
			if (m_mainMenu && m_mainMenu->Visible)
			{
				m_mainMenu->Draw(m_sprite);
			}

			if (m_modalForm)
			{
				if (m_modalAnim > 3)
				{
					m_sprite->Draw(m_white1x1, GetUIArea(m_sprite->getRenderDevice()), 0, CV_PackColor(0, 0, 0, 180));
				}
				else if (m_modalAnim)
				{
					m_sprite->Draw(m_white1x1, GetUIArea(m_sprite->getRenderDevice()), 0, CV_PackColor(0, 0, 0, 225));
				}

				m_modalForm->Draw(m_sprite);
			}


			// cursor
			m_sprite->End();
		}

		void SystemUIImpl::Update(const AppTime* time)
		{
			InteractingForm = nullptr;

			if (m_modalForm)
			{
				m_modalForm->Update(time);
				m_modalAnim++;
				return;
			}

			m_modalAnim = 0;


			bool menuOverriden = false;
			if (m_contextMenu && m_contextMenu->getState() != MENU_Closed &&
				m_contextMenu->Visible)
			{
				m_contextMenu->Update(time);
				menuOverriden = true;
			}
			if (m_mainMenu && m_mainMenu->Visible)
			{
				m_mainMenu->Update(time);
				menuOverriden = m_mainMenu->getState() == MENU_Open;
			}

			if (menuOverriden)
			{
				for (Form* frm : m_forms)
					frm->IsInteractive = false;
			}
			else
			{
				for (Form* frm : m_forms)
					frm->IsInteractive = true;
			}


			Form* alreadyUpdatedForm = nullptr;
			if (TopMostForm)
			{
				alreadyUpdatedForm = TopMostForm;
				TopMostForm->Update(time);
			}

			for (Form* frm : m_forms)
			{
				if (frm->Enabled && frm != alreadyUpdatedForm)
				{
					frm->Update(time);
				}
			}
			
			// execute bring to front request
			for (ControlContainer* cc : m_bringFrontSchedules)
			{
				int32 idx = m_forms.IndexOf(reinterpret_cast<Form*>(cc));
				if (idx == -1)
					idx = m_containers.IndexOf(cc);

				if (idx == -1)
					continue; // invalid pointer

				Form* form = up_cast<Form*>(cc);
				if (form)
				{
					if (m_forms.getCount() > 0 && m_forms[0] != form)
					{
						m_forms.RemoveAt(idx);
						m_forms.Insert(0, form);
					}
				}
				else
				{
					if (m_containers.getCount() > 0 && m_containers[0] != cc)
					{
						m_containers.RemoveAt(idx);
						m_containers.Insert(0, form);
					}
				}
			}
			m_bringFrontSchedules.Clear();

		}


		void SystemUIImpl::Add(ControlContainer* cc)
		{
			Form* form = up_cast<Form*>(cc);
			if (form)
			{
				m_forms.Insert(0, form);
			}
			else
			{
				m_containers.Add(cc);
			}
		}
		void SystemUIImpl::Remove(ControlContainer* cc)
		{
			Form* form = up_cast<Form*>(cc);
			if (form)
			{
				m_forms.Remove(form);
			}
			else
			{
				m_containers.Remove(cc);
			}
		}
		void SystemUIImpl::RemoveForm(const String& name)
		{
			for (int i = 0; i < m_forms.getCount(); i++)
			{
				if (m_forms[i]->Name == name)
				{
					m_forms.RemoveAt(i);
					break;
				}
			}
		}
		void SystemUIImpl::RemoveContainer(const String& name)
		{
			for (int i = 0; i < m_containers.getCount(); i++)
			{
				if (m_containers[i]->Name == name)
				{
					m_containers.RemoveAt(i);
					break;
				}
			}
		}


		void SystemUIImpl::ScheduleBringFront(ControlContainer* cc)
		{
			m_bringFrontSchedules.Add(cc);
		}

	}
}