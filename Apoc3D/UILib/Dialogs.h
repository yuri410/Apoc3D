#pragma once
#ifndef APOC3D_UI_DIALOGS_H
#define APOC3D_UI_DIALOGS_H

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

#include "apoc3d/ApocCommon.h"
#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Collections/List.h"
#include "apoc3d/Meta/EventDelegate.h"

namespace Apoc3D
{
	namespace UI
	{
		enum DialogResult
		{
			DLGRES_None = 0,
			DLGRES_OK = 1 << 0,
			DLGRES_Yes = 1 << 1,
			DLGRES_Abort = 1 << 2,
			DLGRES_Retry = 1 << 3,
			DLGRES_Ignore = 1 << 4,
			DLGRES_No = 1 << 5,
			DLGRES_Cancel = 1 << 6
		};

		class APAPI MessageDialogBox
		{
		public:
			static const int MsgBox_OK = DLGRES_OK;
			static const int MsgBox_OKCancel = DLGRES_OK | DLGRES_Cancel;
			static const int MsgBox_YesNo = DLGRES_Yes | DLGRES_No;
			typedef EventDelegate<DialogResult> InputHandler;

			MessageDialogBox(Apoc3D::Graphics::RenderSystem::RenderDevice* device, const StyleSkin* skin,
				const String& title, const String& text, int buttonsFlags);
			~MessageDialogBox();

			void ShowModal();

			DialogResult getResult() const { return m_result; }
			bool isActive() const;

			InputHandler eventInputResponsed;
		private:
			void AddButton(const String& caption, const StyleSkin* skin, DialogResult dr);
			void Button_Release(Button* ctrl);

			Apoc3D::Graphics::RenderSystem::RenderDevice* m_renderDevice;

			Form* m_form;
			Label* m_content;

			Apoc3D::Collections::List<Button*> m_controlButtons;
			Apoc3D::Collections::HashMap<void*, DialogResult> m_buttonMapping;

			DialogResult m_result;
		};

		class APAPI InputDialogBox
		{
		public:
			typedef EventDelegate<InputDialogBox*> ConfirmInputHandler;

			InputDialogBox(Apoc3D::Graphics::RenderSystem::RenderDevice* device, const StyleSkin* skin,
				const String& title, const String& text, bool multiline);
			~InputDialogBox();

			void ShowModal();
			bool isActive() const;

			DialogResult getResult() const { return m_result; }
			const String& getTextInput() const;

			ConfirmInputHandler eventInputConfirmed;
		private:
			void AddButton(const String& caption, const StyleSkin* skin, DialogResult dr);
			void Button_OkRelease(Button* ctrl);
			void Button_CancelRelease(Button* ctrl);

			Apoc3D::Graphics::RenderSystem::RenderDevice* m_renderDevice;

			Form* m_form;
			Label* m_content;
			TextBox* m_inputField;

			Button* m_btnOk;
			Button* m_btnCancel;

			DialogResult m_result;
		};
	}
}

#endif