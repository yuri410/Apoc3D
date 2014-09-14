#pragma once

#ifndef APOC3D_FONTDOCUMENT_H
#define APOC3D_FONTDOCUMENT_H

#include "APDesigner/Document.h"

namespace APDesigner
{
	class FontDocument : public Document
	{
	public:
		FontDocument(MainWindow* window, EditorExtension* ext, const String& file);
		~FontDocument();

		virtual void LoadRes();
		virtual void SaveRes();
		virtual bool IsReadOnly() { return false; };

		virtual void Initialize(RenderDevice* device);
		virtual void Update(const GameTime* time);
		virtual void Render();

	private:
		void GlyphDistribution_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect);

		String m_filePath;

		PictureBox* m_glyphDistribution;

		List<std::pair<uint32, uint32>> m_regions;
	};
}

#endif