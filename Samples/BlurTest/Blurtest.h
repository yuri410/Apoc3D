
#ifndef GUIDEMO_H
#define GUIDEMO_H

#include "DemoCommon.h"

namespace Sample_GUI
{
    class BlurTest : public Apoc3DEx::Game
	{
	public:
		static const int DMCount = 8;

		BlurTest(RenderWindow* wnd);
		~BlurTest();

		virtual void Initialize();
		virtual void Finalize();

		virtual void Load();
		virtual void Unload();
		virtual void Update(const GameTime* time);
		virtual void Draw(const GameTime* time);

		virtual void OnFrameStart();
		virtual void OnFrameEnd();

	private:
		void DrawQuad(AutomaticEffect* fx, Texture* tex);
		void DrawQuad(AutomaticEffect* fx, Texture* tex, const GaussBlurFilter& flt, bool blurY);

		Sprite* m_sprite;

		Texture* m_original;

		Texture* m_uploadTest;

		RenderTarget* m_blur1;
		RenderTarget* m_blur2;
		RenderTarget* m_blur1SM;
		RenderTarget* m_blur2SM;

		int32 m_mode = 0;

		GaussBlurFilter m_blur13;
		GaussBlurFilter m_blur25;

		VertexBuffer* m_quadBuffer;
		VertexDeclaration* m_quadVtxDecl;

		Material* m_quadTempMtrl;
		GeometryData m_quadTempGeoData;
    };
}
#endif
