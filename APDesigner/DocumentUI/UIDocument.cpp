#include "UIDocument.h"
#include "APDesigner/MainWindow.h"
#include "APDesigner/UIResources.h"
#include "APDesigner/CommonDialog/ChooseColor.h"

#include "apoc3d/Core/ResourceHandle.h"

#include "apoc3d/Config/XmlConfigurationFormat.h"

#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Input/Mouse.h"

#include "apoc3d/IOLib/ModelData.h"
#include "apoc3d/IOLib/Streams.h"

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

using namespace Apoc3D::Input;
using namespace Apoc3D::Utility;
using namespace Apoc3D::IO;
using namespace APDesigner::CommonDialog;

namespace APDesigner
{
	UIDocument::UIDocument(MainWindow* window, EditorExtension* ext, const String& file)
		: Document(window, ext), m_editingForm(nullptr)
	{

		m_uiViewer = new PictureBox(Point(15, 27), 1);
		m_uiViewer->Size = Point(512,512);
		m_uiViewer->SetSkin(window->getUISkin());
		m_uiViewer->eventPictureDraw().Bind(this, &UIDocument::UIViewer_Draw);


		getDocumentForm()->setMinimumSize(Point(1000,600));

		getDocumentForm()->setTitle(file);
	}

	UIDocument::~UIDocument()
	{

	}


	void UIDocument::LoadRes()
	{
		// create virtual form here
		m_editingForm = new Form();
		m_editingForm->SetSkin(getMainWindow()->getUISkin());
		m_editingForm->Initialize(getMainWindow()->getDevice());
		m_editingForm->Visible = true;
	}
	void UIDocument::SaveRes()
	{

	}

	void UIDocument::Initialize(RenderDevice* device)
	{
		getDocumentForm()->getControls().Add(m_uiViewer);

		Document::Initialize(device);

	}
	void UIDocument::Update(const GameTime* const time)
	{
		Document::Update(time);

		m_uiViewer->Size = getDocumentForm()->Size - Point(30, 50);
	}
	void UIDocument::Render()
	{

	}

	void UIDocument::UIViewer_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		sprite->Draw(getMainWindow()->getUISkin()->WhitePixelTexture, *dstRect, nullptr, CV_LightGray);

		Matrix orginalTrans;
		if (!sprite->isUsingStack())
		{
			orginalTrans = sprite->getTransform();
		}


		Matrix offTrans;
		Matrix::CreateTranslation(offTrans, (float)dstRect->X + 7, (float)dstRect->Y + 7, 0);

		sprite->MultiplyTransform(offTrans);
		if (m_editingForm)
		{
			m_editingForm->Draw(sprite);
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