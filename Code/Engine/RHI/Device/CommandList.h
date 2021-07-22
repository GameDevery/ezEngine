#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Descriptors/Descriptors.h>

class EZ_RHI_DLL ezRHICommandList
{
public:
  virtual ~ezRHICommandList() = default;

  void Reset();

  void Close();

  void BindPipeline(ezRHIPipelineHandle hPipeline);

  void BindBindingSet(ezRHIBindingSetHandle hBindingSet);

  void BeginRenderPass(ezRHIRenderPassHandle hRenderPass, ezRHIFramebufferHandle hFramebuffer, const ezRHIClearDescription& clearDesc);

  void EndRenderPass();

  void BeginEvent(const ezString& name);

  void EndEvent();

  void Draw(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 firstVertex, ezUInt32 firstInstance);

  void DrawIndexed(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 firstIndex, ezInt32 vertexOffset, ezUInt32 firstInstance);

  void DrawIndirect(ezRHIResourceHandle hArgumentBuffer, ezUInt64 argumentBufferOffset);

  void DrawIndexedIndirect(ezRHIResourceHandle hArgumentBuffer, ezUInt64 argumentBufferOffset);

  void DrawIndirectCount(
    ezRHIResourceHandle hArgumentBuffer,
    ezUInt64 argumentBufferOffset,
    ezRHIResourceHandle hCountBuffer,
    ezUInt64 countBufferOffset,
    ezUInt32 maxDrawCount,
    ezUInt32 stride);

  void DrawIndexedIndirectCount(
    ezRHIResourceHandle hArgumentBuffer,
    ezUInt64 argumentBufferOffset,
    ezRHIResourceHandle hCountBuffer,
    ezUInt64 countBufferOffset,
    ezUInt32 maxDrawCount,
    ezUInt32 stride);

  void Dispatch(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ);

  void DispatchIndirect(ezRHIResourceHandle hArgumentBuffer, ezUInt64 argumentBufferOffset);

  void DispatchMesh(ezUInt32 threadGroupCountX);

  void DispatchRays(const ezRHIRayTracingShaderTables& shaderTables, ezUInt32 width, ezUInt32 height, ezUInt32 depth);

  void ResourceBarrier(const ezDynamicArray<ezRHIResourceBarrierDescription>& barriers);

  void UAVResourceBarrier(ezRHIResourceHandle hResource);

  void SetViewport(float x, float y, float width, float height);

  void SetScissorRect(ezInt32 left, ezInt32 top, ezUInt32 right, ezUInt32 bottom);

  void IASetIndexBuffer(ezRHIResourceHandle hBuffer, ezRHIResourceFormat format);

  void IASetVertexBuffer(ezUInt32 slot, ezRHIResourceHandle hBuffer);

  void RSSetShadingRate(ezRHIShadingRate shadingRate, const ezStaticArray<ezRHIShadingRateCombiner, 2>& combiners);

  void BuildBottomLevelAS(
    ezRHIResourceHandle hSrc,
    ezRHIResourceHandle hDst,
    ezRHIResourceHandle hScratch,
    ezUInt64 scratchOffset,
    const ezDynamicArray<ezRHIRayTracingGeometryDescription>& descs,
    ezRHIBuildAccelerationStructureFlags flags);

  void BuildTopLevelAS(
    ezRHIResourceHandle hSrc,
    ezRHIResourceHandle hDst,
    ezRHIResourceHandle hScratch,
    ezUInt64 scratchOffset,
    ezRHIResourceHandle hInstanceData,
    ezUInt64 instanceOffset,
    ezUInt32 instanceCount,
    ezRHIBuildAccelerationStructureFlags flags);
  void CopyAccelerationStructure(ezRHIResourceHandle hSrc, ezRHIResourceHandle hDst, ezRHICopyAccelerationStructureMode mode);

  void CopyBuffer(ezRHIResourceHandle hSrcBuffer, ezRHIResourceHandle hDstBuffer,
    const ezDynamicArray<ezRHIBufferCopyRegion>& regions);

  void CopyBufferToTexture(ezRHIResourceHandle hSrcBuffer, ezRHIResourceHandle hDstTexture,
    const ezDynamicArray<ezRHIBufferToTextureCopyRegion>& regions);

  void CopyTexture(ezRHIResourceHandle hSrcTexture, ezRHIResourceHandle hDstTexture,
    const ezDynamicArray<ezRHITextureCopyRegion>& regions);

  void WriteAccelerationStructuresProperties(
    const ezDynamicArray<ezRHIResource*>& accelerationStructures,
    ezRHIQueryHeapHandle hQueryHeap,
    ezUInt32 firstQuery);

  void ResolveQueryData(
    ezRHIQueryHeapHandle hQueryHeap,
    ezUInt32 firstQuery,
    ezUInt32 queryCount,
    ezRHIResourceHandle hDstBuffer,
    ezUInt64 dstOffset);

protected:
  ezRHICommandList(ezRHIDevice* pDevice);

  virtual void ResetPlatform() = 0;

  virtual void ClosePlatform() = 0;

  virtual void BindPipelinePlatform(ezRHIPipeline* pPipeline) = 0;

  virtual void BindBindingSetPlatform(ezRHIBindingSet* pBindingSet) = 0;

  virtual void BeginRenderPassPlatform(ezRHIRenderPass* pRenderPass, ezRHIFramebuffer* pFramebuffer, const ezRHIClearDescription& clearDesc) = 0;

  virtual void EndRenderPassPlatform() = 0;

  virtual void BeginEventPlatform(const ezString& name) = 0;

  virtual void EndEventPlatform() = 0;

  virtual void DrawPlatform(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 firstVertex, ezUInt32 firstInstance) = 0;

  virtual void DrawIndexedPlatform(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 firstIndex, ezInt32 vertexOffset, ezUInt32 firstInstance) = 0;

  virtual void DrawIndirectPlatform(ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset) = 0;

  virtual void DrawIndexedIndirectPlatform(ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset) = 0;

  virtual void DrawIndirectCountPlatform(
    ezRHIResource* pArgumentBuffer,
    ezUInt64 argumentBufferOffset,
    ezRHIResource* pCountBuffer,
    ezUInt64 countBufferOffset,
    ezUInt32 maxDrawCount,
    ezUInt32 stride) = 0;

  virtual void DrawIndexedIndirectCountPlatform(
    ezRHIResource* pArgumentBuffer,
    ezUInt64 argumentBufferOffset,
    ezRHIResource* pCountBuffer,
    ezUInt64 countBufferOffset,
    ezUInt32 maxDrawCount,
    ezUInt32 stride) = 0;

  virtual void DispatchPlatform(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ) = 0;

  virtual void DispatchIndirectPlatform(ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset) = 0;

  virtual void DispatchMeshPlatform(ezUInt32 threadGroupCountX) = 0;

  virtual void DispatchRaysPlatform(const ezRHIRayTracingShaderTables& shaderTables, ezUInt32 width, ezUInt32 height, ezUInt32 depth) = 0;

  virtual void ResourceBarrierPlatform(const ezDynamicArray<ezRHIResourceBarrierDescription>& barriers) = 0;

  virtual void UAVResourceBarrierPlatform(ezRHIResource* pResource) = 0;

  virtual void SetViewportPlatform(float x, float y, float width, float height) = 0;

  virtual void SetScissorRectPlatform(ezInt32 left, ezInt32 top, ezUInt32 right, ezUInt32 bottom) = 0;

  virtual void IASetIndexBufferPlatform(ezRHIResource* pBuffer, ezRHIResourceFormat format) = 0;

  virtual void IASetVertexBufferPlatform(ezUInt32 slot, ezRHIResource* pBuffer) = 0;

  virtual void RSSetShadingRatePlatform(ezRHIShadingRate shadingRate, const ezStaticArray<ezRHIShadingRateCombiner, 2>& combiners) = 0;

  virtual void BuildBottomLevelASPlatform(
    ezRHIResource* pSrc,
    ezRHIResource* pDst,
    ezRHIResource* pScratch,
    ezUInt64 scratchOffset,
    const ezDynamicArray<ezRHIRayTracingGeometryDescription>& descs,
    ezRHIBuildAccelerationStructureFlags flags) = 0;

  virtual void BuildTopLevelASPlatform(
    ezRHIResource* pSrc,
    ezRHIResource* pDst,
    ezRHIResource* pScratch,
    ezUInt64 scratchOffset,
    ezRHIResource* pInstanceData,
    ezUInt64 instanceOffset,
    ezUInt32 instanceCount,
    ezRHIBuildAccelerationStructureFlags flags) = 0;
  virtual void CopyAccelerationStructurePlatform(ezRHIResource* pSrc, ezRHIResource* pDst, ezRHICopyAccelerationStructureMode mode) = 0;

  virtual void CopyBufferPlatform(ezRHIResource* pSrcBuffer, ezRHIResource* pDstBuffer,
    const ezDynamicArray<ezRHIBufferCopyRegion>& regions) = 0;

  virtual void CopyBufferToTexturePlatform(ezRHIResource* pSrcBuffer, ezRHIResource* pDstTexture,
    const ezDynamicArray<ezRHIBufferToTextureCopyRegion>& regions) = 0;

  virtual void CopyTexturePlatform(ezRHIResource* pSrcTexture, ezRHIResource* pDstTexture,
    const ezDynamicArray<ezRHITextureCopyRegion>& regions) = 0;

  virtual void WriteAccelerationStructuresPropertiesPlatform(
    const ezDynamicArray<ezRHIResource*>& accelerationStructures,
    ezRHIQueryHeap* pQueryHeap,
    ezUInt32 firstQuery) = 0;

  virtual void ResolveQueryDataPlatform(
    ezRHIQueryHeap* pQueryHeap,
    ezUInt32 firstQuery,
    ezUInt32 queryCount,
    ezRHIResource* pDstBuffer,
    ezUInt64 dstOffset) = 0;

private:
  ezRHIDevice* m_pDevice;
};
