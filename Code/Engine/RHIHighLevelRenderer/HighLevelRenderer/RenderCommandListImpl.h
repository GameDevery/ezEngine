#pragma once
#include <RHI/Device/Device.h>
#include <RHIHighLevelRenderer/HighLevelRenderer/ObjectCache.h>
#include <RHIHighLevelRenderer/HighLevelRenderer/RenderCommandList.h>
#include <RHIHighLevelRenderer/RHIHighLevelRendererDLL.h>

struct EZ_RHIHIGHLEVELRENDERER_DLL LazyResourceBarrierDesc
{
  ezSharedPtr<Resource> resource;
  ResourceState state;
  ezUInt32 base_mip_level = 0;
  ezUInt32 level_count = 1;
  ezUInt32 base_array_layer = 0;
  ezUInt32 layer_count = 1;
};

constexpr bool kUseFakeClose = true;

class EZ_RHIHIGHLEVELRENDERER_DLL RenderCommandListImpl : public RenderCommandList
{
public:
  RenderCommandListImpl(Device& device, ObjectCache& object_cache, CommandListType type);
  const ezSharedPtr<CommandList>& GetCommandList();
  void LazyResourceBarrier(const std::vector<LazyResourceBarrierDesc>& barriers);
  const std::map<ezSharedPtr<Resource>, ResourceStateTracker>& GetResourceStateTrackers() const;
  const std::vector<ResourceBarrierDesc>& GetLazyBarriers() const;

  void Reset() override;
  void Close() override;
  void Attach(const BindKey& bind_key, const ezSharedPtr<Resource>& resource = {}, const LazyViewDesc& view_desc = {}) override;
  void Attach(const BindKey& bind_key, const std::shared_ptr<DeferredView>& view) override;
  void Attach(const BindKey& bind_key, const ezSharedPtr<View>& view) override;
  void SetRasterizeState(const RasterizerDesc& desc) override;
  void SetBlendState(const BlendDesc& desc) override;
  void SetDepthStencilState(const DepthStencilDesc& desc) override;
  void UseProgram(const std::shared_ptr<Program>& program) override;
  void BeginRenderPass(const RenderPassBeginDesc& desc) override;
  void EndRenderPass() override;
  void BeginEvent(const ezString& name) override;
  void EndEvent() override;
  void Draw(ezUInt32 vertex_count, ezUInt32 instance_count, ezUInt32 first_vertex, ezUInt32 first_instance) override;
  void DrawIndexed(ezUInt32 index_count, ezUInt32 instance_count, ezUInt32 first_index, int32_t vertex_offset, ezUInt32 first_instance) override;
  void DrawIndirect(const ezSharedPtr<Resource>& pArgumentBuffer, ezUInt64 argument_buffer_offset) override;
  void DrawIndexedIndirect(const ezSharedPtr<Resource>& pArgumentBuffer, ezUInt64 argument_buffer_offset) override;
  void DrawIndirectCount(
    const ezSharedPtr<Resource>& pArgumentBuffer,
    ezUInt64 argument_buffer_offset,
    const ezSharedPtr<Resource>& count_buffer,
    ezUInt64 count_buffer_offset,
    ezUInt32 max_draw_count,
    ezUInt32 stride) override;
  void DrawIndexedIndirectCount(
    const ezSharedPtr<Resource>& pArgumentBuffer,
    ezUInt64 argument_buffer_offset,
    const ezSharedPtr<Resource>& count_buffer,
    ezUInt64 count_buffer_offset,
    ezUInt32 max_draw_count,
    ezUInt32 stride) override;
  void Dispatch(ezUInt32 thread_group_count_x, ezUInt32 thread_group_count_y, ezUInt32 thread_group_count_z) override;
  void DispatchIndirect(const ezSharedPtr<Resource>& pArgumentBuffer, ezUInt64 argument_buffer_offset) override;
  void DispatchMesh(ezUInt32 thread_group_count_x) override;
  void DispatchRays(ezUInt32 width, ezUInt32 height, ezUInt32 depth) override;
  void SetViewport(float x, float y, float width, float height) override;
  void SetScissorRect(int32_t left, int32_t top, ezUInt32 right, ezUInt32 bottom) override;
  void IASetIndexBuffer(const ezSharedPtr<Resource>& resource, ezRHIResourceFormat::Enum format) override;
  void IASetVertexBuffer(ezUInt32 slot, const ezSharedPtr<Resource>& resource) override;
  void RSSetShadingRateImage(const ezSharedPtr<View>& view) override;
  void BuildBottomLevelAS(const ezSharedPtr<Resource>& src, const ezSharedPtr<Resource>& dst, const std::vector<RaytracingGeometryDesc>& descs, BuildAccelerationStructureFlags flags = BuildAccelerationStructureFlags::kNone) override;
  void BuildTopLevelAS(const ezSharedPtr<Resource>& src, const ezSharedPtr<Resource>& dst, const std::vector<std::pair<ezSharedPtr<Resource>, ezMat4>>& geometry, BuildAccelerationStructureFlags flags = BuildAccelerationStructureFlags::kNone) override;
  void CopyAccelerationStructure(const ezSharedPtr<Resource>& src, const ezSharedPtr<Resource>& dst, CopyAccelerationStructureMode mode) override;
  void CopyBuffer(const ezSharedPtr<Resource>& src_buffer, const ezSharedPtr<Resource>& dst_buffer,
    const std::vector<BufferCopyRegion>& regions) override;
  void CopyBufferToTexture(const ezSharedPtr<Resource>& src_buffer, const ezSharedPtr<Resource>& dst_texture,
    const std::vector<BufferToTextureCopyRegion>& regions) override;
  void CopyTexture(const ezSharedPtr<Resource>& src_texture, const ezSharedPtr<Resource>& dst_texture,
    const std::vector<TextureCopyRegion>& regions) override;
  void UpdateSubresource(const ezSharedPtr<Resource>& resource, ezUInt32 subresource, const void* data, ezUInt32 row_pitch = 0, ezUInt32 depth_pitch = 0) override;
  ezUInt64& GetFenceValue() override;

private:
  void BufferBarrier(const ezSharedPtr<Resource>& resource, ResourceState state);
  void ViewBarrier(const ezSharedPtr<View>& view, ResourceState state);
  void ImageBarrier(const ezSharedPtr<Resource>& resource, ezUInt32 base_mip_level, ezUInt32 level_count, ezUInt32 base_array_layer, ezUInt32 layer_count, ResourceState state);
  void OnAttachSRV(const BindKey& bind_key, const ezSharedPtr<View>& view);
  void OnAttachUAV(const BindKey& bind_key, const ezSharedPtr<View>& view);
  void SetBinding(const BindKey& bind_key, const ezSharedPtr<View>& view);
  void UpdateDefaultSubresource(const ezSharedPtr<Resource>& resource, ezUInt32 subresource, const void* data, ezUInt32 row_pitch, ezUInt32 depth_pitch);
  void Apply();
  void ApplyPipeline();
  void ApplyBindingSet();
  ResourceStateTracker& GetResourceStateTracker(const ezSharedPtr<Resource>& resource);
  void CreateShaderTable(std::shared_ptr<Pipeline> pipeline);

  Device& m_device;
  ObjectCache& m_object_cache;
  std::vector<std::shared_ptr<Framebuffer>> m_framebuffers;
  ezSharedPtr<CommandList> m_CommandList;
  ezUInt32 m_viewport_width = 0;
  ezUInt32 m_viewport_height = 0;

  std::map<BindKey, ezSharedPtr<View>> m_bound_resources;
  std::map<BindKey, std::shared_ptr<DeferredView>> m_bound_deferred_view;
  std::vector<std::shared_ptr<ResourceLazyViewDesc>> m_resource_lazy_view_descs;

  std::shared_ptr<Program> m_program;
  std::vector<ezSharedPtr<BindingSet>> m_binding_sets;

  ComputePipelineDesc m_compute_pipeline_desc = {};
  RayTracingPipelineDesc m_ray_tracing_pipeline_desc = {};
  GraphicsPipelineDesc m_graphic_pipeline_desc = {};

  std::vector<ezSharedPtr<Resource>> m_cmd_resources;

  std::map<ezSharedPtr<Resource>, ResourceStateTracker> m_resource_state_tracker;
  std::vector<ResourceBarrierDesc> m_lazy_barriers;
  ezUInt64 m_fence_value = 0;
  ezSharedPtr<BindingSetLayout> m_layout;

  RayTracingShaderTables m_shader_tables = {};
  ezSharedPtr<Resource> m_shader_table;
  PipelineType m_pipeline_type = PipelineType::kGraphics;
  std::shared_ptr<Pipeline> m_pipeline;
  ezSharedPtr<View> m_shading_rate_image;
  ShadingRateCombiner m_shading_rate_combiner = ShadingRateCombiner::kPassthrough;
};
