#include <RHIPCH.h>

#include <RHI/Device/CommandList.h>
#include <RHI/Device/Device.h>

ezRHICommandList::ezRHICommandList(ezRHIDevice* pDevice)
  : m_pDevice{pDevice}
{
}

void ezRHICommandList::Reset()
{
  ResetPlatform();
}

void ezRHICommandList::Close()
{
  ClosePlatform();
}

void ezRHICommandList::BindPipeline(ezRHIPipelineHandle hPipeline)
{
  ezRHIPipeline* pPipeline = m_pDevice->GetPipeline(hPipeline);
  EZ_ASSERT_DEV(pPipeline != nullptr, "");
  BindPipelinePlatform(pPipeline);
}

void ezRHICommandList::BindBindingSet(ezRHIBindingSetHandle hBindingSet)
{
  ezRHIBindingSet* pBindingSet = m_pDevice->GetBindingSet(hBindingSet);
  EZ_ASSERT_DEV(pBindingSet != nullptr, "");
  BindBindingSetPlatform(pBindingSet);
}

void ezRHICommandList::BeginRenderPass(ezRHIRenderPassHandle hRenderPass, ezRHIFramebufferHandle hFramebuffer, const ezRHIClearDescription& clearDesc)
{
  ezRHIRenderPass* pRenderPass = m_pDevice->GetRenderPass(hRenderPass);

  ezRHIFramebuffer* pFramebuffer = m_pDevice->GetFramebuffer(hFramebuffer);

  BeginRenderPassPlatform(pRenderPass, pFramebuffer, clearDesc);
}

void ezRHICommandList::EndRenderPass()
{
  EndRenderPassPlatform();
}

void ezRHICommandList::BeginEvent(const ezString& name)
{
  BeginEventPlatform(name);
}

void ezRHICommandList::EndEvent()
{
  EndEventPlatform();
}

void ezRHICommandList::Draw(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 firstVertex, ezUInt32 firstInstance)
{
  DrawPlatform(vertexCount, instanceCount, firstVertex, firstInstance);
}

void ezRHICommandList::DrawIndexed(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 firstIndex, ezInt32 vertexOffset, ezUInt32 firstInstance)
{
  DrawIndexedPlatform(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void ezRHICommandList::DrawIndirect(ezRHIResourceHandle hArgumentBuffer, ezUInt64 argumentBufferOffset)
{
  ezRHIResource* pArgumentBuffer = m_pDevice->GetResource(hArgumentBuffer);
  EZ_ASSERT_DEV(pArgumentBuffer != nullptr, "");

  DrawIndirectPlatform(pArgumentBuffer, argumentBufferOffset);
}

void ezRHICommandList::DrawIndexedIndirect(ezRHIResourceHandle hArgumentBuffer, ezUInt64 argumentBufferOffset)
{
  ezRHIResource* pArgumentBuffer = m_pDevice->GetResource(hArgumentBuffer);
  EZ_ASSERT_DEV(pArgumentBuffer != nullptr, "");

  DrawIndexedIndirectPlatform(pArgumentBuffer, argumentBufferOffset);
}

void ezRHICommandList::DrawIndirectCount(ezRHIResourceHandle hArgumentBuffer, ezUInt64 argumentBufferOffset, ezRHIResourceHandle hCountBuffer, ezUInt64 countBufferOffset, ezUInt32 maxDrawCount, ezUInt32 stride)
{
  ezRHIResource* pArgumentBuffer = m_pDevice->GetResource(hArgumentBuffer);
  EZ_ASSERT_DEV(pArgumentBuffer != nullptr, "");

  ezRHIResource* pCountBuffer = m_pDevice->GetResource(hCountBuffer);
  EZ_ASSERT_DEV(pCountBuffer != nullptr, "");

  DrawIndirectCountPlatform(pArgumentBuffer, argumentBufferOffset, pCountBuffer, countBufferOffset, maxDrawCount, stride);
}

void ezRHICommandList::DrawIndexedIndirectCount(ezRHIResourceHandle hArgumentBuffer, ezUInt64 argumentBufferOffset, ezRHIResourceHandle hCountBuffer, ezUInt64 countBufferOffset, ezUInt32 maxDrawCount, ezUInt32 stride)
{
  ezRHIResource* pArgumentBuffer = m_pDevice->GetResource(hArgumentBuffer);
  EZ_ASSERT_DEV(pArgumentBuffer != nullptr, "");

  ezRHIResource* pCountBuffer = m_pDevice->GetResource(hCountBuffer);
  EZ_ASSERT_DEV(pCountBuffer != nullptr, "");

  DrawIndexedIndirectCountPlatform(pArgumentBuffer, argumentBufferOffset, pCountBuffer, countBufferOffset, maxDrawCount, stride);
}

void ezRHICommandList::Dispatch(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ)
{
  DispatchPlatform(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

void ezRHICommandList::DispatchIndirect(ezRHIResourceHandle hArgumentBuffer, ezUInt64 argumentBufferOffset)
{
  ezRHIResource* pArgumentBuffer = m_pDevice->GetResource(hArgumentBuffer);
  EZ_ASSERT_DEV(pArgumentBuffer != nullptr, "");

  DispatchIndirectPlatform(pArgumentBuffer, argumentBufferOffset);
}

void ezRHICommandList::DispatchMesh(ezUInt32 threadGroupCountX)
{
  DispatchMeshPlatform(threadGroupCountX);
}

void ezRHICommandList::DispatchRays(const ezRHIRayTracingShaderTables& shaderTables, ezUInt32 width, ezUInt32 height, ezUInt32 depth)
{
  DispatchRaysPlatform(shaderTables, width, height, depth);
}

void ezRHICommandList::ResourceBarrier(const ezDynamicArray<ezRHIResourceBarrierDescription>& barriers)
{
  ResourceBarrierPlatform(barriers);
}

void ezRHICommandList::UAVResourceBarrier(ezRHIResourceHandle hResource)
{
  ezRHIResource* pResource = m_pDevice->GetResource(hResource);
  EZ_ASSERT_DEV(pResource != nullptr, "");

  UAVResourceBarrierPlatform(pResource);
}

void ezRHICommandList::SetViewport(float x, float y, float width, float height)
{
  SetViewportPlatform(x, y, width, height);
}

void ezRHICommandList::SetScissorRect(ezInt32 left, ezInt32 top, ezUInt32 right, ezUInt32 bottom)
{
  SetScissorRectPlatform(left, top, right, bottom);
}

void ezRHICommandList::IASetIndexBuffer(ezRHIResourceHandle hBuffer, ezRHIResourceFormat format)
{
  ezRHIResource* pBuffer = m_pDevice->GetResource(hBuffer);
  EZ_ASSERT_DEV(pBuffer != nullptr, "");

  IASetIndexBufferPlatform(pBuffer, format);
}

void ezRHICommandList::IASetVertexBuffer(ezUInt32 slot, ezRHIResourceHandle hBuffer)
{
  ezRHIResource* pBuffer = m_pDevice->GetResource(hBuffer);
  EZ_ASSERT_DEV(pBuffer != nullptr, "");

  IASetVertexBufferPlatform(slot, pBuffer);
}

void ezRHICommandList::RSSetShadingRate(ezRHIShadingRate shadingRate, const ezStaticArray<ezRHIShadingRateCombiner, 2>& combiners)
{
  RSSetShadingRatePlatform(shadingRate, combiners);
}

void ezRHICommandList::BuildBottomLevelAS(ezRHIResourceHandle hSrc, ezRHIResourceHandle hDst, ezRHIResourceHandle hScratch, ezUInt64 scratchOffset, const ezDynamicArray<ezRHIRayTracingGeometryDescription>& descs, ezRHIBuildAccelerationStructureFlags flags)
{
  ezRHIResource* pSrc = m_pDevice->GetResource(hSrc);
  EZ_ASSERT_DEV(pSrc != nullptr, "");

  ezRHIResource* pDst = m_pDevice->GetResource(hDst);
  EZ_ASSERT_DEV(pDst != nullptr, "");

  ezRHIResource* pScratch = m_pDevice->GetResource(hScratch);
  EZ_ASSERT_DEV(pScratch != nullptr, "");

  BuildBottomLevelASPlatform(pSrc, pDst, pScratch, scratchOffset, descs, flags);
}

void ezRHICommandList::BuildTopLevelAS(ezRHIResourceHandle hSrc, ezRHIResourceHandle hDst, ezRHIResourceHandle hScratch, ezUInt64 scratchOffset, ezRHIResourceHandle hInstanceData, ezUInt64 instanceOffset, ezUInt32 instanceCount, ezRHIBuildAccelerationStructureFlags flags)
{

  ezRHIResource* pSrc = m_pDevice->GetResource(hSrc);
  EZ_ASSERT_DEV(pSrc != nullptr, "");

  ezRHIResource* pDst = m_pDevice->GetResource(hDst);
  EZ_ASSERT_DEV(pDst != nullptr, "");

  ezRHIResource* pScratch = m_pDevice->GetResource(hScratch);
  EZ_ASSERT_DEV(pScratch != nullptr, "");

  ezRHIResource* pInstanceData = m_pDevice->GetResource(hInstanceData);
  EZ_ASSERT_DEV(pInstanceData != nullptr, "");

  BuildTopLevelASPlatform(pSrc, pDst, pScratch, scratchOffset, pInstanceData, instanceOffset, instanceCount, flags);
}

void ezRHICommandList::CopyAccelerationStructure(ezRHIResourceHandle hSrc, ezRHIResourceHandle hDst, ezRHICopyAccelerationStructureMode mode)
{
  ezRHIResource* pSrc = m_pDevice->GetResource(hSrc);
  EZ_ASSERT_DEV(pSrc != nullptr, "");

  ezRHIResource* pDst = m_pDevice->GetResource(hDst);
  EZ_ASSERT_DEV(pDst != nullptr, "");

  CopyAccelerationStructurePlatform(pSrc, pDst, mode);
}

void ezRHICommandList::CopyBuffer(ezRHIResourceHandle hSrcBuffer, ezRHIResourceHandle hDstBuffer, const ezDynamicArray<ezRHIBufferCopyRegion>& regions)
{
  ezRHIResource* pSrcBuffer = m_pDevice->GetResource(hSrcBuffer);
  EZ_ASSERT_DEV(pSrcBuffer != nullptr, "");

  ezRHIResource* pDstBuffer = m_pDevice->GetResource(hDstBuffer);
  EZ_ASSERT_DEV(pDstBuffer != nullptr, "");

  CopyBufferPlatform(pSrcBuffer, pDstBuffer, regions);
}

void ezRHICommandList::CopyBufferToTexture(ezRHIResourceHandle hSrcBuffer, ezRHIResourceHandle hDstTexture, const ezDynamicArray<ezRHIBufferToTextureCopyRegion>& regions)
{
  ezRHIResource* pSrcBuffer = m_pDevice->GetResource(hSrcBuffer);
  EZ_ASSERT_DEV(pSrcBuffer != nullptr, "");

  ezRHIResource* pDstTexture = m_pDevice->GetResource(hDstTexture);
  EZ_ASSERT_DEV(pDstTexture != nullptr, "");

  CopyBufferToTexturePlatform(pSrcBuffer, pDstTexture, regions);
}

void ezRHICommandList::CopyTexture(ezRHIResourceHandle hSrcTexture, ezRHIResourceHandle hDstTexture, const ezDynamicArray<ezRHITextureCopyRegion>& regions)
{
  ezRHIResource* pSrcTexture = m_pDevice->GetResource(hSrcTexture);
  EZ_ASSERT_DEV(pSrcTexture != nullptr, "");

  ezRHIResource* pDstTexture = m_pDevice->GetResource(hDstTexture);
  EZ_ASSERT_DEV(pDstTexture != nullptr, "");

  CopyTexturePlatform(pSrcTexture, pDstTexture, regions);
}

void ezRHICommandList::WriteAccelerationStructuresProperties(const ezDynamicArray<ezRHIResource*>& accelerationStructures, ezRHIQueryHeapHandle hQueryHeap, ezUInt32 firstQuery)
{
  ezRHIQueryHeap* pQueryHeap = m_pDevice->GetQueryHeap(hQueryHeap);
  EZ_ASSERT_DEV(pQueryHeap != nullptr, "");

  WriteAccelerationStructuresPropertiesPlatform(accelerationStructures, pQueryHeap, firstQuery);
}

void ezRHICommandList::ResolveQueryData(ezRHIQueryHeapHandle hQueryHeap, ezUInt32 firstQuery, ezUInt32 queryCount, ezRHIResourceHandle hDstBuffer, ezUInt64 dstOffset)
{
  ezRHIQueryHeap* pQueryHeap = m_pDevice->GetQueryHeap(hQueryHeap);
  EZ_ASSERT_DEV(pQueryHeap != nullptr, "");

  ezRHIResource* pDstBuffer = m_pDevice->GetResource(hDstBuffer);
  EZ_ASSERT_DEV(pDstBuffer != nullptr, "");

  ResolveQueryDataPlatform(pQueryHeap, firstQuery, queryCount, pDstBuffer, dstOffset);
}
