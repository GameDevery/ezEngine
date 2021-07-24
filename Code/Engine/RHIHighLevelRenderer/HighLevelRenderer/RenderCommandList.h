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
    virtual void Attach(const BindKey& bind_key, const ezSharedPtr<Resource>& resource = {}, const LazyViewDesc& view_desc = {}) = 0;
    virtual void Attach(const BindKey& bind_key, const std::shared_ptr<DeferredView>& view) = 0;
    virtual void Attach(const BindKey& bind_key, const ezSharedPtr<View>& view) = 0;
    virtual void SetRasterizeState(const RasterizerDesc& desc) = 0;
    virtual void SetBlendState(const BlendDesc& desc) = 0;
    virtual void SetDepthStencilState(const DepthStencilDesc& desc) = 0;
    virtual void UseProgram(const ezSharedPtr<Program>& program) = 0;
    virtual void BeginRenderPass(const RenderPassBeginDesc& desc) = 0;
    virtual void EndRenderPass() = 0;
    virtual void BeginEvent(const ezString& name) = 0;
    virtual void EndEvent() = 0;
    virtual void Draw(ezUInt32 vertex_count, ezUInt32 instance_count, ezUInt32 first_vertex, ezUInt32 first_instance) = 0;
    virtual void DrawIndexed(ezUInt32 index_count, ezUInt32 instance_count, ezUInt32 first_index, int32_t vertex_offset, ezUInt32 first_instance) = 0;
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
    virtual void DispatchRays(ezUInt32 width, ezUInt32 height, ezUInt32 depth) = 0;
    virtual void SetViewport(float x, float y, float width, float height) = 0;
    virtual void SetScissorRect(int32_t left, int32_t top, ezUInt32 right, ezUInt32 bottom) = 0;
    virtual void IASetIndexBuffer(const ezSharedPtr<Resource>& resource, ezRHIResourceFormat::Enum format) = 0;
    virtual void IASetVertexBuffer(ezUInt32 slot, const ezSharedPtr<Resource>& resource) = 0;
    virtual void RSSetShadingRateImage(const ezSharedPtr<View>& view) = 0;
    virtual void BuildBottomLevelAS(const ezSharedPtr<Resource>& src, const ezSharedPtr<Resource>& dst, const std::vector<RaytracingGeometryDesc>& descs, BuildAccelerationStructureFlags flags = BuildAccelerationStructureFlags::kNone) = 0;
    virtual void BuildTopLevelAS(const ezSharedPtr<Resource>& src, const ezSharedPtr<Resource>& dst, const std::vector<std::pair<ezSharedPtr<Resource>, ezMat4>>& geometry, BuildAccelerationStructureFlags flags = BuildAccelerationStructureFlags::kNone) = 0;
    virtual void CopyAccelerationStructure(const ezSharedPtr<Resource>& src, const ezSharedPtr<Resource>& dst, CopyAccelerationStructureMode mode) = 0;
    virtual void CopyBuffer(const ezSharedPtr<Resource>& src_buffer, const ezSharedPtr<Resource>& dst_buffer,
                            const std::vector<BufferCopyRegion>& regions) = 0;
    virtual void CopyBufferToTexture(const ezSharedPtr<Resource>& src_buffer, const ezSharedPtr<Resource>& dst_texture,
                                     const std::vector<BufferToTextureCopyRegion>& regions) = 0;
    virtual void CopyTexture(const ezSharedPtr<Resource>& src_texture, const ezSharedPtr<Resource>& dst_texture,
                             const std::vector<TextureCopyRegion>& regions) = 0;
    virtual void UpdateSubresource(const ezSharedPtr<Resource>& resource, ezUInt32 subresource, const void* data, ezUInt32 row_pitch = 0, ezUInt32 depth_pitch = 0) = 0;
    virtual ezUInt64& GetFenceValue() = 0;
};
