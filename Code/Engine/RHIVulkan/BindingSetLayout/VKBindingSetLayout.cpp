#include <RHIVulkanPCH.h>

#include <RHIVulkan/BindingSetLayout/VKBindingSetLayout.h>
#include <RHIVulkan/Device/VKDevice.h>

EZ_DEFINE_AS_POD_TYPE(vk::UniqueDescriptorSetLayout);
EZ_DEFINE_AS_POD_TYPE(vk::DescriptorSetLayout);

vk::DescriptorType GetDescriptorType(ViewType view_type)
{
    switch (view_type)
    {
    case ViewType::kConstantBuffer:
        return vk::DescriptorType::eUniformBuffer;
    case ViewType::kSampler:
        return vk::DescriptorType::eSampler;
    case ViewType::kTexture:
        return vk::DescriptorType::eSampledImage;
    case ViewType::kRWTexture:
        return vk::DescriptorType::eStorageImage;
    case ViewType::kBuffer:
        return vk::DescriptorType::eUniformTexelBuffer;
    case ViewType::kRWBuffer:
        return vk::DescriptorType::eStorageTexelBuffer;
    case ViewType::kStructuredBuffer:
        return vk::DescriptorType::eStorageBuffer;
    case ViewType::kRWStructuredBuffer:
        return vk::DescriptorType::eStorageBuffer;
    case ViewType::kAccelerationStructure:
        return vk::DescriptorType::eAccelerationStructureKHR;
    default:
        break;
    }
    assert(false);
    return {};
}

vk::ShaderStageFlagBits ShaderType2Bit(ShaderType type)
{
    switch (type)
    {
    case ShaderType::kVertex:
        return vk::ShaderStageFlagBits::eVertex;
    case ShaderType::kPixel:
        return vk::ShaderStageFlagBits::eFragment;
    case ShaderType::kGeometry:
        return vk::ShaderStageFlagBits::eGeometry;
    case ShaderType::kCompute:
        return vk::ShaderStageFlagBits::eCompute;
    case ShaderType::kAmplification:
        return vk::ShaderStageFlagBits::eTaskNV;
    case ShaderType::kMesh:
        return vk::ShaderStageFlagBits::eMeshNV;
    case ShaderType::kLibrary:
        return vk::ShaderStageFlagBits::eAll;
    }
    assert(false);
    return {};
}

VKBindingSetLayout::VKBindingSetLayout(VKDevice& device, const ezDynamicArray<BindKey>& descs)
{
    ezMap<ezUInt32, ezDynamicArray<vk::DescriptorSetLayoutBinding>> bindings_by_set;
    ezMap<ezUInt32, ezDynamicArray<vk::DescriptorBindingFlags>> bindings_flags_by_set;

    for (const auto& bind_key : descs)
    {
        decltype(auto) binding = bindings_by_set[bind_key.space].ExpandAndGetRef();
        binding.binding = bind_key.slot;
        binding.descriptorType = GetDescriptorType(bind_key.view_type);
        binding.descriptorCount = bind_key.count;
        binding.stageFlags = ShaderType2Bit(bind_key.shader_type);

        decltype(auto) binding_flag = bindings_flags_by_set[bind_key.space].ExpandAndGetRef();
        if (bind_key.count == ezMath::MaxValue<ezUInt32>())
        {
            binding.descriptorCount = max_bindless_heap_size;
            binding_flag = vk::DescriptorBindingFlagBits::eVariableDescriptorCount;
            m_bindless_type.Insert(bind_key.space, binding.descriptorType);
            binding.stageFlags = vk::ShaderStageFlagBits::eAll;
        }
    }

    for (const auto& set_desc : bindings_by_set)
    {
        vk::DescriptorSetLayoutCreateInfo layout_info = {};
      layout_info.bindingCount = set_desc.Value().GetCount();
        layout_info.pBindings = set_desc.Value().GetData();

        vk::DescriptorSetLayoutBindingFlagsCreateInfo layout_flags_info = {};
        layout_flags_info.bindingCount = bindings_flags_by_set[set_desc.Key()].GetCount();
        layout_flags_info.pBindingFlags = bindings_flags_by_set[set_desc.Key()].GetData();
        layout_info.pNext = &layout_flags_info;

        ezUInt32 set_num = set_desc.Key();
        if (m_descriptor_set_layouts.GetCount() <= set_num)
        {
            m_descriptor_set_layouts.SetCountUninitialized(set_num + 1);
            m_descriptor_count_by_set.SetCount(set_num + 1);
        }

        decltype(auto) descriptor_set_layout = m_descriptor_set_layouts[set_num];
        descriptor_set_layout = device.GetDevice().createDescriptorSetLayoutUnique(layout_info);

        decltype(auto) descriptor_count = m_descriptor_count_by_set[set_num];
        for (const auto& binding : set_desc.Value())
        {
            descriptor_count[binding.descriptorType] += binding.descriptorCount;
        }
    }

    ezDynamicArray<vk::DescriptorSetLayout> descriptor_set_layouts;
    for (auto& descriptor_set_layout : m_descriptor_set_layouts)
    {
        if (!descriptor_set_layout)
        {
            vk::DescriptorSetLayoutCreateInfo layout_info = {};
            descriptor_set_layout = device.GetDevice().createDescriptorSetLayoutUnique(layout_info);
        }

        descriptor_set_layouts.PushBack(descriptor_set_layout.get());
    }

    vk::PipelineLayoutCreateInfo pipeline_layout_info = {};
    pipeline_layout_info.setLayoutCount = descriptor_set_layouts.GetCount();
    pipeline_layout_info.pSetLayouts = descriptor_set_layouts.GetData();

    m_pipeline_layout = device.GetDevice().createPipelineLayoutUnique(pipeline_layout_info);
}

const ezMap<ezUInt32, vk::DescriptorType>& VKBindingSetLayout::GetBindlessType() const
{
    return m_bindless_type;
}

const ezDynamicArray<vk::UniqueDescriptorSetLayout>& VKBindingSetLayout::GetDescriptorSetLayouts() const
{
    return m_descriptor_set_layouts;
}

const ezDynamicArray<ezMap<vk::DescriptorType, ezUInt32>>& VKBindingSetLayout::GetDescriptorCountBySet() const
{
    return m_descriptor_count_by_set;
}

vk::PipelineLayout VKBindingSetLayout::GetPipelineLayout() const
{
    return m_pipeline_layout.get();
}
