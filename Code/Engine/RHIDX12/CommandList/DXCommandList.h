#pragma once
#include <RHI/CommandList/CommandList.h>
#include <dxgi.h>
#include <directx/d3d12.h>
#include <wrl.h>
using namespace Microsoft::WRL;

class DXDevice;
class DXResource;
class DXPipeline;

class DXCommandList : public CommandList
{
public:
    DXCommandList(DXDevice& device, CommandListType type);
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
    void DrawIndirect(const std::shared_ptr<Resource>& argument_buffer, ezUInt64 argument_buffer_offset) override;
    void DrawIndexedIndirect(const std::shared_ptr<Resource>& argument_buffer, ezUInt64 argument_buffer_offset) override;
    void DrawIndirectCount(
        const std::shared_ptr<Resource>& argument_buffer,
        ezUInt64 argument_buffer_offset,
        const std::shared_ptr<Resource>& count_buffer,
        ezUInt64 count_buffer_offset,
        ezUInt32 max_draw_count,
        ezUInt32 stride) override;
    void DrawIndexedIndirectCount(
        const std::shared_ptr<Resource>& argument_buffer,
        ezUInt64 argument_buffer_offset,
        const std::shared_ptr<Resource>& count_buffer,
        ezUInt64 count_buffer_offset,
        ezUInt32 max_draw_count,
        ezUInt32 stride) override;
    void Dispatch(ezUInt32 thread_group_count_x, ezUInt32 thread_group_count_y, ezUInt32 thread_group_count_z) override;
    void DispatchIndirect(const std::shared_ptr<Resource>& argument_buffer, ezUInt64 argument_buffer_offset) override;
    void DispatchMesh(ezUInt32 thread_group_count_x) override;
    void DispatchRays(const RayTracingShaderTables& shader_tables, ezUInt32 width, ezUInt32 height, ezUInt32 depth) override;
    void ResourceBarrier(const std::vector<ResourceBarrierDesc>& barriers) override;
    void UAVResourceBarrier(const std::shared_ptr<Resource>& resource) override;
    void SetViewport(float x, float y, float width, float height) override;
    void SetScissorRect(ezInt32 left, ezInt32 top, ezUInt32 right, ezUInt32 bottom) override;
    void IASetIndexBuffer(const std::shared_ptr<Resource>& resource, ezRHIResourceFormat::Enum format) override;
    void IASetVertexBuffer(ezUInt32 slot, const std::shared_ptr<Resource>& resource) override;
    void RSSetShadingRate(ShadingRate shading_rate, const std::array<ShadingRateCombiner, 2>& combiners) override;
    void BuildBottomLevelAS(
        const std::shared_ptr<Resource>& src,
        const std::shared_ptr<Resource>& dst,
        const std::shared_ptr<Resource>& scratch,
        ezUInt64 scratch_offset,
        const std::vector<RaytracingGeometryDesc>& descs,
        BuildAccelerationStructureFlags flags) override;
    void BuildTopLevelAS(
        const std::shared_ptr<Resource>& src,
        const std::shared_ptr<Resource>& dst,
        const std::shared_ptr<Resource>& scratch,
        ezUInt64 scratch_offset,
        const std::shared_ptr<Resource>& instance_data,
        ezUInt64 instance_offset,
        ezUInt32 instance_count,
        BuildAccelerationStructureFlags flags) override;
    void CopyAccelerationStructure(const std::shared_ptr<Resource>& src, const std::shared_ptr<Resource>& dst, CopyAccelerationStructureMode mode) override;
    void CopyBuffer(const std::shared_ptr<Resource>& src_buffer, const std::shared_ptr<Resource>& dst_buffer,
                    const std::vector<BufferCopyRegion>& regions) override;
    void CopyBufferToTexture(const std::shared_ptr<Resource>& src_buffer, const std::shared_ptr<Resource>& dst_texture,
                             const std::vector<BufferToTextureCopyRegion>& regions) override;
    void CopyTexture(const std::shared_ptr<Resource>& src_texture, const std::shared_ptr<Resource>& dst_texture,
                     const std::vector<TextureCopyRegion>& regions) override;
    void WriteAccelerationStructuresProperties(
        const std::vector<std::shared_ptr<Resource>>& acceleration_structures,
        const std::shared_ptr<QueryHeap>& query_heap,
        ezUInt32 first_query) override;
    void ResolveQueryData(
        const std::shared_ptr<QueryHeap>& query_heap,
        ezUInt32 first_query,
        ezUInt32 query_count,
        const std::shared_ptr<Resource>& dst_buffer,
        ezUInt64 dst_offset) override;

    ComPtr<ID3D12GraphicsCommandList> GetCommandList();

private:
    void ExecuteIndirect(
        D3D12_INDIRECT_ARGUMENT_TYPE type,
        const std::shared_ptr<Resource>& argument_buffer,
        ezUInt64 argument_buffer_offset,
        const std::shared_ptr<Resource>& count_buffer,
        ezUInt64 count_buffer_offset,
        ezUInt32 max_draw_count,
        ezUInt32 stride);

    void BeginRenderPassImpl(const std::shared_ptr<RenderPass>& render_pass, const std::shared_ptr<Framebuffer>& framebuffer, const ClearDesc& clear_desc);
    void OMSetFramebuffer(const std::shared_ptr<RenderPass>& render_pass, const std::shared_ptr<Framebuffer>& framebuffer, const ClearDesc& clear_desc);
    void IASetVertexBufferImpl(ezUInt32 slot, const std::shared_ptr<Resource>& resource, ezUInt32 stride);
    void BuildAccelerationStructure(D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& inputs, const std::shared_ptr<Resource>& src, const std::shared_ptr<Resource>& dst, const std::shared_ptr<Resource>& scratch, ezUInt64 scratch_offset);

    DXDevice& m_device;
    ComPtr<ID3D12CommandAllocator> m_command_allocator;
    ComPtr<ID3D12GraphicsCommandList> m_command_list;
    ComPtr<ID3D12GraphicsCommandList4> m_command_list4;
    ComPtr<ID3D12GraphicsCommandList5> m_command_list5;
    ComPtr<ID3D12GraphicsCommandList6> m_command_list6;
    bool m_closed = false;
    std::vector<ComPtr<ID3D12DescriptorHeap>> m_heaps;
    std::shared_ptr<DXPipeline> m_state;
    std::shared_ptr<BindingSet> m_binding_set;
    ezMap<ezUInt32, std::shared_ptr<Resource>> m_lazy_vertex;
    std::shared_ptr<View> m_shading_rate_image_view;
};
