
#ifndef MOUSEINTERFACE_H
#define MOUSEINTERFACE_H

#include "Common.h"
#include "Math/Point.h"

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace UI
	{
		class APAPI MouseInterface
		{
		private:
			bool m_lastBtnState[3];
			bool m_btnState[3];

			Point m_lastPosition;
			Point m_currentPos;
		public:
			static const Point& GetCurrentPosition() { return m_currentPos; }
			static bool IsLeftPressed() { return m_btnState[0] & !m_lastBtnState[0]; }
			static bool IsLeftUp() { return !m_btnState[0] & m_lastBtnState[0]; }
			static bool IsRightPressed() { return m_btnState[2] & !m_lastBtnState[2]; }
			static bool IsRightUp() { return !m_btnState[2] & m_lastBtnState[2]; }

			static void Update(const GameTime* const time){}
		};
	}
}
#endif