#include "FontDocument.h"
#include "APDesigner/MainWindow.h"
#include "APDesigner/UIResources.h"
#include "APDesigner/CommonDialog/ChooseColor.h"

#include "apoc3d/Core/ResourceHandle.h"
#include "FontDocument.h"

using namespace APDesigner::CommonDialog;

namespace APDesigner
{
	FontDocument::FontDocument(MainWindow* window, EditorExtension* ext, const String& file)
		: Document(window, ext), m_filePath(file)
	{
		m_glyphDistribution = new PictureBox(window->getUISkin(), Point(10, 27), 1);
		m_glyphDistribution->setSize(512, 512);
		
		m_glyphDistribution->eventPictureDraw.Bind(this, &FontDocument::GlyphDistribution_Draw);

		m_sampleRender = new Label(window->getUISkin(), Point(10, 27), L"", 512);

		getDocumentForm()->ReleaseControls = true;
		getDocumentForm()->setMinimumSize(Point(550,600));

		getDocumentForm()->setTitle(file);
	}
	FontDocument::~FontDocument()
	{
	}

	void FontDocument::LoadRes()
	{
		m_sampleRender->SetFont(FontManager::getSingleton().getFont(L"english"));
		DELETE_AND_NULL(m_font);
		
		m_regions.Clear();
		m_glyphCount = 0;

		FileLocation fl(m_filePath);
		m_font = new Font(getMainWindow()->getDevice(), fl);

		m_glyphDistribution->Position.Y += m_font->getLineHeightInt();
		getDocumentForm()->setHeight(getDocumentForm()->getHeight() + m_font->getLineHeightInt());

		uint lastIdx = 0;
		for (uint i = 0; i < 65535; i++)
		{
			short left, top;
			float advx;

			bool exists = m_font->LookupCharacterSetting(i, left, top, advx);
			if (!exists)
			{
				if (i - lastIdx > 1)
				{
					// put session
					m_regions.Add(std::make_pair(lastIdx + 1, i - 1));
				}
				lastIdx = i;
			}
			else
			{
				m_glyphCount++;
			}
		}
		m_sampleRender->SetFont(m_font);
		m_sampleRender->SetText(L"Glyph Count: " + StringUtils::UIntToString(m_glyphCount));
	}
	void FontDocument::SaveRes()
	{

	}
	

	void FontDocument::Initialize(RenderDevice* device)
	{
		getDocumentForm()->getControls().Add(m_glyphDistribution);
		getDocumentForm()->getControls().Add(m_sampleRender);

		Document::Initialize(device);
	}
	void FontDocument::Update(const AppTime* time)
	{
		Document::Update(time);

		m_glyphDistribution->setSize(getDocumentForm()->getSize() - Point(m_glyphDistribution->Position.X + 10, m_glyphDistribution->Position.Y + 10));
	}
	void FontDocument::Render()
	{

	}


	void FontDocument::GlyphDistribution_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		Texture* white = SystemUI::GetWhitePixel();
		sprite->Draw(white, *dstRect, nullptr, CV_White);

		Matrix orginalTrans;
		if (!sprite->isUsingStack())
		{
			orginalTrans = sprite->getTransform();
		}


		Matrix offTrans;
		Matrix::CreateTranslation(offTrans, (float)dstRect->X + 7, (float)dstRect->Y + 7, 0);

		sprite->MultiplyTransform(offTrans);
		

		const int CellSize = 32;
		const int EdgeCellCount = 16;

		Font* defFont = FontManager::getSingleton().getFont(L"english");
	

		for (int i=0;i<m_regions.getCount();i++)
		{
			//int idx = m_regions[i].first;
			int startX = (m_regions[i].first / (4096/(EdgeCellCount*CellSize)));
			int endX = (m_regions[i].second / (4096/(EdgeCellCount*CellSize)));
			startX %= EdgeCellCount*CellSize;
			endX %= EdgeCellCount*CellSize;

			if (endX < startX)
			{
				int row1 = (m_regions[i].first >> 8) / EdgeCellCount;
				int row2 = (m_regions[i].second >> 8) / EdgeCellCount;

				Apoc3D::Math::Rectangle drect1(startX, row1*CellSize, EdgeCellCount*CellSize-startX, CellSize);
				sprite->Draw(white, drect1, nullptr, CV_Red);

				for (int i=row1+1;i<row2-1;i++)
				{
					Apoc3D::Math::Rectangle drect(0, row1*CellSize, EdgeCellCount*CellSize, CellSize);
					sprite->Draw(white, drect, nullptr, CV_Red);
				}

				
				Apoc3D::Math::Rectangle drect2(0, row2*CellSize, endX, CellSize);
				sprite->Draw(white, drect2, nullptr, CV_Red);
			}
			else if (startX<=endX)
			{
				int row = (m_regions[i].first >> 8) / EdgeCellCount;
				Apoc3D::Math::Rectangle drect(startX, row*CellSize, endX-startX+1, CellSize);
				sprite->Draw(white, drect, nullptr, CV_Red);
			}
		}

		for (int i=0;i<=EdgeCellCount;i++)
		{
			sprite->Draw(white, Apoc3D::Math::Rectangle(0,i*CellSize,CellSize*EdgeCellCount,1), nullptr, CV_Black);
		}
		for (int j=0;j<=EdgeCellCount;j++)
		{
			sprite->Draw(white, Apoc3D::Math::Rectangle(j*CellSize,0,1,CellSize*EdgeCellCount), nullptr, CV_Black);
		}

		for (int i = 0; i < EdgeCellCount; i++)
		{
			for (int j = 0; j < EdgeCellCount; j++)
			{
				int cx = j * CellSize + CellSize / 4;
				int cy = i * CellSize + CellSize / 5;
				String txt = StringUtils::UIntToStringHex((uint32)(i*EdgeCellCount + j), StrFmt::a<2, '0'>::val | StringUtils::SF_UpperCase);

				defFont->DrawString(sprite, txt, Point(cx, cy), CV_Black);
			}
		}

		if (sprite->isUsingStack())
		{
			sprite->PopTransform();
		}
		else
		{
			sprite->SetTransform(orginalTrans);
		}
	}

}