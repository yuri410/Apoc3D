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
        : Application(wnd) { }

	DIP1::~DIP1()
	{		

	}

	void DIP1::Draw(const AppTime* time)
	{
		m_device->Clear(CLEAR_ColorAndDepth, CV_DarkGray, 1,0);
		m_device->BeginFrame();
		
		m_sprite->Begin(Sprite::SPRMix_ManageStateAlphaBlended);
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
				Font* fnt = FontManager::getSingleton().getFont(L"english");

				PointF pos = dstRect.getPosition() - PointF(256, 0);

				PointF startPos = pos + Point(92, 0);
				PointF endPos = startPos + PointF(100, 0);

				fnt->DrawString(m_sprite, L"Cap: Butt", startPos - Point(92, fnt->getLineHeightInt() / 2 + 2), CV_White);
				m_sprite->DrawLine(colorPhoto, startPos, endPos, CV_White, 8, LineCapOptions::Butt, PointF(1, 0.1f));

				startPos.Y += 16; endPos.Y += 16;
				fnt->DrawString(m_sprite, L"Cap: Round", startPos - Point(92, fnt->getLineHeightInt() / 2 + 2), CV_White);
				m_sprite->DrawLine(colorPhoto, startPos, endPos, CV_White, 8, LineCapOptions::Round, PointF(1, 0.1f));

				startPos.Y += 16; endPos.Y += 16;
				fnt->DrawString(m_sprite, L"Cap: Square", startPos - Point(92, fnt->getLineHeightInt() / 2 + 2), CV_White);
				m_sprite->DrawLine(colorPhoto, startPos, endPos, CV_White, 8, LineCapOptions::Square, PointF(1, 0.1f));
			}
		}

		m_sprite->End();
		
		for (int i = 0; i < m_subdemos.getCount(); i++)
		{
			if (m_subdemos[i] == nullptr)
				continue;

			m_subdemos[i]->Draw(time, m_sprite);
		}

		Application::Draw(time);


		m_device->EndFrame();

	}
	void DIP1::Initialize()
	{
		m_window->setTitle(L"DIP Demos");
		Application::Initialize();

		m_sprite = m_device->getObjectFactory()->CreateSprite();
	}
	void DIP1::Finalize()
	{
		Application::Finalize();
	}
	void DIP1::Load()
	{
		Application::Load();

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
		Application::Unload();

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

	void DIP1::Update(const AppTime* time)
	{
		Application::Update(time);

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
