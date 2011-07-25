#include "Form.h"

namespace Apoc3D
{
	namespace UI
	{
		Form::Form()
			: m_titleOffset(12,2), m_minimumSize(100, 40), m_minimizedSize(100, 20),
			m_maximumSize(0,0), m_isMinimized(false), m_isMaximized(false),
			m_previousPosition(0,0), m_previousSize(0,0), m_minimizedPos(0,0), 
			m_hasMinimizeButton(true), m_hasMaximizeButton(true), 
			m_dragArea(0,0,0,0), m_resizeArea(0,0,15,15), m_isDragging(false), m_isResizeing(false),
			m_isMinimizing(false), m_isInReiszeArea(false), m_posOffset(0,0), m_oldSize(0,0), m_initialized(false), m_lastClickTime(0),
			m_borderStyle(FBS_Sizable), m_state(FWS_Normal)
		{

		}
		Form::Form(BorderStyle border)
		{

		}

		Form::~Form()
		{

		}

		void Form::Show()
		{
			
		}
		void Form::Hide()
		{

		}
		void Form::Close()
		{

		}
		void Form::Focus()
		{

		}
		void Form::Unfocus()
		{

		}
		void Form::Minimize()
		{

		}
		void Form::Maximize()
		{

		}
		void Form::Restore()
		{

		}
	}
}