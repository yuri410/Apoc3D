
#ifndef CHOOSEFONT_H
#define CHOOSEFONT_H

#include "APDCommon.h"
#include "DialogCommon.h"


using namespace Apoc3D::Math;

namespace APDesigner
{
	namespace CommonDialog
	{
		class ChooseFontDialog
		{
		private:
			String m_fontFamilyName;
			float m_size;
			bool m_isBold;
			bool m_isItalic;
		public:

			const String& getFontFamilyName() const { return m_fontFamilyName; }
			float getSize() const { return m_size; }
			bool isBold() const { return m_isBold; }
			bool isItalic() const { return m_isItalic; }

			DialogResult ShowDialog();

		};
	}
}


#endif