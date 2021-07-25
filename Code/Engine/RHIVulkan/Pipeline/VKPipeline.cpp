#include <RHIVulkanPCH.h>

#include <RHIVulkan/Pipeline/VKPipeline.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/BindingSetLayout/VKBindingSetLayout.h>


EZ_DEFINE_AS_POD_TYPE(vk::PipelineShaderStageCreateInfo);
EZ_DEFINE_AS_POD_TYPE(vk::UniqueShaderModule);

vk::ShaderStageFlagBits ExecutionModel2Bit(ShaderKind kind)
{
    switch (kind)
    {
    case ShaderKind::kVertex:
        return vk::ShaderStageFlagBits::eVertex;
    case ShaderKind::kPixel:
        return vk::ShaderStageFlagBits::eFragment;
    case ShaderKind::kCompute:
        return vk::ShaderStageFlagBits::eCompute;
    case ShaderKind::kGeometry:
        return vk::ShaderStageFlagBits::eGeometry;
    case ShaderKind::kAmplification:
        return vk::ShaderStageFlagBits::eTaskNV;
    case ShaderKind::kMesh:
        return vk::ShaderStageFlagBits::eMeshNV;
    case ShaderKind::kRayGeneration:
        return vk::ShaderStageFlagBits::eRaygenKHR;
    case ShaderKind::kIntersection:
        return vk::ShaderStageFlagBits::eIntersectionKHR;
    case ShaderKind::kAnyHit:
        return vk::ShaderStageFlagBits::eAnyHitKHR;
    case ShaderKind::kClosestHit:
        return vk::ShaderStageFlagBits::eClosestHitKHR;
    case ShaderKind::kMiss:
        return vk::ShaderStageFlagBits::eMissKHR;
    case ShaderKind::kCallable:
        return vk::ShaderStageFlagBits::eCallableKHR;
    }
    assert(false);
    return {};
}

VKPipeline::VKPipeline(VKDevice& device, const ezSharedPtr<Program>& program, const ezSharedPtr<BindingSetLayout>& layout)
    : m_device(device)
{
    decltype(auto) vk_layout = layout.Downcast<VKBindingSetLayout>();
    m_pipeline_layout = vk_layout->GetPipelineLayout();

    decltype(auto) shaders = program->GetShaders();
    for (const auto& shader : shaders)
    {
        decltype(auto) blob = shader->GetBlob();
        vk::ShaderModuleCreateInfo shader_module_info = {};
        shader_module_info.codeSize = blob.GetCount();
        shader_module_info.pCode = (ezUInt32*)blob.GetData();
        m_shader_modules.PushBack(m_device.GetDevice().createShaderModuleUnique(shader_module_info));

        decltype(auto) reflection = shader->GetReflection();
        decltype(auto) entry_points = reflection->GetEntryPoints();
        for (const auto& entry_point : entry_points)
        {
          m_shader_ids[shader->GetId(entry_point.name)] = m_shader_stage_create_info.GetCount();
            decltype(auto) shader_stage_create_info = m_shader_stage_create_info.ExpandAndGetRef();
            shader_stage_create_info.stage = ExecutionModel2Bit(entry_point.kind);
            shader_stage_create_info.module = m_shader_modules.PeekBack().get();
            decltype(auto) name = entry_point_names.emplace_back(entry_point.name);
            shader_stage_create_info.pName = name.GetData();
        }
    }
}

vk::Pipeline VKPipeline::GetPipeline() const
{
    return m_pipeline.get();
}

vk::PipelineLayout VKPipeline::GetPipelineLayout() const
{
    return m_pipeline_layout;
}

ezDynamicArray<ezUInt8> VKPipeline::GetRayTracingShaderGroupHandles(ezUInt32 first_group, ezUInt32 group_count) const
{
    return {};
}
