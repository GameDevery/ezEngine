#pragma once
#include <RHIHighLevelRenderer/RHIHighLevelRendererDLL.h>
#include <RHI/Instance/BaseTypes.h>
#include <RHI/Pipeline/Pipeline.h>
#include <RHI/Device/Device.h>
#include <map>

class EZ_RHIHIGHLEVELRENDERER_DLL ObjectCache
{
public:
    ObjectCache(Device& device);

    std::shared_ptr<Pipeline> GetPipeline(const GraphicsPipelineDesc& desc);
    std::shared_ptr<Pipeline> GetPipeline(const ComputePipelineDesc& desc);
    std::shared_ptr<Pipeline> GetPipeline(const RayTracingPipelineDesc& desc);
    ezSharedPtr<RenderPass> GetRenderPass(const RenderPassDesc& desc);
    ezSharedPtr<BindingSetLayout> GetBindingSetLayout(const std::vector<BindKey>& keys);
    ezSharedPtr<BindingSet> GetBindingSet(const ezSharedPtr<BindingSetLayout>& layout, const std::vector<BindingDesc>& bindings);
    std::shared_ptr<Framebuffer> GetFramebuffer(const FramebufferDesc& desc);
    ezSharedPtr<View> GetView(const std::shared_ptr<Program>& program, const BindKey& bind_key, const ezSharedPtr<Resource>& resource, const LazyViewDesc& view_desc);

private:
    Device& m_device;
    std::map<GraphicsPipelineDesc, std::shared_ptr<Pipeline>> m_graphics_object_cache;
    std::map<ComputePipelineDesc, std::shared_ptr<Pipeline>> m_compute_object_cache;
    std::map<RayTracingPipelineDesc, std::shared_ptr<Pipeline>> m_ray_tracing_object_cache;
    std::map<RenderPassDesc, ezSharedPtr<RenderPass>> m_render_pass_cache;
    std::map<std::vector<BindKey>, ezSharedPtr<BindingSetLayout>> m_layout_cache;
    std::map<std::pair<ezSharedPtr<BindingSetLayout>, std::vector<BindingDesc>>, ezSharedPtr<BindingSet>> m_binding_set_cache;
    std::map<FramebufferDesc, std::shared_ptr<Framebuffer>> m_framebuffers;
    std::map<std::tuple<std::shared_ptr<Program>, BindKey, ezSharedPtr<Resource>, LazyViewDesc>, ezSharedPtr<View>> m_views;
};
