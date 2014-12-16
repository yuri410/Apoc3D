#pragma once

#include "../SubDemo.h"

namespace dip
{
	struct HistogramData
	{
		int PixelCount[256];
		int TotalPixelCount = 0;
		int HighestPixelCount = 0;

		HistogramData();

		void Build(Texture* tex);
		void Draw(Sprite* spr, const Apoc3D::Math::Rectangle& rect);
	};

	class DemoHistogram : public SubDemo
	{
	public:
		DemoHistogram(DIP1* parent, RenderDevice* device, const StyleSkin* skin);
		~DemoHistogram();
		
		virtual void Show() override;
	private:

		void Work11(MenuItem* ctrl);
		void Process(Button* ctrl);
		void Work11_HG1(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect);
		void Work11_HG2(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect);
		void Work11_HG3(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect);

		HistogramData m_originalHgram;

		Form* m_form;
		Texture* m_result1;
		Texture* m_result2;
		HistogramData m_result1HG;
		HistogramData m_result2HG;

	};
}