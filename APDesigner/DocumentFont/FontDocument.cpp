#include "FontDocument.h"
#include "APDesigner/MainWindow.h"
#include "APDesigner/UIResources.h"
#include "APDesigner/CommonDialog/ChooseColor.h"

#include "apoc3d/Core/ResourceHandle.h"

#include "apoc3d/Config/XmlConfigurationFormat.h"

#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Input/Mouse.h"

#include "apoc3d/IOLib/ModelData.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/IOLib/BinaryReader.h"

#include "apoc3d/Graphics/Animation/AnimationData.h"
#include "apoc3d/Graphics/Animation/AnimationManager.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/RenderStateManager.h"
#include "apoc3d/Graphics/RenderSystem/RenderTarget.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/RenderSystem/VertexElement.h"
#include "apoc3d/Graphics/RenderSystem/Buffer/HardwareBuffer.h"
#include "apoc3d/Graphics/EffectSystem/EffectManager.h"
#include "apoc3d/Graphics/ModelManager.h"
#include "apoc3d/Graphics/Material.h"
#include "apoc3d/Graphics/Model.h"
#include "apoc3d/Graphics/Mesh.h"
#include "apoc3d/Graphics/Camera.h"

#include "apoc3d/Scene/SceneRenderer.h"
#include "apoc3d/Scene/SceneProcedure.h"
#include "apoc3d/Scene/ScenePass.h"
#include "apoc3d/Scene/SceneObject.h"

#include "apoc3d/UILib/Form.h"
#include "apoc3d/UILib/Button.h"
#include "apoc3d/UILib/PictureBox.h"
#include "apoc3d/UILib/Label.h"
#include "apoc3d/UILib/FontManager.h"
#include "apoc3d/UILib/ComboBox.h"
#include "apoc3d/UILib/StyleSkin.h"
#include "apoc3d/UILib/CheckBox.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/FileLocateRule.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Vfs/File.h"

#include <strstream>
#include <sstream>

using namespace Apoc3D::Input;
using namespace Apoc3D::Utility;
using namespace Apoc3D::IO;
using namespace APDesigner::CommonDialog;

namespace APDesigner
{
	FontDocument::FontDocument(MainWindow* window, EditorExtension* ext, const String& file)
		: Document(window, ext), m_filePath(file)
	{
		m_glyphDistribution = new PictureBox(Point(15, 27), 1);
		m_glyphDistribution->Size = Point(512,512);
		m_glyphDistribution->SetSkin(window->getUISkin());
		m_glyphDistribution->eventPictureDraw().Bind(this, &FontDocument::GlyphDistribution_Draw);


		getDocumentForm()->setMinimumSize(Point(1000,600));

		getDocumentForm()->setTitle(file);
	}
	FontDocument::~FontDocument()
	{
		delete m_glyphDistribution;
	}

	void FontDocument::LoadRes()
	{
		m_regions.Clear();

		FileStream* fs = new FileStream(m_filePath);
		BinaryReader* br = new BinaryReader(fs);

		int count = br->ReadInt32();
		for (int i=0;i<count;i++)
		{
			uint32 a = br->ReadUInt32();
			uint32 b = br->ReadUInt32();
			m_regions.Add(std::make_pair(a,b));
		}
		br->Close();
		delete br;
	}
	void FontDocument::SaveRes()
	{

	}
	

	void FontDocument::Initialize(RenderDevice* device)
	{
		getDocumentForm()->getControls().Add(m_glyphDistribution);

		Document::Initialize(device);
	}
	void FontDocument::Update(const GameTime* const time)
	{
		Document::Update(time);

		m_glyphDistribution->Size = getDocumentForm()->Size - Point(30, 50);
	}
	void FontDocument::Render()
	{

	}


	void FontDocument::GlyphDistribution_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		Texture* white = getMainWindow()->getUISkin()->WhitePixelTexture;
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

		for (int i=0;i<EdgeCellCount;i++)
		{
			for (int j=0;j<EdgeCellCount;j++)
			{
				std::wostringstream stream;
				stream.width(2);
				stream.fill('0');
				stream.imbue(std::locale::classic());
				stream.setf ( std::ios::hex, std::ios::basefield );       // set hex as the basefield

				stream << (i*EdgeCellCount+j);

				int cx = j * CellSize + CellSize/4;
				int cy = i * CellSize + CellSize/5;
				String txt = stream.str();
				StringUtils::ToUpperCase(txt);

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