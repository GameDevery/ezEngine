#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/BindingSet/BindingSet.h>
#include <RHI/Framebuffer/Framebuffer.h>
#include <RHI/Instance/BaseTypes.h>
#include <RHI/Pipeline/Pipeline.h>
#include <RHI/QueryHeap/QueryHeap.h>
#include <RHI/RHIDLL.h>
#include <RHI/Resource/Resource.h>
#include <RHI/View/View.h>
#include <array>
#include <memory>

class EZ_RHI_DLL CommandList : public ezRefCounted
{
public:
  virtual ~CommandList() = default;
  virtual void Reset() = 0;
  virtual void Close() = 0;
  virtual void BindPipeline(const ezSharedPtr<Pipeline>& state) = 0;
  virtual void BindBindingSet(const ezSharedPtr<BindingSet>& binding_set) = 0;
  virtual void BeginRenderPass(const ezSharedPtr<RenderPass>& render_pass, const ezSharedPtr<Framebuffer>& framebuffer, const ClearDesc& clear_desc) = 0;
  virtual void EndRenderPass() = 0;
  virtual void BeginEvent(const ezString& name) = 0;
  virtual void EndEvent() = 0;
  virtual void Draw(ezUInt32 vertex_count, ezUInt32 instanceCount, ezUInt32 first_vertex, ezUInt32 first_instance) = 0;
  virtual void DrawIndexed(ezUInt32 index_count, ezUInt32 instanceCount, ezUInt32 firstIndex, ezInt32 vertexOffset, ezUInt32 first_instance) = 0;
  virtual void DrawIndirect(const ezSharedPtr<Resource>& pArgumentBuffer, ezUInt64 argument_buffer_offset) = 0;
  virtual void DrawIndexedIndirect(const ezSharedPtr<Resource>& pArgumentBuffer, ezUInt64 argument_buffer_offset) = 0;
  virtual void DrawIndirectCount(
    const ezSharedPtr<Resource>& pArgumentBuffer,
    ezUInt64 argument_buffer_offset,
    const ezSharedPtr<Resource>& count_buffer,
    ezUInt64 count_buffer_offset,
    ezUInt32 max_draw_count,
    ezUInt32 stride) = 0;
  virtual void DrawIndexedIndirectCount(
    const ezSharedPtr<Resource>& pArgumentBuffer,
    ezUInt64 argument_buffer_offset,
    const ezSharedPtr<Resource>& count_buffer,
    ezUInt64 count_buffer_offset,
    ezUInt32 max_draw_count,
    ezUInt32 stride) = 0;
  virtual void Dispatch(ezUInt32 thread_group_count_x, ezUInt32 thread_group_count_y, ezUInt32 thread_group_count_z) = 0;
  virtual void DispatchIndirect(const ezSharedPtr<Resource>& pArgumentBuffer, ezUInt64 argument_buffer_offset) = 0;
  virtual void DispatchMesh(ezUInt32 thread_group_count_x) = 0;
  virtual void DispatchRays(const RayTracingShaderTables& shader_tables, ezUInt32 width, ezUInt32 height, ezUInt32 depth) = 0;
  virtual void ResourceBarrier(const std::vector<ResourceBarrierDesc>& barriers) = 0;
  virtual void UAVResourceBarrier(const ezSharedPtr<Resource>& resource) = 0;
  virtual void SetViewport(float x, float y, float width, float height) = 0;
  virtual void SetScissorRect(ezInt32 left, ezInt32 top, ezUInt32 right, ezUInt32 bottom) = 0;
  virtual void IASetIndexBuffer(const ezSharedPtr<Resource>& resource, ezRHIResourceFormat::Enum format) = 0;
  virtual void IASetVertexBuffer(ezUInt32 slot, const ezSharedPtr<Resource>& resource) = 0;
  virtual void RSSetShadingRate(ShadingRate shading_rate, const std::array<ShadingRateCombiner, 2>& combiners) = 0;
  virtual void BuildBottomLevelAS(
    const ezSharedPtr<Resource>& src,
    const ezSharedPtr<Resource>& dst,
    const ezSharedPtr<Resource>& scratch,
    ezUInt64 scratch_offset,
    const std::vector<RaytracingGeometryDesc>& descs,
    BuildAccelerationStructureFlags flags) = 0;
  virtual void BuildTopLevelAS(
    const ezSharedPtr<Resource>& src,
    const ezSharedPtr<Resource>& dst,
    const ezSharedPtr<Resource>& scratch,
    ezUInt64 scratch_offset,
    const ezSharedPtr<Resource>& instance_data,
    ezUInt64 instance_offset,
    ezUInt32 instanceCount,
    BuildAccelerationStructureFlags flags) = 0;
  virtual void CopyAccelerationStructure(const ezSharedPtr<Resource>& src, const ezSharedPtr<Resource>& dst, CopyAccelerationStructureMode mode) = 0;
  virtual void CopyBuffer(const ezSharedPtr<Resource>& src_buffer, const ezSharedPtr<Resource>& dst_buffer,
    const std::vector<BufferCopyRegion>& regions) = 0;
  virtual void CopyBufferToTexture(const ezSharedPtr<Resource>& src_buffer, const ezSharedPtr<Resource>& dst_texture,
    const std::vector<BufferToTextureCopyRegion>& regions) = 0;
  virtual void CopyTexture(const ezSharedPtr<Resource>& src_texture, const ezSharedPtr<Resource>& dst_texture,
    const std::vector<TextureCopyRegion>& regions) = 0;
  virtual void WriteAccelerationStructuresProperties(
    const std::vector<ezSharedPtr<Resource>>& acceleration_structures,
    const ezSharedPtr<QueryHeap>& query_heap,
    ezUInt32 first_query) = 0;
  virtual void ResolveQueryData(
    const ezSharedPtr<QueryHeap>& query_heap,
    ezUInt32 first_query,
    ezUInt32 query_count,
    const ezSharedPtr<Resource>& dst_buffer,
    ezUInt64 dst_offset) = 0;
};
