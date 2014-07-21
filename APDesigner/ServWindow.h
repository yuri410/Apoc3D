#pragma once
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
		virtual void Update(const GameTime* const time);
		virtual void Draw(const GameTime* const time);

		virtual void OnFrameStart() { }
		virtual void OnFrameEnd() { }

	private:
		void BtnHide_Release(Control* ctrl);
		void BtnBuild_Release(Control* ctrl);
		void BtnExit_Release(Control* ctrl);

		void Notify(const String& title, const String& message);
		void UpdateProject();

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
