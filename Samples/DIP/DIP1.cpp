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
#include "DIP1.h"
#include "DIPMath.h"

#include "Demos/Histogram.h"
#include "Demos/Noise.h"
#include "Demos/Transformation.h"
#include "Demos/EdgeFilters.h"
#include "Demos/FreqDomainFilters.h"
#include "Demos/Huffman.h"
#include "Demos/Clustering.h"

#include "ImageLibrary.h"

namespace dip
{
    DIP1::DIP1(RenderWindow* wnd)
        : Game(wnd) { }

	DIP1::~DIP1()
	{		

	}

	void DIP1::Draw(const GameTime* time)
	{
		m_device->Clear(CLEAR_ColorAndDepth, CV_DarkGray, 1,0);
		m_device->BeginFrame();
		
		m_sprite->Begin((Sprite::SpriteSettings)(Sprite::SPR_RestoreState | Sprite::SPR_AlphaBlended));
		{
			Texture* colorPhoto = ImageLibrary::getSingleton().m_colorPhoto;
			Texture* white16 = ImageLibrary::getSingleton().m_white16;

			Apoc3D::Math::RectangleF dstRect = { 900, 50, 128, 128 };
			Apoc3D::Math::RectangleF srcRect = { 64, 64, 128, 128 };

			m_sprite->DrawRoundedRect(colorPhoto, dstRect, &srcRect, 25.0f, 4, CV_White);

			dstRect.Y += 192;
			m_sprite->DrawRoundedRectBorder(colorPhoto, dstRect, &srcRect, 2, 25.0f, 4, CV_White);


			dstRect.Y += 192;
			dstRect.Height *= 0.7f;
			m_sprite->DrawCircle(colorPhoto, dstRect, &srcRect, Math::PI*0.25f, Math::PI*1.75f, 16, CV_White);

			dstRect.Y += 192;
			m_sprite->DrawCircleArc(colorPhoto, dstRect, &srcRect, 5, Math::PI*0.25f, Math::PI*1.75f, 16, CV_White);
			
			dstRect.X -= 192;
			dstRect.Width = dstRect.Height = 4;
			m_sprite->DrawCircleArc(white16, dstRect, nullptr, 1, 6, CV_White);

			//dstRect.Y -= 192;
			{
				PointF startPos = dstRect.getPosition() - PointF(192, 0);
				PointF endPos = startPos + PointF(100, 0);

				m_sprite->DrawLine(colorPhoto, startPos, endPos, CV_White, 7, LineCapsOptions::Butt, PointF(1, 0.1f));

				startPos.Y -= 16; endPos.Y -= 16;
				m_sprite->DrawLine(colorPhoto, startPos, endPos, CV_White, 7, LineCapsOptions::Round, PointF(1, 0.1f));

				startPos.Y -= 16; endPos.Y -= 16;
				m_sprite->DrawLine(colorPhoto, startPos, endPos, CV_White, 7, LineCapsOptions::Square, PointF(1, 0.1f));
			}
		}

		m_sprite->End();
		
		for (int i = 0; i < m_subdemos.getCount(); i++)
		{
			if (m_subdemos[i] == nullptr)
				continue;

			m_subdemos[i]->Draw(time, m_sprite);
		}

		Game::Draw(time);


		m_device->EndFrame();

	}
	void DIP1::Initialize()
	{
		m_window->setTitle(L"DIP Demos");
		Game::Initialize();

		m_sprite = m_device->getObjectFactory()->CreateSprite();
	}
	void DIP1::Finalize()
	{
		Game::Finalize();
	}
	void DIP1::Load()
	{
		Game::Load();

		ImageLibrary::Initialize();
		ImageLibrary::getSingleton().Load(m_device);

		m_console->Minimize();

		m_subdemos.Add(new DemoHistogram(this, m_device, m_UIskin));
		m_subdemos.Add(new DemoNoise(this, m_device, m_UIskin));
		m_subdemos.Add(new DemoTransformation(this, m_device, m_UIskin));
		m_subdemos.Add(nullptr);

		m_subdemos.Add(new DemoEdgeFilters(this, m_device, m_UIskin));
		m_subdemos.Add(new DemoFreqDomainFilters(this, m_device, m_UIskin));
		m_subdemos.Add(new DemoHuffman(this, m_device, m_UIskin));
		m_subdemos.Add(new DemoClustering(this, m_device, m_UIskin));


		{
			m_aboutDlg = new Form(m_UIskin, m_device, FBS_Fixed, L"About");
			m_aboutDlg->setSize(460, 175);

			Label* lbl = new Label(m_UIskin, Point(30 + 128, 30),
				L"DIP Demos by Tao Xin. \n"
				L"Powered by Apoc3D.",
				m_aboutDlg->getWidth() - 40 - 128, TextHAlign::Left);
			
			PictureBox* photo = new PictureBox(m_UIskin, Point(15, 30), 1, ImageLibrary::getSingleton().m_colorPhoto);
			photo->setSize(128, 128);

			m_aboutDlg->getControls().Add(photo);
			m_aboutDlg->getControls().Add(lbl);

			SystemUI::Add(m_aboutDlg);
		}


		m_mainMenu = new MenuBar(m_UIskin);


		{
			MenuItem* pojMenu = new MenuItem(L"Demos");
			SubMenu* pojSubMenu = new SubMenu(m_UIskin, nullptr);

			for (SubDemo* sd : m_subdemos)
			{
				if (sd == nullptr)
				{
					MenuItem* mi = new MenuItem(L"-");
					pojSubMenu->Add(mi, nullptr);
				}
				else
				{
					MenuItem* mi = new MenuItem(sd->getName());
					mi->event.Bind(this, &DIP1::MenuItem_OpenDemo);
					mi->UserPointer = sd;
					pojSubMenu->Add(mi, nullptr);
				}
			}

			m_mainMenu->Add(pojMenu, pojSubMenu);
		}

		{
			MenuItem* aboutMenu = new MenuItem(L"Help");

			SubMenu* aboutSubMenu = new SubMenu(m_UIskin, nullptr);

			MenuItem* mi = new MenuItem(L"About...");
			mi->event.Bind(this, &DIP1::ShowAbout);
			aboutSubMenu->Add(mi, 0);

			m_mainMenu->Add(aboutMenu, aboutSubMenu);
		}

		SystemUI::MainMenu = m_mainMenu;

	}

	void DIP1::Unload()
	{
		Game::Unload();

		m_subdemos.DeleteAndClear();

		ImageLibrary::getSingleton().Free();
		ImageLibrary::Finalize();

		delete m_sprite;
	}

	void DIP1::MenuItem_OpenDemo(MenuItem* mi)
	{
		for (SubDemo* sd : m_subdemos)
		{
			if (sd == mi->UserPointer)
			{
				sd->Show();
				break;
			}
		}
	}

	void DIP1::Update(const GameTime* time)
	{
		Game::Update(time);

		for (int i = 0; i < m_subdemos.getCount();i++)
		{
			if (m_subdemos[i] == nullptr)
				continue;

			m_subdemos[i]->Update(time);
		}
	}

	void DIP1::ShowAbout(MenuItem* ctrl)
	{
		m_aboutDlg->ShowModal();
	}


	void DIP1::OnFrameStart() { }
	void DIP1::OnFrameEnd() { }

}
