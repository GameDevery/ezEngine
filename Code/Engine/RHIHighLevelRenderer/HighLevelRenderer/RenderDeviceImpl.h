#pragma once
#include <RHIHighLevelRenderer/RHIHighLevelRendererDLL.h>
#include <RHIHighLevelRenderer/HighLevelRenderer/RenderDevice.h>
#include <RHI/Instance/BaseTypes.h>
#include <RHI/Instance/Instance.h>
#include <memory>

#include <memory>
#include <array>
#include <set>
#include <deque>

#include <RHI/Resource/Resource.h>

#include <RHIHighLevelRenderer/HighLevelRenderer/RenderCommandList.h>
#include <RHIHighLevelRenderer/HighLevelRenderer/RenderDevice.h>
#include <RHIHighLevelRenderer/HighLevelRenderer/ObjectCache.h>

class EZ_RHIHIGHLEVELRENDERER_DLL RenderDeviceImpl : public RenderDevice
{
public:
    RenderDeviceImpl(const RenderDeviceDesc& settings, ezWindowBase* window);
    ~RenderDeviceImpl();

    std::shared_ptr<RenderCommandList> CreateRenderCommandList(CommandListType type) override;
    ezSharedPtr<Resource> CreateTexture(ezUInt32 bind_flag, ezRHIResourceFormat::Enum format, ezUInt32 sample_count, int width, int height, int depth, int mip_levels) override;
    ezSharedPtr<Resource> CreateBuffer(ezUInt32 bind_flag, ezUInt32 buffer_size, MemoryType memory_type) override;
    ezSharedPtr<Resource> CreateSampler(const SamplerDesc& desc) override;
    ezSharedPtr<Resource> CreateBottomLevelAS(const std::vector<RaytracingGeometryDesc>& descs, BuildAccelerationStructureFlags flags) override;
    ezSharedPtr<Resource> CreateTopLevelAS(ezUInt32 instance_count, BuildAccelerationStructureFlags flags) override;
    ezSharedPtr<View> CreateView(const ezSharedPtr<Resource>& resource, const ViewDesc& view_desc) override;
    std::shared_ptr<Shader> CreateShader(const ShaderDesc& desc, ezDynamicArray<ezUInt8> byteCode, std::shared_ptr<ShaderReflection> reflection) override;
    std::shared_ptr<Program> CreateProgram(const std::vector<std::shared_ptr<Shader>>& shaders) override;
    bool IsDxrSupported() const override;
    bool IsRayQuerySupported() const override;
    bool IsVariableRateShadingSupported() const override;
    bool IsMeshShadingSupported() const override;
    ezUInt32 GetShadingRateImageTileSize() const override;
    ezUInt32 GetFrameIndex() const override;
    ezRHIResourceFormat::Enum GetFormat() const override;
    ezSharedPtr<Resource> GetBackBuffer(ezUInt32 buffer) override;
    const ezString& GetGpuName() const override;
    void ExecuteCommandLists(const std::vector<std::shared_ptr<RenderCommandList>>& command_lists) override;
    void Present() override;
    void Wait(ezUInt64 fence_value) override;
    void WaitForIdle() override;
    void Resize(ezUInt32 width, ezUInt32 height) override;

    void ExecuteCommandListsImpl(const std::vector<std::shared_ptr<RenderCommandList>>& command_lists);

private:
    void InsertPresentBarrier();

    ezWindowBase* m_window;
    bool m_vsync;
    ezUInt32 m_FrameCount;
    ezUInt32 m_frame_index = 0;
    int m_width = 0;
    int m_height = 0;
    std::shared_ptr<Instance> m_instance;
    std::shared_ptr<Adapter> m_adapter;
    std::shared_ptr<Device> m_device;
    ezSharedPtr<CommandQueue> m_command_queue;
    ezSharedPtr<Swapchain> m_Swapchain;
    std::vector<ezSharedPtr<CommandList>> m_BarrierCommandLists;
    std::vector<ezUInt64> m_frame_fence_values;
    ezUInt64 m_FenceValue = 0;
    ezSharedPtr<Fence> m_pFence;
    std::vector<ezSharedPtr<CommandList>> m_CommandListPool;
    std::deque<std::pair<ezUInt64 /*fence_value*/, size_t /*offset*/>> m_fence_value_by_cmd;
    std::unique_ptr<ObjectCache> m_object_cache;
};
