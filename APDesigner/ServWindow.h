#pragma once
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

#include "APDCommon.h"

namespace APDesigner
{
	class ServWindow : public RenderWindowHandler
	{
	public:
		ServWindow(RenderWindow* wnd, const String& projectFilePath);
		~ServWindow();

		virtual void Initialize();
		virtual void Finalize();

		virtual void Load();
		virtual void Unload();
		virtual void Update(const GameTime* time);
		virtual void Draw(const GameTime* time);

		virtual void OnFrameStart() { }
		virtual void OnFrameEnd() { }

	private:
		void BtnHide_Release(Button* ctrl);
		void BtnBuild_Release(Button* ctrl);
		void BtnExit_Release(Button* ctrl);

		void Notify(const String& title, const String& message);
		void UpdateProject();

		String GetProjectStampFilePath();

		StyleSkin* m_UIskin;

		RenderWindow* m_window;
		RenderDevice* m_device;

		Sprite* m_sprite;
		String m_projectFilePath;

		Form* m_baseForm;
		Label* m_lblDescription;
		Label* m_lblLoadedProject;
		Label* m_lblStatus;

		Label* m_lblHotKeys;
		Button* m_btnHide;
		Button* m_btnExit;
		Button* m_btnBuild;

		bool m_lastHideKeyPressed;
		bool m_lastBuildKeyPressed;

		String m_queuedMessage;

		List<char*> m_changeBuffer;
		Project* m_currentProject;
	};
}
