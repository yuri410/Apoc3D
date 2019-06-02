/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "RenderDevice.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Graphics/GeometryData.h"
#include "apoc3d/Graphics/Material.h"
#include "apoc3d/Graphics/EffectSystem/Effect.h"
#include "apoc3d/Graphics/RenderSystem/InstancingData.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Math/Point.h"
#include "apoc3d/Vfs/ResourceLocation.h"

using namespace Apoc3D::Graphics::EffectSystem;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			bool RenderDevice::HasBatchReportRequest = false;

			void RenderDevice::Render(Material* mtrl, const RenderOperation* op, int32 count, int32 passSelID)
			{
				if (HasBatchReportRequest && count > 0)
				{
					BatchReportEntry* entry = m_reportTableByMaterial->TryGetValue(mtrl);

					if (entry == nullptr)
					{
						BatchReportEntry ent;
						ent.DebugName = L"(??)";
#if _DEBUG
						if (!mtrl->DebugName.empty())
							ent.DebugName = mtrl->DebugName;
#endif

						m_reportTableByMaterial->Add(mtrl, ent);
						entry = &m_reportTableByMaterial->operator[](mtrl);
					}

					Effect* fx = mtrl->GetPassEffect(passSelID);

					if (fx->SupportsInstancing())
					{
						const RenderOperation& rop = op[0];
						const GeometryData* gm = rop.GeometryData;

						if (gm->VertexCount == 0 || gm->PrimitiveCount == 0)
							return;

						entry->PrimitiveInstanced += gm->PrimitiveCount*count;
						entry->VertexInstanced += gm->VertexCount*count;
						entry->DPInstanced += count;
						entry->InstancingBatch += count / InstancingData::MaxOneTimeInstances;
						if ((count % InstancingData::MaxOneTimeInstances) != 0)
							entry->InstancingBatch++;
					}
					else
					{
						const RenderOperation& rop = op[0];
						const GeometryData* gm = rop.GeometryData;

						entry->Primitive += gm->PrimitiveCount*count;
						entry->Vertex += gm->VertexCount*count;
						entry->DP += count;
					}
				}
			}


			void RenderDevice::BeginFrame()
			{
				m_batchCount = 0;
				m_primitiveCount = 0;
				m_vertexCount = 0;

				if (HasBatchReportRequest)
				{
					m_reportTableByMaterial = new HashMap<void*, BatchReportEntry>();
				}
			}

			void RenderDevice::EndFrame()
			{
				if (HasBatchReportRequest)
				{
					List<BatchReportEntry*> sortingList(m_reportTableByMaterial->getCount());

					for (BatchReportEntry& bre : m_reportTableByMaterial->getValueAccessor())
					{
						bre.CalculateSorter();

						sortingList.Add(&bre);
					}

					sortingList.Sort([](const BatchReportEntry* a, const BatchReportEntry* b)->int
					{
						return -Apoc3D::Collections::OrderComparer(a->Sorter, b->Sorter);
					});

					for (BatchReportEntry* ent : sortingList)
					{
						String msg;
						msg.append(ent->DebugName);
						msg.append(L"\t DP=");
						msg.append(StringUtils::IntToString(ent->DP));
						msg.append(L"\t I=");
						msg.append(StringUtils::IntToString(ent->InstancingBatch));
						msg.append(L"\t DPI=");
						msg.append(StringUtils::IntToString(ent->DPInstanced));


						msg.append(L"\t TRI=");
						msg.append(StringUtils::IntToString(ent->Primitive));
						msg.append(L"\t VTX=");
						msg.append(StringUtils::IntToString(ent->Vertex));
						msg.append(L"\t EROP=");
						msg.append(StringUtils::IntToString(ent->EmptyROP));

						msg.append(L"\t TRII=");
						msg.append(StringUtils::IntToString(ent->PrimitiveInstanced));
						msg.append(L"\t VTXI=");
						msg.append(StringUtils::IntToString(ent->VertexInstanced));
						msg.append(L"\t EROPI=");
						msg.append(StringUtils::IntToString(ent->EmptyInstancedROP));


						ApocLog(LOG_Graphics, msg, LOGLVL_Default);
					}


					HasBatchReportRequest = false;

					delete m_reportTableByMaterial;
				}
			}

			/************************************************************************/
			/*   BatchReportEntry                                                   */
			/************************************************************************/

			void RenderDevice::BatchReportEntry::CalculateSorter()
			{
				Sorter = DP + InstancingBatch;
			}

			/************************************************************************/
			/* ObjectFactory                                                        */
			/************************************************************************/

			Texture* ObjectFactory::CreateTexture(const Point& size2d, int32 levelCount, TextureUsage usage, PixelFormat format)
			{
				return CreateTexture(size2d.X, size2d.Y, levelCount, usage, format);
			}


			RenderTarget* ObjectFactory::CreateRenderTarget(const Point& size2d, PixelFormat clrFmt, const String& multisampleMode)
			{
				return CreateRenderTarget(size2d.X, size2d.Y, clrFmt, multisampleMode);
			}
			RenderTarget* ObjectFactory::CreateRenderTarget(const Point& size2d, PixelFormat clrFmt)
			{
				return CreateRenderTarget(size2d.X, size2d.Y, clrFmt, L"");
			}
			RenderTarget* ObjectFactory::CreateRenderTarget(int32 width, int32 height, PixelFormat clrFmt)
			{
				return CreateRenderTarget(width, height, clrFmt, L"");
			}


			DepthStencilBuffer* ObjectFactory::CreateDepthStencilBuffer(const Point& size2d, DepthFormat depFmt, const String& multisampleMode)
			{
				return CreateDepthStencilBuffer(size2d.X, size2d.Y, depFmt, multisampleMode);
			}
			DepthStencilBuffer* ObjectFactory::CreateDepthStencilBuffer(const Point& size2d, DepthFormat depFmt)
			{
				return CreateDepthStencilBuffer(size2d.X, size2d.Y, depFmt, L"");
			}
			DepthStencilBuffer* ObjectFactory::CreateDepthStencilBuffer(int32 width, int32 height, DepthFormat depFmt)
			{
				return CreateDepthStencilBuffer(width, height, depFmt, L"");
			}

			Shader* ObjectFactory::CreateVertexShader(const ResourceLocation& resLoc)
			{
				char* codeBuffer = resLoc.ReadAllToAllocatedBuffer();
				Shader* result = CreateVertexShader((byte*)codeBuffer);
				delete[] codeBuffer;
				return result;
			}
			Shader* ObjectFactory::CreatePixelShader(const ResourceLocation& resLoc)
			{
				char* codeBuffer = resLoc.ReadAllToAllocatedBuffer();
				Shader* result = CreatePixelShader((byte*)codeBuffer);
				delete[] codeBuffer;
				return result;
			}
		}
	}
}