#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/CommandList/CommandList.h>

class VKDevice;
class VKPipeline;

class VKCommandList : public CommandList
{
public:
    VKCommandList(VKDevice& device, CommandListType type);
    void Reset() override;
    void Close() override;
    void BindPipeline(const std::shared_ptr<Pipeline>& state) override;
    void BindBindingSet(const std::shared_ptr<BindingSet>& binding_set) override;
    void BeginRenderPass(const std::shared_ptr<RenderPass>& render_pass, const std::shared_ptr<Framebuffer>& framebuffer, const ClearDesc& clear_desc) override;
    void EndRenderPass() override;
    void BeginEvent(const ezString& name) override;
    void EndEvent() override;
    void Draw(ezUInt32 vertex_count, ezUInt32 instance_count, ezUInt32 first_vertex, ezUInt32 first_instance) override;
    void DrawIndexed(ezUInt32 index_count, ezUInt32 instance_count, ezUInt32 first_index, ezInt32 vertex_offset, ezUInt32 first_instance) override;
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
    void DispatchRays(const RayTracingShaderTables& shader_tables, ezUInt32 width, ezUInt32 height, ezUInt32 depth) override;
    void ResourceBarrier(const std::vector<ResourceBarrierDesc>& barriers) override;
    void UAVResourceBarrier(const ezSharedPtr<Resource>& resource) override;
    void SetViewport(float x, float y, float width, float height) override;
    void SetScissorRect(ezInt32 left, ezInt32 top, ezUInt32 right, ezUInt32 bottom) override;
    void IASetIndexBuffer(const ezSharedPtr<Resource>& resource, ezRHIResourceFormat::Enum format) override;
    void IASetVertexBuffer(ezUInt32 slot, const ezSharedPtr<Resource>& resource) override;
    void RSSetShadingRate(ShadingRate shading_rate, const std::array<ShadingRateCombiner, 2>& combiners) override;
    void BuildBottomLevelAS(
        const ezSharedPtr<Resource>& src,
        const ezSharedPtr<Resource>& dst,
        const ezSharedPtr<Resource>& scratch,
        ezUInt64 scratch_offset,
        const std::vector<RaytracingGeometryDesc>& descs,
        BuildAccelerationStructureFlags flags) override;
    void BuildTopLevelAS(
        const ezSharedPtr<Resource>& src,
        const ezSharedPtr<Resource>& dst,
        const ezSharedPtr<Resource>& scratch,
        ezUInt64 scratch_offset,
        const ezSharedPtr<Resource>& instance_data,
        ezUInt64 instance_offset,
        ezUInt32 instance_count,
        BuildAccelerationStructureFlags flags) override;
    void CopyAccelerationStructure(const ezSharedPtr<Resource>& src, const ezSharedPtr<Resource>& dst, CopyAccelerationStructureMode mode) override;
    void CopyBuffer(const ezSharedPtr<Resource>& src_buffer, const ezSharedPtr<Resource>& dst_buffer,
                    const std::vector<BufferCopyRegion>& regions) override;
    void CopyBufferToTexture(const ezSharedPtr<Resource>& src_buffer, const ezSharedPtr<Resource>& dst_texture,
                             const std::vector<BufferToTextureCopyRegion>& regions) override;
    void CopyTexture(const ezSharedPtr<Resource>& src_texture, const ezSharedPtr<Resource>& dst_texture,
                     const std::vector<TextureCopyRegion>& regions) override;
    void WriteAccelerationStructuresProperties(
        const std::vector<ezSharedPtr<Resource>>& acceleration_structures,
        const std::shared_ptr<QueryHeap>& query_heap,
        ezUInt32 first_query) override;
    void ResolveQueryData(
        const std::shared_ptr<QueryHeap>& query_heap,
        ezUInt32 first_query,
        ezUInt32 query_count,
        const ezSharedPtr<Resource>& dst_buffer,
        ezUInt64 dst_offset) override;

    vk::CommandBuffer GetCommandList();

private:
    void BuildAccelerationStructure(vk::AccelerationStructureCreateInfoKHR& build_info, const vk::Buffer& instance_data, ezUInt64 instance_offset, const ezSharedPtr<Resource>& src, const ezSharedPtr<Resource>& dst, const ezSharedPtr<Resource>& scratch, ezUInt64 scratch_offset);

    VKDevice& m_device;
    vk::UniqueCommandBuffer m_command_list;
    bool m_closed = false;
    std::shared_ptr<VKPipeline> m_state;
    std::shared_ptr<BindingSet> m_binding_set;
};
