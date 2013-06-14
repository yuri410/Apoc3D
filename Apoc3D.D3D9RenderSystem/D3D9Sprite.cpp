/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/
#include "D3D9Sprite.h"
#include "D3D9RenderDevice.h"
#include "D3D9Texture.h"
#include "D3D9VertexDeclaration.h"
#include "Buffer/D3D9VertexBuffer.h"
#include "D3D9RenderStateManager.h"

#include "apoc3d/Graphics/RenderSystem/Shader.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			int MaxVertices = 1024;
			int FlushThreshold = MaxVertices/5;
			
			D3D9Sprite::D3D9Sprite(D3D9RenderDevice* device)
				: Sprite(device), m_device(device), m_rawDevice(device->getDevice())
			{
				FastList<VertexElement> elements;
				elements.Add(VertexElement(0, VEF_Vector4, VEU_PositionTransformed));
				elements.Add(VertexElement(16, VEF_Color, VEU_Color));
				elements.Add(VertexElement(20, VEF_Vector2, VEU_TextureCoordinate,0));

				m_vtxDecl = new D3D9VertexDeclaration(device, elements);

				//int test = m_vtxDecl->GetVertexSize();
				m_quadBuffer = new D3D9VertexBuffer(device, MaxVertices * m_vtxDecl->GetVertexSize(), (BufferUsageFlags)(BU_Dynamic|BU_WriteOnly));

				m_storedState.oldBlendFactor = 0;
			}
			D3D9Sprite::~D3D9Sprite()
			{
				delete m_vtxDecl;
				delete m_quadBuffer;
			}
			void D3D9Sprite::Begin(SpriteSettings settings)
			{
				Sprite::Begin(settings);

				D3DVIEWPORT9 vp;
				m_rawDevice->GetViewport(&vp);

				SetRenderState();
			}
			void D3D9Sprite::End()
			{
				Sprite::End();

				Flush();
				RestoreRenderState();
			}
			void D3D9Sprite::SetRenderState()
			{
				NativeD3DStateManager* mgr = m_device->getNativeStateManager();

				if ((getSettings() & SPR_RestoreState) == SPR_RestoreState)
				{
					m_storedState.oldAlphaBlendEnable = mgr->getAlphaBlendEnable();
					m_storedState.oldBlendFunc = mgr->getAlphaBlendOperation();
					m_storedState.oldSrcBlend = mgr->getAlphaSourceBlend();
					m_storedState.oldDstBlend = mgr->getAlphaDestinationBlend();
					m_storedState.oldBlendFactor = mgr->getAlphaBlendFactor();
					m_storedState.oldDepthEnabled = mgr->getDepthBufferEnabled();
					m_storedState.oldCull = mgr->getCullMode();
				}

				if (getSettings() & SPR_ChangeState)
				{
					mgr->SetDepth(false, mgr->getDepthBufferWriteEnabled());
					mgr->SetCullMode(CULL_None);

					if ((getSettings() & SPR_AlphaBlended) == SPR_AlphaBlended)
						mgr->SetAlphaBlend(true, BLFUN_Add, BLEND_SourceAlpha, BLEND_InverseSourceAlpha, m_storedState.oldBlendFactor);

					ShaderSamplerState state = mgr->getPixelSampler(0);
					state.MinFilter = TFLT_Linear;
					state.MagFilter = TFLT_Linear;
					state.MipFilter = TFLT_Linear;
					state.AddressU = TA_Clamp;
					state.AddressV = TA_Clamp;
					state.MaxMipLevel = 0;
					state.MipMapLODBias = 0;

					mgr->SetPixelSampler(0, state);

					m_rawDevice->SetVertexDeclaration(m_vtxDecl->getD3DDecl());
					m_rawDevice->SetStreamSource(0, m_quadBuffer->getD3DBuffer(), 0, sizeof(QuadVertex));
					m_rawDevice->SetVertexShader(NULL); m_rawDevice->SetPixelShader(NULL);
					m_rawDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
					m_rawDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
					m_rawDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
					//m_rawDevice->SetTexture(0,0);
				}

				
				
				//mgr->SetAlphaBlend(false, BLFUN_Add, BLEND_SourceAlpha, BLEND_InverseSourceAlpha, oldBlendFactor);
				
			}
			void D3D9Sprite::RestoreRenderState()
			{
				if ((getSettings() & SPR_RestoreState) == SPR_RestoreState)
				{
					NativeD3DStateManager* mgr = m_device->getNativeStateManager();

					if ((getSettings() & SPR_AlphaBlended) == SPR_AlphaBlended)
						mgr->SetAlphaBlend(m_storedState.oldAlphaBlendEnable, m_storedState.oldBlendFunc, m_storedState.oldSrcBlend, m_storedState.oldDstBlend, m_storedState.oldBlendFactor);

					mgr->SetDepth(m_storedState.oldDepthEnabled, mgr->getDepthBufferWriteEnabled());
					mgr->SetCullMode(m_storedState.oldCull);
				}
			}

			void D3D9Sprite::Draw(Texture* texture, const Apoc3D::Math::RectangleF& dstRect, float uScale, float vScale, float uBias, float vBias, uint color)
			{
				assert(m_began);

				DrawEntry drawE;
				_DrawAsEntry(drawE, texture, dstRect, NULL, color);
				AddTransformedUVDraw(drawE, uScale, vScale, uBias, vBias);
			}

			struct RECTF
			{
				float left;
				float top;
				float right;
				float bottom;
			};

			// Auto resizing to fit the target rectangle is implemented in this method.
			// 
			void D3D9Sprite::_DrawAsEntry(DrawEntry& result, Texture* texture, const Apoc3D::Math::RectangleF& dstRect, const Apoc3D::Math::RectangleF* srcRect, uint color)
			{
				float position[3] = { dstRect.X, dstRect.Y, 0 };

				Matrix tempM;

				if (srcRect)
				{
					// RECT:
					// LONG    left;
					// LONG    top;
					// LONG    right;
					// LONG    bottom;
					//Apoc3D::Math::RectangleF r2 = *srcRect;
					//RECTF r = { 
					//	(LONG)srcRect->X, 
					//	(LONG)srcRect->Y,
					//	(LONG)srcRect->getRight(),
					//	(LONG)srcRect->getBottom()
					//};
					RECTF r = { 
						srcRect->X, 
						srcRect->Y,
						srcRect->getRight(),
						srcRect->getBottom()
					};

					// In some cases, the X,Y of the rect is not always the top-left corner,
					// when the Width or Height is negative. Standardize it.
					//if (r2.getLeft()>r2.getRight())
					//{
					//	r2.X += r2.Width;
					//	r2.Width = -r2.Width;

					//	position[0] -= r2.Width;
					//}
					//if (r2.getTop()>r2.getBottom())
					//{
					//	r2.Y += r2.Height;
					//	r2.Height = -r2.Height;

					//	position[1] -= r2.Height;
					//}
					if (r.left > r.right)
					{
						float temp = r.right;
						r.right = r.left;
						r.left = temp;
						position[0] -= ( r.left - r.right);
					}
					if (r.top>r.bottom)
					{
						float temp = r.bottom;
						r.bottom = r.top;
						r.top = temp;
						position[1] -= ( r.top - r.bottom);
					}
					Apoc3D::Math::RectangleF r2(r.left, r.top, r.right-r.left,r.bottom-r.top);

					// calculate a scaling and translation matrix
					Matrix trans;
					Matrix::CreateTranslation(trans, position[0], position[1], position[2]);
					trans.M11 = (float)dstRect.Width / (float)srcRect->Width;
					trans.M22 = (float)dstRect.Height / (float)srcRect->Height;

					// add "trans" at the the beginning for the result
					const Matrix& baseTrans = getTransform();
					Matrix::Multiply(tempM, trans, baseTrans);

					// As the position have been added to the transform, 
					// draw the texture at the origin
					//AddTransformedDraw(texture, tempM, &r, color);
					_FillTransformedDraw(result, texture, tempM, &r2, color);
				}
				else
				{
					Matrix trans;
					Matrix::CreateTranslation(trans, position[0], position[1], position[2]);
					trans.M11 = (float)dstRect.Width / (float)texture->getWidth();
					trans.M22 = (float)dstRect.Height / (float)texture->getHeight();

					const Matrix& baseTrans = getTransform();
					Matrix::Multiply(tempM, trans, baseTrans);

					// As the position have been added to the transform, 
					// draw the texture at the origin
					//AddTransformedDraw(texture, tempM, NULL, color);
					_FillTransformedDraw(result, texture, tempM, NULL, color);
				}
			}

			void D3D9Sprite::Draw(Texture* texture, 
				const Apoc3D::Math::RectangleF& dstRect, const Apoc3D::Math::RectangleF* srcRect, uint color)
			{
				assert(m_began);

				DrawEntry drawE;
				_DrawAsEntry(drawE, texture, dstRect, srcRect, color);
				m_deferredDraws.Add(drawE);

				if (m_deferredDraws.getCount()>FlushThreshold)
				{
					Flush();
				}
			}


			void D3D9Sprite::Draw(Texture* texture, Vector2 pos, uint color)
			{
				assert(m_began);

				AddNormalDraw(texture, Vector2Utils::GetX(pos), Vector2Utils::GetY(pos), color);
			}
			void D3D9Sprite::Draw(Texture* texture, const PointF& pos, uint color)
			{
				assert(m_began);

				AddNormalDraw(texture, pos.X, pos.Y, color);
			}
			void D3D9Sprite::Draw(Texture* texture, const PointF& pos, float uScale, float vScale, float uBias, float vBias, uint color)
			{
				assert(m_began);

				AddUVDraw(texture, pos.X, pos.Y, color, uScale, vScale, uBias, vBias);
			}


			void D3D9Sprite::Flush()
			{
				char* vtxData = (char*)m_quadBuffer->Lock(0, m_deferredDraws.getCount()*4*sizeof(QuadVertex), LOCK_Discard);

				for (int i=0;i<m_deferredDraws.getCount();i++)
				{
					memcpy(vtxData, &m_deferredDraws[i].TL, sizeof(QuadVertex)*4);

					vtxData+= sizeof(QuadVertex)*4;
				}

				m_quadBuffer->Unlock();

				NativeD3DStateManager* mgr = m_device->getNativeStateManager();

				//m_rawDevice->SetTexture(0,0);
				mgr->SetTexture(0,0);
				D3D9Texture* currentTexture = NULL;
				for (int i=0;i<m_deferredDraws.getCount();i++)
				{
					ShaderSamplerState state;
					if (m_deferredDraws[i].IsUVExtended)
					{
						state = mgr->getPixelSampler(0);
						state.AddressU = TA_Wrap;
						state.AddressV = TA_Wrap;
						mgr->SetPixelSampler(0, state);
					}

					if (m_deferredDraws[i].Tex != currentTexture)
					{
						mgr->SetTexture(0,m_deferredDraws[i].Tex);
						//m_rawDevice->SetTexture(0, m_deferredDraws[i].Tex->getInternal2D());
					}
					
					m_rawDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, i*4, 2);

					if (m_deferredDraws[i].IsUVExtended)
					{
						state.AddressU = TA_Clamp;
						state.AddressV = TA_Clamp;
						mgr->SetPixelSampler(0, state);
					}
				}



				m_deferredDraws.Clear();
			}

			void D3D9Sprite::AddUVDraw(Texture* texture, float x, float y, uint color, float uScale, float vScale, float uBias, float vBias)
			{
				DrawEntry drawE;
				_FillNormalDraw(drawE, texture, x, y, color);
				
				drawE.BL.TexCoord[0] *= uScale;
				drawE.BR.TexCoord[0] *= uScale;
				drawE.TL.TexCoord[0] *= uScale;
				drawE.TR.TexCoord[0] *= uScale;

				drawE.BL.TexCoord[1] *= vScale;
				drawE.BR.TexCoord[1] *= vScale;
				drawE.TL.TexCoord[1] *= vScale;
				drawE.TR.TexCoord[1] *= vScale;

				drawE.BL.TexCoord[0] += uBias;
				drawE.BR.TexCoord[0] += uBias;
				drawE.TL.TexCoord[0] += uBias;
				drawE.TR.TexCoord[0] += uBias;

				drawE.BL.TexCoord[1] += vBias;
				drawE.BR.TexCoord[1] += vBias;
				drawE.TL.TexCoord[1] += vBias;
				drawE.TR.TexCoord[1] += vBias;

				drawE.IsUVExtended = true;

				m_deferredDraws.Add(drawE);

				if (m_deferredDraws.getCount()>FlushThreshold)
				{
					Flush();
				}
			}
			void D3D9Sprite::AddTransformedUVDraw(DrawEntry& drawE, float uScale, float vScale, float uBias, float vBias)
			{

				drawE.BL.TexCoord[0] *= uScale;
				drawE.BR.TexCoord[0] *= uScale;
				drawE.TL.TexCoord[0] *= uScale;
				drawE.TR.TexCoord[0] *= uScale;

				drawE.BL.TexCoord[1] *= vScale;
				drawE.BR.TexCoord[1] *= vScale;
				drawE.TL.TexCoord[1] *= vScale;
				drawE.TR.TexCoord[1] *= vScale;

				drawE.BL.TexCoord[0] += uBias;
				drawE.BR.TexCoord[0] += uBias;
				drawE.TL.TexCoord[0] += uBias;
				drawE.TR.TexCoord[0] += uBias;

				drawE.BL.TexCoord[1] += vBias;
				drawE.BR.TexCoord[1] += vBias;
				drawE.TL.TexCoord[1] += vBias;
				drawE.TR.TexCoord[1] += vBias;

				drawE.IsUVExtended = true;

				m_deferredDraws.Add(drawE);

				if (m_deferredDraws.getCount()>FlushThreshold)
				{
					Flush();
				}
			}
			void D3D9Sprite::AddTransformedDraw(Texture* texture, const Matrix& t, const Apoc3D::Math::RectangleF* srcRect, uint color)
			{
				DrawEntry drawE;
				_FillTransformedDraw(drawE, texture, t, srcRect, color);
				m_deferredDraws.Add(drawE);

				if (m_deferredDraws.getCount()>FlushThreshold)
				{
					Flush();
				}
			}
			void D3D9Sprite::AddNormalDraw(Texture* texture, float x, float y, uint color)
			{
				DrawEntry drawE;
				_FillNormalDraw(drawE, texture, x, y, color);
				m_deferredDraws.Add(drawE);

				if (m_deferredDraws.getCount()>FlushThreshold)
				{
					Flush();
				}
			}

			void D3D9Sprite::_FillNormalDraw(DrawEntry& drawE, Texture* texture, float x, float y, uint color)
			{
				float width = (float)texture->getWidth();
				float height = (float)texture->getHeight();

				const Matrix& trans = getTransform();

				Vector3 tl = Vector3Utils::LDVector(x,y,0);
				Vector3 tr = Vector3Utils::LDVector(width+x, y,0);
				Vector3 bl = Vector3Utils::LDVector(x, height+y,0);
				Vector3 br = Vector3Utils::LDVector(width+x, height+y, 0);

				tl = Vector3Utils::TransformCoordinate(tl, trans);
				tr = Vector3Utils::TransformCoordinate(tr, trans);
				bl = Vector3Utils::TransformCoordinate(bl, trans);
				br = Vector3Utils::TransformCoordinate(br, trans);


				drawE.Tex = static_cast<D3D9Texture*>(texture);
				drawE.TL.Position[0] = _V3X(tl)-0.5f;
				drawE.TL.Position[1] = _V3Y(tl)-0.5f;
				drawE.TL.Position[2] = _V3Z(tl);
				drawE.TL.Position[3] = 1;

				drawE.TR.Position[0] = _V3X(tr)-0.5f;
				drawE.TR.Position[1] = _V3Y(tr)-0.5f;
				drawE.TR.Position[2] = _V3Z(tr);
				drawE.TR.Position[3] = 1;

				drawE.BL.Position[0] = _V3X(bl)-0.5f;
				drawE.BL.Position[1] = _V3Y(bl)-0.5f;
				drawE.BL.Position[2] = _V3Z(bl);
				drawE.BL.Position[3] = 1;

				drawE.BR.Position[0] = _V3X(br)-0.5f;
				drawE.BR.Position[1] = _V3Y(br)-0.5f;
				drawE.BR.Position[2] = _V3Z(br);
				drawE.BR.Position[3] = 1;

				drawE.TL.Diffuse = drawE.TR.Diffuse = drawE.BL.Diffuse = drawE.BR.Diffuse = color;

				drawE.TL.TexCoord[0] = 0; drawE.TL.TexCoord[1] = 0;
				drawE.TR.TexCoord[0] = 1; drawE.TR.TexCoord[1] = 0;
				drawE.BL.TexCoord[0] = 0; drawE.BL.TexCoord[1] = 1;
				drawE.BR.TexCoord[0] = 1; drawE.BR.TexCoord[1] = 1;
				drawE.IsUVExtended = false;
			}
			void D3D9Sprite::_FillTransformedDraw(DrawEntry& drawE, Texture* texture, const Matrix& t, const Apoc3D::Math::RectangleF* srcRect, uint color)
			{
				drawE.Tex = static_cast<D3D9Texture*>(texture);

				drawE.TL.Diffuse = drawE.TR.Diffuse = drawE.BL.Diffuse = drawE.BR.Diffuse = color;

				const Matrix& trans = t;//getTransform();

				if (srcRect)
				{
					float width = srcRect->Width;//(float)(srcRect->right-srcRect->left);
					float height = srcRect->Height;//(float)(srcRect->bottom-srcRect->top);

					Vector3 tl = Vector3Utils::LDVector(0, 0,0);
					Vector3 tr = Vector3Utils::LDVector(width, 0,0);
					Vector3 bl = Vector3Utils::LDVector(0, height,0);
					Vector3 br = Vector3Utils::LDVector(width, height, 0);

					tl = Vector3Utils::TransformCoordinate(tl, trans);
					tr = Vector3Utils::TransformCoordinate(tr, trans);
					bl = Vector3Utils::TransformCoordinate(bl, trans);
					br = Vector3Utils::TransformCoordinate(br, trans);

					drawE.TL.Position[0] = _V3X(tl)-0.5f;
					drawE.TL.Position[1] = _V3Y(tl)-0.5f;
					drawE.TL.Position[2] = _V3Z(tl);
					drawE.TL.Position[3] = 1;

					drawE.TR.Position[0] = _V3X(tr)-0.5f;
					drawE.TR.Position[1] = _V3Y(tr)-0.5f;
					drawE.TR.Position[2] = _V3Z(tr);
					drawE.TR.Position[3] = 1;

					drawE.BL.Position[0] = _V3X(bl)-0.5f;
					drawE.BL.Position[1] = _V3Y(bl)-0.5f;
					drawE.BL.Position[2] = _V3Z(bl);
					drawE.BL.Position[3] = 1;

					drawE.BR.Position[0] = _V3X(br)-0.5f;
					drawE.BR.Position[1] = _V3Y(br)-0.5f;
					drawE.BR.Position[2] = _V3Z(br);
					drawE.BR.Position[3] = 1;


					width = (float)texture->getWidth();
					height = (float)texture->getHeight();

					drawE.TL.TexCoord[0] = srcRect->getLeft()/width ;		drawE.TL.TexCoord[1] = srcRect->getTop()/height;
					drawE.TR.TexCoord[0] = srcRect->getRight()/width;		drawE.TR.TexCoord[1] = srcRect->getTop()/height;
					drawE.BL.TexCoord[0] = srcRect->getLeft()/width ;		drawE.BL.TexCoord[1] = srcRect->getBottom()/height;
					drawE.BR.TexCoord[0] = srcRect->getRight()/width;		drawE.BR.TexCoord[1] = srcRect->getBottom()/height;
					drawE.IsUVExtended = false;
				}
				else
				{
					float width = (float)texture->getWidth();
					float height = (float)texture->getHeight();

					Vector3 tl = Vector3Utils::LDVector(0,0,0);
					Vector3 tr = Vector3Utils::LDVector(width, 0,0);
					Vector3 bl = Vector3Utils::LDVector(0, height,0);
					Vector3 br = Vector3Utils::LDVector(width, height, 0);

					tl = Vector3Utils::TransformCoordinate(tl, trans);
					tr = Vector3Utils::TransformCoordinate(tr, trans);
					bl = Vector3Utils::TransformCoordinate(bl, trans);
					br = Vector3Utils::TransformCoordinate(br, trans);


					drawE.TL.Position[0] = _V3X(tl)-0.5f;
					drawE.TL.Position[1] = _V3Y(tl)-0.5f;
					drawE.TL.Position[2] = _V3Z(tl);
					drawE.TL.Position[3] = 1;

					drawE.TR.Position[0] = _V3X(tr)-0.5f;
					drawE.TR.Position[1] = _V3Y(tr)-0.5f;
					drawE.TR.Position[2] = _V3Z(tr);
					drawE.TR.Position[3] = 1;

					drawE.BL.Position[0] = _V3X(bl)-0.5f;
					drawE.BL.Position[1] = _V3Y(bl)-0.5f;
					drawE.BL.Position[2] = _V3Z(bl);
					drawE.BL.Position[3] = 1;

					drawE.BR.Position[0] = _V3X(br)-0.5f;
					drawE.BR.Position[1] = _V3Y(br)-0.5f;
					drawE.BR.Position[2] = _V3Z(br);
					drawE.BR.Position[3] = 1;


					drawE.TL.TexCoord[0] = 0; drawE.TL.TexCoord[1] = 0;
					drawE.TR.TexCoord[0] = 1; drawE.TR.TexCoord[1] = 0;
					drawE.BL.TexCoord[0] = 0; drawE.BL.TexCoord[1] = 1;
					drawE.BR.TexCoord[0] = 1; drawE.BR.TexCoord[1] = 1;
					drawE.IsUVExtended = false;
				}
			}

			void D3D9Sprite::SetTransform(const Matrix& matrix)
			{
				Sprite::SetTransform(matrix);
			}

		}
	}
}