#pragma once
#ifndef DIP_H
#define DIP_H

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

#include "DIPCommon.h"

namespace dip
{
    class DIP1 : public Apoc3DEx::Game
	{
	public:
		static const int DMCount = 8;
		DIP1(RenderWindow* wnd);
		~DIP1();


		virtual void Initialize();
		virtual void Finalize();

		virtual void Load();
		virtual void Unload();
		virtual void Update(const GameTime* time);
		virtual void Draw(const GameTime* time);

		virtual void OnFrameStart();
		virtual void OnFrameEnd();

		Texture* getSourceTexture() const { return m_original; }
		Texture* getSourceTexture512() const { return m_original512; }
		Texture* getSourceTexture256() const { return m_original256; }

	private:
		void MenuItem_OpenDemo(MenuItem* mi);

		void Work13(MenuItem* ctrl);
		void Work13_Process(Button* ctrl);


		void Work21(MenuItem* ctrl);
		void Work21_SubProcess(Texture* original, Texture* resultTex1, Texture* resultTex2, Texture* resultTex3,
			int thres1, int thres2, int thres3);
		void Work21_Process(Button* ctrl);

		void Work22(MenuItem* ctrl);
		void Work22_Subprocess(const Complex* source, const FreqDomainFilter& filter, Texture* resultTex);
		void Work22_Process(Button* ctrl);
		void Work22_SelImage(ComboBox* ctrl);

		void Work23(MenuItem* ctrl);
		void Work23_Process(Button* ctrl);
		void Work23_SelImage(ComboBox* ctrl);


		void ShowAbout(MenuItem* ctrl);

		void WorkDM(MenuItem* ctrl);
		void WorkDM_Process(Button* ctrl);

		Texture* m_white16;

		Texture* m_colorPhoto;
		Texture* m_original256;
		Texture* m_original512;
		Texture* m_original;

		Texture* m_originalGrad;
		Texture* m_originalAlt;
		Texture* m_originalAperture256;
		Texture* m_originalAlt256;

		List<SubDemo*> m_subdemos;
		
		Form* m_frmW13;
		Texture* m_w13result;


		Form* m_frmW21;
		Texture* m_w21result1_1;
		Texture* m_w21result1_2;
		Texture* m_w21result1_3;
		Texture* m_w21result2_1;
		Texture* m_w21result2_2;
		Texture* m_w21result2_3;
		TextBox* m_w21Thres1_1;
		TextBox* m_w21Thres1_2;
		TextBox* m_w21Thres1_3;
		TextBox* m_w21Thres2_1;
		TextBox* m_w21Thres2_2;
		TextBox* m_w21Thres2_3;

		Form* m_frmW22;
		Texture* m_w22resultAmp;
		Texture* m_w22resultPhase;

		Texture* m_w22result2_1;
		Texture* m_w22result1_1;
		Texture* m_w22result2_2;
		Texture* m_w22result1_2;
		TextBox* m_w22Thres1_1;
		TextBox* m_w22Thres2_1;
		ComboBox* m_w22ImageSel;
		PictureBox* m_w22OriginalImage;

		Form* m_frmW23;
		ListView* m_w23result;
		Label* m_w23comRate;
		PictureBox* m_w23OriginalImage;
		ComboBox* m_w23ImageSel;



		CheckBox* m_dmInitCluster[8];
		Texture* m_dmOriginals[8];
		Texture* m_dmSpecturm[8];
		List<PictureBox*> m_resultDisplays;
		PictureBox* m_dmPicBoxSpecturm[9];
		Form* m_frmDM;



		Form* m_aboutDlg;

		MenuBar* m_mainMenu;

		Sprite* m_sprite;
    };
}
#endif
