#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/BindingSet/BindingSet.h>
#include <RHI/Framebuffer/Framebuffer.h>
#include <RHI/Instance/BaseTypes.h>
#include <RHI/Instance/QueryInterface.h>
#include <RHI/Pipeline/Pipeline.h>
#include <RHI/QueryHeap/QueryHeap.h>
#include <RHI/RHIDLL.h>
#include <RHI/Resource/Resource.h>
#include <RHI/View/View.h>
#include <array>
#include <memory>

class EZ_RHI_DLL CommandList : public QueryInterface
{
public:
  virtual ~CommandList() = default;
  virtual void Reset() = 0;
  virtual void Close() = 0;
  virtual void BindPipeline(const std::shared_ptr<Pipeline>& state) = 0;
  virtual void BindBindingSet(const std::shared_ptr<BindingSet>& binding_set) = 0;
  virtual void BeginRenderPass(const std::shared_ptr<RenderPass>& render_pass, const std::shared_ptr<Framebuffer>& framebuffer, const ClearDesc& clear_desc) = 0;
  virtual void EndRenderPass() = 0;
  virtual void BeginEvent(const ezString& name) = 0;
  virtual void EndEvent() = 0;
  virtual void Draw(ezUInt32 vertex_count, ezUInt32 instanceCount, ezUInt32 first_vertex, ezUInt32 first_instance) = 0;
  virtual void DrawIndexed(ezUInt32 index_count, ezUInt32 instanceCount, ezUInt32 firstIndex, ezInt32 vertexOffset, ezUInt32 first_instance) = 0;
  virtual void DrawIndirect(const std::shared_ptr<Resource>& argument_buffer, ezUInt64 argument_buffer_offset) = 0;
  virtual void DrawIndexedIndirect(const std::shared_ptr<Resource>& argument_buffer, ezUInt64 argument_buffer_offset) = 0;
  virtual void DrawIndirectCount(
    const std::shared_ptr<Resource>& argument_buffer,
    ezUInt64 argument_buffer_offset,
    const std::shared_ptr<Resource>& count_buffer,
    ezUInt64 count_buffer_offset,
    ezUInt32 max_draw_count,
    ezUInt32 stride) = 0;
  virtual void DrawIndexedIndirectCount(
    const std::shared_ptr<Resource>& argument_buffer,
    ezUInt64 argument_buffer_offset,
    const std::shared_ptr<Resource>& count_buffer,
    ezUInt64 count_buffer_offset,
    ezUInt32 max_draw_count,
    ezUInt32 stride) = 0;
  virtual void Dispatch(ezUInt32 thread_group_count_x, ezUInt32 thread_group_count_y, ezUInt32 thread_group_count_z) = 0;
  virtual void DispatchIndirect(const std::shared_ptr<Resource>& argument_buffer, ezUInt64 argument_buffer_offset) = 0;
  virtual void DispatchMesh(ezUInt32 thread_group_count_x) = 0;
  virtual void DispatchRays(const RayTracingShaderTables& shader_tables, ezUInt32 width, ezUInt32 height, ezUInt32 depth) = 0;
  virtual void ResourceBarrier(const std::vector<ResourceBarrierDesc>& barriers) = 0;
  virtual void UAVResourceBarrier(const std::shared_ptr<Resource>& resource) = 0;
  virtual void SetViewport(float x, float y, float width, float height) = 0;
  virtual void SetScissorRect(ezInt32 left, ezInt32 top, ezUInt32 right, ezUInt32 bottom) = 0;
  virtual void IASetIndexBuffer(const std::shared_ptr<Resource>& resource, ezRHIResourceFormat::Enum format) = 0;
  virtual void IASetVertexBuffer(ezUInt32 slot, const std::shared_ptr<Resource>& resource) = 0;
  virtual void RSSetShadingRate(ShadingRate shading_rate, const std::array<ShadingRateCombiner, 2>& combiners) = 0;
  virtual void BuildBottomLevelAS(
    const std::shared_ptr<Resource>& src,
    const std::shared_ptr<Resource>& dst,
    const std::shared_ptr<Resource>& scratch,
    ezUInt64 scratch_offset,
    const std::vector<RaytracingGeometryDesc>& descs,
    BuildAccelerationStructureFlags flags) = 0;
  virtual void BuildTopLevelAS(
    const std::shared_ptr<Resource>& src,
    const std::shared_ptr<Resource>& dst,
    const std::shared_ptr<Resource>& scratch,
    ezUInt64 scratch_offset,
    const std::shared_ptr<Resource>& instance_data,
    ezUInt64 instance_offset,
    ezUInt32 instanceCount,
    BuildAccelerationStructureFlags flags) = 0;
  virtual void CopyAccelerationStructure(const std::shared_ptr<Resource>& src, const std::shared_ptr<Resource>& dst, CopyAccelerationStructureMode mode) = 0;
  virtual void CopyBuffer(const std::shared_ptr<Resource>& src_buffer, const std::shared_ptr<Resource>& dst_buffer,
    const std::vector<BufferCopyRegion>& regions) = 0;
  virtual void CopyBufferToTexture(const std::shared_ptr<Resource>& src_buffer, const std::shared_ptr<Resource>& dst_texture,
    const std::vector<BufferToTextureCopyRegion>& regions) = 0;
  virtual void CopyTexture(const std::shared_ptr<Resource>& src_texture, const std::shared_ptr<Resource>& dst_texture,
    const std::vector<TextureCopyRegion>& regions) = 0;
  virtual void WriteAccelerationStructuresProperties(
    const std::vector<std::shared_ptr<Resource>>& acceleration_structures,
    const std::shared_ptr<QueryHeap>& query_heap,
    ezUInt32 first_query) = 0;
  virtual void ResolveQueryData(
    const std::shared_ptr<QueryHeap>& query_heap,
    ezUInt32 first_query,
    ezUInt32 query_count,
    const std::shared_ptr<Resource>& dst_buffer,
    ezUInt64 dst_offset) = 0;
};
