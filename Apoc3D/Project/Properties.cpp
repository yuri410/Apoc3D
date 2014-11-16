#include "Properties.h"

#include "apoc3d/Collections/List.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	void ProjectTextureResizingOptions::Parse(const String& text)
	{
		List<String> p = StringUtils::Split(text, L", ");

		float* ratioParts[3] = { &m_newWidthRatio, &m_newHeightRatio, &m_newDepthRatio };
		int32* absParts[3] = { &m_newWidth, &m_newHeight, &m_newDepth };

		for (int32 i = 0; i < p.getCount(); i++)
		{
			String& n = p[i];

			bool isRatio = n.find('%') != String::npos;

			if (isRatio)
			{
				StringUtils::TrimRight(n, L"%");
				*(ratioParts[i]) = StringUtils::ParseSingle(n) / 100;
			}
			else
			{
				*(absParts[i]) = StringUtils::ParseInt32(n);
			}
		}
	}
	String ProjectTextureResizingOptions::ToString() const
	{
		float ratioParts[3] = { m_newWidthRatio, m_newHeightRatio, m_newDepthRatio };
		int32 absParts[3] = { m_newWidth, m_newHeight, m_newDepth };

		int32 totalPartCount = 0;
		for (int32 i = 0; i < countof(absParts); i++)
		{
			if (ratioParts[i] != 0 || absParts[i] != 0)
			{
				totalPartCount++;
			}
		}

		String result;
		for (int32 i = 0; i < countof(absParts); i++)
		{
			bool hasVal = false;

			if (ratioParts[i] != 0)
			{
				result.append(StringUtils::SingleToString(ratioParts[i]*100));
				result.append(1, '%');
				hasVal = true;
			}
			else if (absParts[i])
			{
				result.append(StringUtils::IntToString(absParts[i]));
				hasVal = true;
			}

			if (hasVal && i < totalPartCount-1)
			{
				result.append(L", ");
			}
		}
		
		if (result.empty())
			return L"[none]";

		return result;
	}

	int32 ProjectTextureResizingOptions::GetResizedWidth(int32 cw) const
	{
		if (m_newWidth == 0 && m_newWidthRatio == 0)
			return cw;

		return m_newWidth ? m_newWidth : Math::Round(m_newWidthRatio * cw);
	}
	int32 ProjectTextureResizingOptions::GetResizedHeight(int32 ch) const
	{
		if (m_newHeight == 0 && m_newHeightRatio == 0)
			return ch;

		return m_newHeight ? m_newHeight : Math::Round(m_newHeightRatio * ch);
	}
	int32 ProjectTextureResizingOptions::GetResizedDepth(int32 cd) const
	{
		if (m_newDepth == 0 && m_newDepthRatio == 0)
			return cd;

		return m_newDepth ? m_newDepth : Math::Round(m_newDepthRatio * cd);
	}

	bool ProjectTextureResizingOptions::IsResizing() const
	{
		return m_newWidth != 0 || m_newHeight != 0 || m_newDepth != 0 ||
			m_newWidthRatio != 0 || m_newHeightRatio != 0 || m_newDepthRatio != 0;
	}
}