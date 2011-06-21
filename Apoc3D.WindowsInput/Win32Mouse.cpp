#include "Win32Mouse.h"
#include "Core/GameTime.h"

namespace Apoc3D
{
	namespace Input
	{
		namespace Win32
		{
			Win32Mouse::Win32Mouse(OIS::InputManager* mgr)
				: m_inpMgr(mgr)
			{
				m_mouse = static_cast<OIS::Mouse*>(mgr->createInputObject(OIS::OISMouse, true));
			}
			Win32Mouse::~Win32Mouse()
			{
				m_inpMgr->destroyInputObject(m_mouse);
			}

			void Win32Mouse::Update(const GameTime* const time)
			{
				memcpy(m_lastBtnState, m_btnState, sizeof(m_btnState));
				m_lastPosition = m_currentPos;
				m_lastZ = m_z;
				
				m_mouse->capture();
			}

			bool Win32Mouse::mouseMoved( const OIS::MouseEvent &arg )
			{
				const OIS::MouseState& s = arg.state;
				m_currentPos.X = s.X.abs;
				m_currentPos.Y = s.Y.abs;
				m_z = s.Z.abs;

				return true;
			}
			bool Win32Mouse::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) 
			{
				const OIS::MouseState& s = arg.state;
				m_btnState[(int)id] = true;

				//std::cout << "\nMouse button #" << id << " pressed. Abs("
					//<< s.X.abs << ", " << s.Y.abs << ", " << s.Z.abs << ") Rel("
					//<< s.X.rel << ", " << s.Y.rel << ", " << s.Z.rel << ")";
				return true;
			}
			bool Win32Mouse::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
			{
				const OIS::MouseState& s = arg.state;

				m_btnState[(int)id] = false;

				//std::cout << "\nMouse button #" << id << " released. Abs("
					//<< s.X.abs << ", " << s.Y.abs << ", " << s.Z.abs << ") Rel("
					//<< s.X.rel << ", " << s.Y.rel << ", " << s.Z.rel << ")";
				return true;
			}
		}
	}
}