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
#include "Buffer/D3D9IndexBuffer.h"
#include "D3D9RenderStateManager.h"

#include "apoc3d/Graphics/RenderSystem/Shader.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			//static const int MaxVertices = 1024;
			//static const int FlushThreshold = MaxVertices/5;
			const int MaxDeferredDraws = 144;
			const int FlushThreshold = 128;
			
			D3D9Sprite::D3D9Sprite(D3D9RenderDevice* device)
				: Sprite(device), m_device(device), m_rawDevice(device->getDevice())
			{
				{
					const List<VertexElement> elements = 
					{
						VertexElement(0, VEF_Vector4, VEU_PositionTransformed),
						VertexElement(16, VEF_Color, VEU_Color),
						VertexElement(20, VEF_Vector2, VEU_TextureCoordinate, 0)
					};

					m_vtxDecl = new D3D9VertexDeclaration(device, elements);
				}
				{
					const List<VertexElement> elements =
					{
						VertexElement(0, VEF_Vector4, VEU_Position),
						VertexElement(16, VEF_Color, VEU_Color),
						VertexElement(20, VEF_Vector2, VEU_TextureCoordinate, 0)
					};

					m_vtxDeclShadable = new D3D9VertexDeclaration(device, elements);
				}

				m_quadBuffer = new D3D9VertexBuffer(device, (MaxDeferredDraws * 4) * m_vtxDecl->GetVertexSize(), (BufferUsageFlags)(BU_Dynamic|BU_WriteOnly));
				m_quadIndices = new D3D9IndexBuffer(device, IBT_Bit16, sizeof(uint16) * MaxDeferredDraws * 6, BU_WriteOnly);

				{
					uint16* indices = (uint16*)m_quadIndices->Lock(LOCK_None);

					for (uint16 i = 0; i < (uint16)MaxDeferredDraws; i++)
					{
						uint16 base = i * 4;

						*indices++ = base + 0;
						*indices++ = base + 1;
						*indices++ = base + 2;

						*indices++ = base + 2;
						*indices++ = base + 1;
						*indices++ = base + 3;
					}

					m_quadIndices->Unlock();
				}
				
				m_storedState.oldBlendFactor = 0;
			}
			D3D9Sprite::~D3D9Sprite()
			{
				delete m_vtxDecl;
				delete m_vtxDeclShadable;
				delete m_quadBuffer;
				delete m_quadIndices;
			}
			void D3D9Sprite::Begin(SpriteSettings settings)
			{
				Sprite::Begin(settings);

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

					m_rawDevice->SetStreamSource(0, m_quadBuffer->getD3DBuffer(), 0, sizeof(QuadVertex));
					m_rawDevice->SetIndices(m_quadIndices->getD3DBuffer());

					if (getSettings() & SPR_AllowShading)
					{
						m_rawDevice->SetVertexDeclaration(m_vtxDeclShadable->getD3DDecl());
					}
					else
					{
						ShaderSamplerState state = mgr->getPixelSampler(0);
						state.MinFilter = TFLT_Linear;
						state.MagFilter = TFLT_Linear;
						state.MipFilter = TFLT_None;
						state.AddressU = TA_Clamp;
						state.AddressV = TA_Clamp;
						state.MaxMipLevel = 0;
						state.MipMapLODBias = 0;

						mgr->SetPixelSampler(0, state);


						m_rawDevice->SetVertexDeclaration(m_vtxDecl->getD3DDecl());

						m_device->BindVertexShader(nullptr);
						m_device->BindPixelShader(nullptr);

						m_rawDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
						m_rawDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
						m_rawDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
					}
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

			void D3D9Sprite::Draw(Texture* texture, const PointF& pos, uint color)
			{
				assert(m_began);

				DrawEntry drawE;
				drawE.FillNormalDraw(texture, getTransform(), pos.X, pos.Y, color);
				EnqueueDrawEntry(drawE);
			}

			void D3D9Sprite::Draw(Texture* texture, const Apoc3D::Math::RectangleF& dstRect, const Apoc3D::Math::RectangleF* srcRect, uint color)
			{
				assert(m_began);

				DrawEntry drawE;
				drawE.FillNormalDraw(texture, getTransform(), dstRect, srcRect, color);
				EnqueueDrawEntry(drawE);
			}

			void D3D9Sprite::Draw(Texture* texture, const Apoc3D::Math::RectangleF& dstRect, const Apoc3D::Math::RectangleF* srcRect,
				uint tlColor, uint trColor, uint blColor, uint brColor) 
			{
				assert(m_began);

				DrawEntry drawE;
				drawE.FillNormalDraw(texture, getTransform(), dstRect, srcRect, CV_White);

				drawE.SetColors(tlColor, trColor, blColor, brColor);
				EnqueueDrawEntry(drawE);
			}



			void D3D9Sprite::Draw(Texture* texture, const PointF(&corners)[4], const Apoc3D::Math::RectangleF* srcRect, const uint(&colors)[4])
			{
				assert(m_began);

				DrawEntry drawE;
				drawE.SetPositions(getTransform(), corners[0], corners[1], corners[2], corners[3]);
				drawE.SetSrcRect(texture, srcRect);
				drawE.SetColors(colors[0], colors[1], colors[2], colors[3]);
				EnqueueDrawEntry(drawE);
			}

			void D3D9Sprite::DrawTiled(Texture* texture, const PointF& pos, float uScale, float vScale, float uBias, float vBias, uint color)
			{
				assert(m_began);

				DrawEntry drawE;
				drawE.FillNormalDraw(texture, getTransform(), pos.X, pos.Y, color);

				drawE.ChangeUV(uScale, vScale, uBias, vBias);

				EnqueueDrawEntry(drawE);
			}

			void D3D9Sprite::DrawTiled(Texture* texture, const Apoc3D::Math::RectangleF& dstRect, float uScale, float vScale, float uBias, float vBias, uint color)
			{
				assert(m_began);

				DrawEntry drawE;
				drawE.FillNormalDraw(texture, getTransform(), dstRect, NULL, color);

				drawE.ChangeUV(uScale, vScale, uBias, vBias);

				EnqueueDrawEntry(drawE);
			}


			void D3D9Sprite::Flush()
			{
				if (m_deferredDraws.getCount() == 0)
					return;

				char* vtxData = (char*)m_quadBuffer->Lock(0, m_deferredDraws.getCount() * 4 * sizeof(QuadVertex), LOCK_Discard);

				for (int i = 0; i < m_deferredDraws.getCount(); i++)
				{
					memcpy(vtxData, &m_deferredDraws[i].TL, sizeof(QuadVertex) * 4);

					vtxData += sizeof(QuadVertex) * 4;
				}

				m_quadBuffer->Unlock();

				NativeD3DStateManager* mgr = m_device->getNativeStateManager();

				//m_rawDevice->SetTexture(0,0);
				mgr->SetTexture(0,0);
				D3D9Texture* currentTexture = m_deferredDraws[0].Tex;
				bool currentUVExtend = m_deferredDraws[0].IsUVExtended;
				int32 lastIndex = 0;

				for (int i = 0; i < m_deferredDraws.getCount() + 1; i++)
				{
					bool stateChanged = false;
					const DrawEntry* de;

					if (i == m_deferredDraws.getCount())
					{
						// last entry, make a dummy next DrawEntry to force state change
						// then the previous entries will be drawn
						static DrawEntry dummy;
						dummy.Tex = nullptr;
						dummy.IsUVExtended = !currentUVExtend;
						de = &dummy;
					}
					else
					{
						de = &m_deferredDraws[i];
					}

					if (de->Tex != currentTexture)
					{
						mgr->SetTexture(0, currentTexture);
						currentTexture = de->Tex;
						stateChanged = true;
					}

					if (de->IsUVExtended != currentUVExtend)
					{
						if (currentUVExtend)
						{
							ShaderSamplerState state = mgr->getPixelSampler(0);
							state.AddressU = TA_Wrap;
							state.AddressV = TA_Wrap;
							mgr->SetPixelSampler(0, state);
						}
						else
						{
							ShaderSamplerState state = mgr->getPixelSampler(0);
							state.AddressU = TA_Clamp;
							state.AddressV = TA_Clamp;
							mgr->SetPixelSampler(0, state);
						}
						currentUVExtend = de->IsUVExtended;
						stateChanged = true;
					}

					if (stateChanged)
					{
						int32 startIndex = lastIndex * 6;
						int32 startVertex = lastIndex * 4;
						int32 dpCount = i - lastIndex; // not including i

						int32 vtxCount = dpCount * 4;

						m_rawDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, startVertex, vtxCount, startIndex, dpCount * 2);
						m_batchCount++;

						lastIndex = i;
					}


					//m_rawDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, i*4, 2);
				}
				

				m_deferredDraws.Clear();
			}


			void D3D9Sprite::EnqueueDrawEntry(const DrawEntry& drawE)
			{
				m_deferredDraws.Add(drawE);

				if (m_deferredDraws.getCount() > FlushThreshold)
				{
					Flush();
				}
			}

			static Apoc3D::Math::RectangleF ObtainSrcRect(Texture* texture, const Apoc3D::Math::RectangleF* _srcRect)
			{
				Apoc3D::Math::RectangleF srcRect;
				if (_srcRect)
					srcRect = *_srcRect;
				else
					srcRect = { 0, 0, static_cast<float>(texture->getWidth()), static_cast<float>(texture->getHeight()) };

				return srcRect;
			}

			void D3D9Sprite::DrawRoundedRect(Texture* texture, const Apoc3D::Math::RectangleF& dstRect, const Apoc3D::Math::RectangleF* _srcRect,
				float cornerRadius, int32 div, uint color)
			{
				assert(m_began);

				float minDstEdge = Math::Min(dstRect.Width, dstRect.Height);
				if (cornerRadius > minDstEdge) cornerRadius = minDstEdge;

				Apoc3D::Math::RectangleF srcRect = ObtainSrcRect(texture, _srcRect);

				float srcRectXRatio = srcRect.Width / dstRect.Width;
				float srcRectYRatio = srcRect.Height / dstRect.Height;

				{
					// draw rect body

					// draw center vertical
					Apoc3D::Math::RectangleF centerDstRect = dstRect;
					centerDstRect.Inflate(-cornerRadius, 0);

					Apoc3D::Math::RectangleF centerSrcRect = srcRect;
					centerSrcRect.Inflate(-cornerRadius * srcRectXRatio, 0);

					Draw(texture, centerDstRect, &centerSrcRect, color);

					// draw left & right cap

					Apoc3D::Math::RectangleF leftDstRect = centerDstRect;
					leftDstRect.Inflate(0, -cornerRadius);
					leftDstRect.X = dstRect.X;
					leftDstRect.Width = cornerRadius;

					Apoc3D::Math::RectangleF leftSrcRect = centerSrcRect;
					leftSrcRect.Inflate(0, -cornerRadius * srcRectYRatio);
					leftSrcRect.X = srcRect.X;
					leftSrcRect.Width = cornerRadius * srcRectXRatio;

					Draw(texture, leftDstRect, &leftSrcRect, color);

					Apoc3D::Math::RectangleF rightDstRect = leftDstRect;
					rightDstRect.X += centerDstRect.Width + leftDstRect.Width;

					Apoc3D::Math::RectangleF rightSrcRect = leftSrcRect;
					rightSrcRect.X += centerSrcRect.Width + leftSrcRect.Width;

					Draw(texture, rightDstRect, &rightSrcRect, color);
				}
				
				if (div < 1) div = 1;
				int32 dualPieCount = div;
				float angleStep = Math::PI*0.5f / dualPieCount;

				const int32 cornerCount = 4;
				const PointF cornerBases[cornerCount] =
				{
					{ cornerRadius, cornerRadius },
					{ dstRect.Width - cornerRadius, cornerRadius },
					{ dstRect.Width - cornerRadius, dstRect.Height - cornerRadius },
					{ cornerRadius, dstRect.Height - cornerRadius }
				};
				const float cornerBaseAngles[cornerCount] =
				{
					Math::PI * 0.5f,
					0,
					-Math::PI * 0.5f,
					-Math::PI
				};

				for (int32 j = 0; j < cornerCount; j++)
				{
					float baseAngle = cornerBaseAngles[j];
					
					PointF cornerBaseDstPos = cornerBases[j];
					PointF cornerBaseSrcPos = cornerBases[j];
					cornerBaseSrcPos.X *= srcRectXRatio;
					cornerBaseSrcPos.Y *= srcRectYRatio;
					cornerBaseSrcPos += srcRect.getPosition();

					for (int32 i = 0; i < dualPieCount; i++)
					{
						float startAngle = angleStep * i + baseAngle;
						float endAngle = angleStep * (i + 1) + baseAngle;
						float centerAngle = (startAngle + endAngle)*0.5f;

						PointF leftDir = { cosf(startAngle), -sinf(startAngle) };
						PointF rightDir = { cosf(endAngle), -sinf(endAngle) };
						PointF centerDir = { cosf(centerAngle), -sinf(centerAngle) };

						leftDir *= cornerRadius;
						rightDir *= cornerRadius;
						centerDir *= cornerRadius;

						PointF leftDstPos = cornerBaseDstPos + leftDir;
						PointF rightDstPos = cornerBaseDstPos + rightDir;
						PointF centerDstPos = cornerBaseDstPos + centerDir;
						PointF baseDstPos = cornerBaseDstPos;

						PointF leftSrcPos = leftDstPos;
						PointF rightSrcPos = rightDstPos;
						PointF centerSrcPos = centerDstPos;
						
						PointF* srcPosList[] = { &leftSrcPos, &rightSrcPos, &centerSrcPos };
						for (PointF* sp : srcPosList)
						{
							sp->X *= srcRectXRatio;
							sp->Y *= srcRectYRatio;
							*sp += srcRect.getPosition();
						}

						PointF* dstPosList[] = { &leftDstPos, &rightDstPos, &centerDstPos, &baseDstPos };
						for (PointF* dp : dstPosList)
						{
							*dp += dstRect.getPosition();
						}

						DrawEntry de;
						de.FillNormalDraw(texture, getTransform(), leftDstPos, centerDstPos, baseDstPos, rightDstPos,
							leftSrcPos, centerSrcPos, cornerBaseSrcPos, rightSrcPos, color);
						EnqueueDrawEntry(de);
					}
				}
				
			}

			void D3D9Sprite::DrawCircle(Texture* texture, const Apoc3D::Math::RectangleF& dstRect, const Apoc3D::Math::RectangleF* _srcRect,
				float beginAngle, float endAngle, int32 div, uint color)
			{
				assert(m_began);

				float yRadius = dstRect.Height / 2;
				float xRadius = dstRect.Width / 2;

				Apoc3D::Math::RectangleF srcRect = ObtainSrcRect(texture, _srcRect);

				float srcRectXRatio = srcRect.Width / dstRect.Width;
				float srcRectYRatio = srcRect.Height / dstRect.Height;

				if (div < 2) div = 2;
				int32 dualPieCount = div;
				float angleStep = Math::Max(0.0f, endAngle - beginAngle) / dualPieCount;

				const PointF circleBaseDstPos = dstRect.getSize() * 0.5f;
				const PointF circleBaseSrcPos = srcRect.getCenter();

				for (int32 i = 0; i < dualPieCount; i++)
				{
					float startAngle = angleStep * i + beginAngle;
					float endAngle = angleStep * (i + 1) + beginAngle;
					float centerAngle = (startAngle + endAngle)*0.5f;

					PointF leftDir = { cosf(startAngle) * xRadius, -sinf(startAngle) * yRadius };
					PointF rightDir = { cosf(endAngle) * xRadius, -sinf(endAngle) * yRadius };
					PointF centerDir = { cosf(centerAngle) * xRadius, -sinf(centerAngle) * yRadius };
					
					PointF leftDstPos = circleBaseDstPos + leftDir;
					PointF rightDstPos = circleBaseDstPos + rightDir;
					PointF centerDstPos = circleBaseDstPos + centerDir;
					PointF baseDstPos = circleBaseDstPos;

					PointF leftSrcPos = leftDstPos;
					PointF rightSrcPos = rightDstPos;
					PointF centerSrcPos = centerDstPos;

					PointF* srcPosList[] = { &leftSrcPos, &rightSrcPos, &centerSrcPos };
					for (PointF* sp : srcPosList)
					{
						sp->X *= srcRectXRatio;
						sp->Y *= srcRectYRatio;
						*sp += srcRect.getPosition();
					}

					PointF* dstPosList[] = { &leftDstPos, &rightDstPos, &centerDstPos, &baseDstPos };
					for (PointF* dp : dstPosList)
					{
						*dp += dstRect.getPosition();
					}

					DrawEntry de;
					de.FillNormalDraw(texture, getTransform(), leftDstPos, centerDstPos, baseDstPos, rightDstPos,
						leftSrcPos, centerSrcPos, circleBaseSrcPos, rightSrcPos, color);
					EnqueueDrawEntry(de);
				}
			}

			void D3D9Sprite::DrawCircleArc(Texture* texture, const Apoc3D::Math::RectangleF& dstRect, const Apoc3D::Math::RectangleF* _srcRect, float lineWidth,
				float beginAngle, float endAngle, int32 div, uint color)
			{
				float yRadius = dstRect.Height / 2;
				float xRadius = dstRect.Width / 2;

				Apoc3D::Math::RectangleF srcRect = ObtainSrcRect(texture, _srcRect);

				float srcRectXRatio = srcRect.Width / dstRect.Width;
				float srcRectYRatio = srcRect.Height / dstRect.Height;

				if (div < 2) div = 2;
				int32 segmentCount = div * 2;
				float angleStep = Math::Max(0.0f, endAngle - beginAngle) / segmentCount;

				const PointF circleCenter = dstRect.getSize() * 0.5f;

				float minR = Math::Min(xRadius, yRadius);
				if (lineWidth > minR) lineWidth = minR;

				for (int32 i = 0; i < segmentCount; i++)
				{
					float startAngle = angleStep * i + beginAngle;
					float endAngle = angleStep * (i + 1) + beginAngle;
					
					PointF leftDir = { cosf(startAngle), -sinf(startAngle) };
					PointF rightDir = { cosf(endAngle), -sinf(endAngle) };
					
					PointF leftInnerD = { cosf(startAngle) * (xRadius - lineWidth), -sinf(startAngle) * (yRadius - lineWidth) };
					PointF leftOutterD = { cosf(startAngle) * xRadius, -sinf(startAngle) * yRadius };

					PointF rightInnerD = { cosf(endAngle) * (xRadius - lineWidth), -sinf(endAngle) * (yRadius - lineWidth) };
					PointF rightOutterD = { cosf(endAngle) * xRadius, -sinf(endAngle) * yRadius };

					PointF leftDstInnerPos = circleCenter + leftInnerD;
					PointF rightDstInnerPos = circleCenter + rightInnerD;

					PointF leftDstOutterPos = circleCenter + leftOutterD;
					PointF rightDstOutterPos = circleCenter + rightOutterD;

					PointF leftSrcInnerPos = leftDstInnerPos;
					PointF rightSrcInnerPos = rightDstInnerPos;

					PointF leftSrcOutterPos = leftDstOutterPos;
					PointF rightSrcOutterPos = rightDstOutterPos;

					PointF* srcPosList[] = { &leftSrcInnerPos, &rightSrcInnerPos, &leftSrcOutterPos, &rightSrcOutterPos };
					for (PointF* sp : srcPosList)
					{
						sp->X *= srcRectXRatio;
						sp->Y *= srcRectYRatio;
						*sp += srcRect.getPosition();
					}

					PointF* dstPosList[] = { &leftDstInnerPos, &rightDstInnerPos, &leftDstOutterPos, &rightDstOutterPos };
					for (PointF* dp : dstPosList)
					{
						*dp += dstRect.getPosition();
					}

					DrawEntry de;
					de.FillNormalDraw(texture, getTransform(), leftDstOutterPos, rightDstOutterPos, leftDstInnerPos, rightDstInnerPos,
						leftSrcOutterPos, rightSrcOutterPos, leftSrcInnerPos, rightSrcInnerPos, color);
					EnqueueDrawEntry(de);
				}
			}


			void D3D9Sprite::SetTransform(const Matrix& matrix)
			{
				Sprite::SetTransform(matrix);
			}

			void D3D9Sprite::DrawEntry::FillNormalDraw(Texture* texture, const Matrix& baseTrans, 
				const PointF& tl_dp, const PointF& tr_dp, const PointF& bl_dp, const PointF& br_dp,
				const PointF& tl_sp, const PointF& tr_sp, const PointF& bl_sp, const PointF& br_sp, uint color)
			{
				Tex = static_cast<D3D9Texture*>(texture);

				SetPositions(baseTrans, tl_dp, tr_dp, bl_dp, br_dp);
				SetColors(color);

				float invWidth = 1.0f / texture->getWidth();
				float invHeight = 1.0f / texture->getHeight();

				TL.TexCoord[0] = tl_sp.X * invWidth; TL.TexCoord[1] = tl_sp.Y * invHeight;
				TR.TexCoord[0] = tr_sp.X * invWidth; TR.TexCoord[1] = tr_sp.Y * invHeight;
				BL.TexCoord[0] = bl_sp.X * invWidth; BL.TexCoord[1] = bl_sp.Y * invHeight;
				BR.TexCoord[0] = br_sp.X * invWidth; BR.TexCoord[1] = br_sp.Y * invHeight;
			}

			void D3D9Sprite::DrawEntry::FillNormalDraw(Texture* texture, const Matrix& baseTrans, float x, float y, uint color)
			{
				Tex = static_cast<D3D9Texture*>(texture);

				float width = (float)texture->getWidth();
				float height = (float)texture->getHeight();

				SetPositions(baseTrans, { 0, 0 }, { width, 0 }, { 0, height }, { width, height });

				SetColors(color);
				SetSrcRect(nullptr, nullptr);
			}

			// Auto resizing to fit the target rectangle is implemented in this method.
			void D3D9Sprite::DrawEntry::FillNormalDraw(Texture* texture, const Matrix& baseTrans, 
				const Apoc3D::Math::RectangleF& dstRect, const Apoc3D::Math::RectangleF* srcRect, uint color)
			{
				float position[3] = { dstRect.X, dstRect.Y, 0 };

				Matrix tempM;

				if (srcRect)
				{
					struct RECTF
					{
						float left;
						float top;
						float right;
						float bottom;
					} r = {
						srcRect->X,
						srcRect->Y,
						srcRect->getRight(),
						srcRect->getBottom()
					};

					// In some cases, the X,Y of the rect is not always the top-left corner,
					// when the Width or Height is negative. Standardize it.
					if (r.left > r.right)
					{
						float temp = r.right;
						r.right = r.left;
						r.left = temp;
						position[0] += r.right - r.left;
					}
					if (r.top > r.bottom)
					{
						float temp = r.bottom;
						r.bottom = r.top;
						r.top = temp;
						position[1] += r.bottom - r.top;
					}
					Apoc3D::Math::RectangleF r2(r.left, r.top, r.right - r.left, r.bottom - r.top);

					// calculate a scaling and translation matrix
					Matrix trans;
					Matrix::CreateTranslation(trans, position[0], position[1], position[2]);
					trans.M11 = (float)dstRect.Width / (float)srcRect->Width;
					trans.M22 = (float)dstRect.Height / (float)srcRect->Height;

					// add "trans" at the the beginning for the result
					Matrix::Multiply(tempM, trans, baseTrans);

					// As the position have been added to the transform, 
					// draw the texture at the origin
					FillTransformedDraw(texture, tempM, &r2, color);
				}
				else
				{
					Matrix trans;
					Matrix::CreateTranslation(trans, position[0], position[1], position[2]);
					trans.M11 = (float)dstRect.Width / (float)texture->getWidth();
					trans.M22 = (float)dstRect.Height / (float)texture->getHeight();

					Matrix::Multiply(tempM, trans, baseTrans);

					// As the position have been added to the transform, 
					// draw the texture at the origin
					FillTransformedDraw(texture, tempM, NULL, color);
				}
			}

			void D3D9Sprite::DrawEntry::FillTransformedDraw(Texture* texture, const Matrix& t, const Apoc3D::Math::RectangleF* srcRect, uint color)
			{
				Tex = static_cast<D3D9Texture*>(texture);

				SetColors(color);

				float width;
				float height;

				if (srcRect)
				{
					width = srcRect->Width;
					height = srcRect->Height;
				}
				else
				{
					width = (float)texture->getWidth();
					height = (float)texture->getHeight();
				}
				
				SetPositions(t, { 0, 0 }, { width, 0 }, { 0, height }, { width, height });

				SetSrcRect(texture, srcRect);
			}


			void D3D9Sprite::DrawEntry::SetPositions(const Matrix& baseTrans, const PointF& tl_dp, const PointF& tr_dp, const PointF& bl_dp, const PointF& br_dp)
			{
				const Matrix& trans = baseTrans;

				Vector3 tl = { tl_dp.X, tl_dp.Y, 0 };
				Vector3 tr = { tr_dp.X, tr_dp.Y, 0 };
				Vector3 bl = { bl_dp.X, bl_dp.Y, 0 };
				Vector3 br = { br_dp.X, br_dp.Y, 0 };

				tl = Vector3::TransformCoordinate(tl, trans);
				tr = Vector3::TransformCoordinate(tr, trans);
				bl = Vector3::TransformCoordinate(bl, trans);
				br = Vector3::TransformCoordinate(br, trans);


				TL.Position[0] = tl.X - 0.5f;
				TL.Position[1] = tl.Y - 0.5f;
				TL.Position[2] = tl.Z;
				TL.Position[3] = 1;

				TR.Position[0] = tr.X - 0.5f;
				TR.Position[1] = tr.Y - 0.5f;
				TR.Position[2] = tr.Z;
				TR.Position[3] = 1;

				BL.Position[0] = bl.X - 0.5f;
				BL.Position[1] = bl.Y - 0.5f;
				BL.Position[2] = bl.Z;
				BL.Position[3] = 1;

				BR.Position[0] = br.X - 0.5f;
				BR.Position[1] = br.Y - 0.5f;
				BR.Position[2] = br.Z;
				BR.Position[3] = 1;
			}

			void D3D9Sprite::DrawEntry::SetSrcRect(Texture* texture, const Apoc3D::Math::RectangleF* srcRect)
			{
				if (srcRect)
				{
					float invWidth = 1.0f / (float)texture->getWidth();
					float invHeight = 1.0f / (float)texture->getHeight();

					TL.TexCoord[0] = srcRect->getLeft() * invWidth; TL.TexCoord[1] = srcRect->getTop() * invHeight;
					TR.TexCoord[0] = srcRect->getRight() * invWidth; TR.TexCoord[1] = srcRect->getTop() * invHeight;
					BL.TexCoord[0] = srcRect->getLeft() * invWidth; BL.TexCoord[1] = srcRect->getBottom() * invHeight;
					BR.TexCoord[0] = srcRect->getRight() * invWidth; BR.TexCoord[1] = srcRect->getBottom() * invHeight;
				}
				else
				{
					TL.TexCoord[0] = 0; TL.TexCoord[1] = 0;
					TR.TexCoord[0] = 1; TR.TexCoord[1] = 0;
					BL.TexCoord[0] = 0; BL.TexCoord[1] = 1;
					BR.TexCoord[0] = 1; BR.TexCoord[1] = 1;
				}
			}

			void D3D9Sprite::DrawEntry::SetColors(uint color)
			{
				TL.Diffuse = TR.Diffuse = BL.Diffuse = BR.Diffuse = color;
			}

			void D3D9Sprite::DrawEntry::SetColors(uint tlColor, uint trColor, uint blColor, uint brColor)
			{
				TL.Diffuse = tlColor;
				TR.Diffuse = trColor;
				BL.Diffuse = blColor;
				BR.Diffuse = brColor;
			}

			void D3D9Sprite::DrawEntry::SetTextureCoords(const PointF& tl_sp, const PointF& tr_sp, const PointF& bl_sp, const PointF& br_sp)
			{
				TL.TexCoord[0] = tl_sp.X; TL.TexCoord[1] = tl_sp.Y;
				TR.TexCoord[0] = tr_sp.X; TR.TexCoord[1] = tr_sp.Y;
				BL.TexCoord[0] = bl_sp.X; BL.TexCoord[1] = bl_sp.Y;
				BR.TexCoord[0] = br_sp.X; BR.TexCoord[1] = br_sp.Y;
			}

			void D3D9Sprite::DrawEntry::ChangeUV(float uScale, float vScale, float uBias, float vBias)
			{
				BL.TexCoord[0] *= uScale;
				BR.TexCoord[0] *= uScale;
				TL.TexCoord[0] *= uScale;
				TR.TexCoord[0] *= uScale;

				BL.TexCoord[1] *= vScale;
				BR.TexCoord[1] *= vScale;
				TL.TexCoord[1] *= vScale;
				TR.TexCoord[1] *= vScale;

				BL.TexCoord[0] += uBias;
				BR.TexCoord[0] += uBias;
				TL.TexCoord[0] += uBias;
				TR.TexCoord[0] += uBias;

				BL.TexCoord[1] += vBias;
				BR.TexCoord[1] += vBias;
				TL.TexCoord[1] += vBias;
				TR.TexCoord[1] += vBias;

				IsUVExtended = true;
			}

		}
	}
}