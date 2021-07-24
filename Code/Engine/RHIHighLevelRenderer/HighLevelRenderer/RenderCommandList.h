#pragma once
#include <RHIHighLevelRenderer/RHIHighLevelRendererDLL.h>
#include <RHI/Instance/QueryInterface.h>
#include <RHI/Resource/Resource.h>
#include <RHI/Instance/BaseTypes.h>

class EZ_RHIHIGHLEVELRENDERER_DLL RenderCommandList : public QueryInterface
{
public:
    virtual ~RenderCommandList() = default;
    virtual void Reset() = 0;
    virtual void Close() = 0;
    virtual void Attach(const BindKey& bind_key, const std::shared_ptr<Resource>& resource = {}, const LazyViewDesc& view_desc = {}) = 0;
    virtual void Attach(const BindKey& bind_key, const std::shared_ptr<DeferredView>& view) = 0;
    virtual void Attach(const BindKey& bind_key, const std::shared_ptr<View>& view) = 0;
    virtual void SetRasterizeState(const RasterizerDesc& desc) = 0;
    virtual void SetBlendState(const BlendDesc& desc) = 0;
    virtual void SetDepthStencilState(const DepthStencilDesc& desc) = 0;
    virtual void UseProgram(const std::shared_ptr<Program>& program) = 0;
    virtual void BeginRenderPass(const RenderPassBeginDesc& desc) = 0;
    virtual void EndRenderPass() = 0;
    virtual void BeginEvent(const ezString& name) = 0;
    virtual void EndEvent() = 0;
    virtual void Draw(ezUInt32 vertex_count, ezUInt32 instance_count, ezUInt32 first_vertex, ezUInt32 first_instance) = 0;
    virtual void DrawIndexed(ezUInt32 index_count, ezUInt32 instance_count, ezUInt32 first_index, int32_t vertex_offset, ezUInt32 first_instance) = 0;
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
    virtual void DispatchRays(ezUInt32 width, ezUInt32 height, ezUInt32 depth) = 0;
    virtual void SetViewport(float x, float y, float width, float height) = 0;
    virtual void SetScissorRect(int32_t left, int32_t top, ezUInt32 right, ezUInt32 bottom) = 0;
    virtual void IASetIndexBuffer(const std::shared_ptr<Resource>& resource, ezRHIResourceFormat::Enum format) = 0;
    virtual void IASetVertexBuffer(ezUInt32 slot, const std::shared_ptr<Resource>& resource) = 0;
    virtual void RSSetShadingRateImage(const std::shared_ptr<View>& view) = 0;
    virtual void BuildBottomLevelAS(const std::shared_ptr<Resource>& src, const std::shared_ptr<Resource>& dst, const std::vector<RaytracingGeometryDesc>& descs, BuildAccelerationStructureFlags flags = BuildAccelerationStructureFlags::kNone) = 0;
    virtual void BuildTopLevelAS(const std::shared_ptr<Resource>& src, const std::shared_ptr<Resource>& dst, const std::vector<std::pair<std::shared_ptr<Resource>, ezMat4>>& geometry, BuildAccelerationStructureFlags flags = BuildAccelerationStructureFlags::kNone) = 0;
    virtual void CopyAccelerationStructure(const std::shared_ptr<Resource>& src, const std::shared_ptr<Resource>& dst, CopyAccelerationStructureMode mode) = 0;
    virtual void CopyBuffer(const std::shared_ptr<Resource>& src_buffer, const std::shared_ptr<Resource>& dst_buffer,
                            const std::vector<BufferCopyRegion>& regions) = 0;
    virtual void CopyBufferToTexture(const std::shared_ptr<Resource>& src_buffer, const std::shared_ptr<Resource>& dst_texture,
                                     const std::vector<BufferToTextureCopyRegion>& regions) = 0;
    virtual void CopyTexture(const std::shared_ptr<Resource>& src_texture, const std::shared_ptr<Resource>& dst_texture,
                             const std::vector<TextureCopyRegion>& regions) = 0;
    virtual void UpdateSubresource(const std::shared_ptr<Resource>& resource, ezUInt32 subresource, const void* data, ezUInt32 row_pitch = 0, ezUInt32 depth_pitch = 0) = 0;
    virtual ezUInt64& GetFenceValue() = 0;
};
