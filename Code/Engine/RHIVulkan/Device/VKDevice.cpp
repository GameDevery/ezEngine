#include <RHIVulkanPCH.h>

#include <RHI/Shader/ShaderBase.h>
#include <RHIVulkan/Adapter/VKAdapter.h>
#include <RHIVulkan/BindingSet/VKBindingSet.h>
#include <RHIVulkan/BindingSetLayout/VKBindingSetLayout.h>
#include <RHIVulkan/CommandList/VKCommandList.h>
#include <RHIVulkan/CommandQueue/VKCommandQueue.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Fence/VKTimelineSemaphore.h>
#include <RHIVulkan/Framebuffer/VKFramebuffer.h>
#include <RHIVulkan/Instance/VKInstance.h>
#include <RHIVulkan/Memory/VKMemory.h>
#include <RHIVulkan/Pipeline/VKComputePipeline.h>
#include <RHIVulkan/Pipeline/VKGraphicsPipeline.h>
#include <RHIVulkan/Pipeline/VKRayTracingPipeline.h>
#include <RHIVulkan/Program/VKProgram.h>
#include <RHIVulkan/QueryHeap/VKQueryHeap.h>
#include <RHIVulkan/RenderPass/VKRenderPass.h>
#include <RHIVulkan/Swapchain/VKSwapchain.h>
#include <RHIVulkan/Utilities/VKUtility.h>
#include <RHIVulkan/View/VKView.h>

EZ_DEFINE_AS_POD_TYPE(vk::QueueFamilyProperties);
EZ_DEFINE_AS_POD_TYPE(vk::ExtensionProperties);
EZ_DEFINE_AS_POD_TYPE(vk::PhysicalDeviceFragmentShadingRateKHR);

static vk::IndexType GetVkIndexType(ezRHIResourceFormat::Enum format)
{
  vk::Format vk_format = VKUtils::ToVkFormat(format);
  switch (vk_format)
  {
    case vk::Format::eR16Uint:
      return vk::IndexType::eUint16;
    case vk::Format::eR32Uint:
      return vk::IndexType::eUint32;
    default:
      assert(false);
      return {};
  }
}

vk::ImageLayout ConvertState(ResourceState state)
{
  static std::pair<ResourceState, vk::ImageLayout> mapping[] = {
    {ResourceState::kCommon, vk::ImageLayout::eGeneral},
    {ResourceState::kRenderTarget, vk::ImageLayout::eColorAttachmentOptimal},
    {ResourceState::kUnorderedAccess, vk::ImageLayout::eGeneral},
    {ResourceState::kDepthStencilWrite, vk::ImageLayout::eDepthStencilAttachmentOptimal},
    {ResourceState::kDepthStencilRead, vk::ImageLayout::eDepthStencilReadOnlyOptimal},
    {ResourceState::kNonPixelShaderResource, vk::ImageLayout::eShaderReadOnlyOptimal},
    {ResourceState::kPixelShaderResource, vk::ImageLayout::eShaderReadOnlyOptimal},
    {ResourceState::kCopyDest, vk::ImageLayout::eTransferDstOptimal},
    {ResourceState::kCopySource, vk::ImageLayout::eTransferSrcOptimal},
    {ResourceState::kShadingRateSource, vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR},
    {ResourceState::kPresent, vk::ImageLayout::ePresentSrcKHR},
    {ResourceState::kUndefined, vk::ImageLayout::eUndefined},
  };
  for (const auto& m : mapping)
  {
    if (state & m.first)
    {
      assert(state == m.first);
      return m.second;
    }
  }
  assert(false);
  return vk::ImageLayout::eGeneral;
}

VKDevice::VKDevice(VKAdapter& adapter)
  : m_Adapter(adapter)
  , m_PhysicalDevice(adapter.GetPhysicalDevice())
  , m_GPUDescriptorPool(*this)
{
  ezUInt32 queueFamiliesCount = 0;
  m_PhysicalDevice.getQueueFamilyProperties(&queueFamiliesCount, nullptr);

  ezDynamicArray<vk::QueueFamilyProperties> queueFamilies;
  queueFamilies.SetCountUninitialized(queueFamiliesCount);
  m_PhysicalDevice.getQueueFamilyProperties(&queueFamiliesCount, queueFamilies.GetData());

  auto hasAllBits = [](auto flags, auto bits) {
    return (flags & bits) == bits;
  };
  auto hasAnyBits = [](auto flags, auto bits) {
    return flags & bits;
  };
  for (ezUInt32 i = 0; i < queueFamilies.GetCount(); ++i)
  {
    const auto& queue = queueFamilies[i];
    if (queue.queueCount > 0 && hasAllBits(queue.queueFlags, vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute | vk::QueueFlagBits::eTransfer))
    {
      m_QueuesInfo[CommandListType::kGraphics].QueueFamilyIndex = i;
      m_QueuesInfo[CommandListType::kGraphics].QueueCount = queue.queueCount;
    }
    else if (queue.queueCount > 0 && hasAllBits(queue.queueFlags, vk::QueueFlagBits::eCompute | vk::QueueFlagBits::eTransfer) && !hasAnyBits(queue.queueFlags, vk::QueueFlagBits::eGraphics))
    {
      m_QueuesInfo[CommandListType::kCompute].QueueFamilyIndex = i;
      m_QueuesInfo[CommandListType::kCompute].QueueCount = queue.queueCount;
    }
    else if (queue.queueCount > 0 && hasAllBits(queue.queueFlags, vk::QueueFlagBits::eTransfer) && !hasAnyBits(queue.queueFlags, vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute))
    {
      m_QueuesInfo[CommandListType::kCopy].QueueFamilyIndex = i;
      m_QueuesInfo[CommandListType::kCopy].QueueCount = queue.queueCount;
    }
  }

  ezUInt32 extensionsCount = 0;
  vk::Result result = m_PhysicalDevice.enumerateDeviceExtensionProperties(nullptr, &extensionsCount, nullptr);

  ezDynamicArray<vk::ExtensionProperties> extensions;
  extensions.SetCountUninitialized(extensionsCount);
  result = m_PhysicalDevice.enumerateDeviceExtensionProperties(nullptr, &extensionsCount, extensions.GetData());

  ezSet<ezString> requiredExtensions;

  requiredExtensions.Insert(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
  requiredExtensions.Insert(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
  requiredExtensions.Insert(VK_KHR_RAY_QUERY_EXTENSION_NAME);
  requiredExtensions.Insert(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
  requiredExtensions.Insert(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
  requiredExtensions.Insert(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
  requiredExtensions.Insert(VK_KHR_MAINTENANCE3_EXTENSION_NAME);
  requiredExtensions.Insert(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
  requiredExtensions.Insert(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
  requiredExtensions.Insert(VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME);
  requiredExtensions.Insert(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);
  requiredExtensions.Insert(VK_KHR_MAINTENANCE1_EXTENSION_NAME);
  requiredExtensions.Insert(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
  requiredExtensions.Insert(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
  requiredExtensions.Insert(VK_NV_MESH_SHADER_EXTENSION_NAME);
  requiredExtensions.Insert(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME);

  ezDynamicArray<const char*> foundExtensions;
  for (const auto& extension : extensions)
  {
    if (requiredExtensions.Contains(extension.extensionName.data()))
      foundExtensions.PushBack(extension.extensionName);

    if (ezStringUtils::IsEqual(extension.extensionName.data(), VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME))
      m_is_variable_rate_shading_supported = true;
    if (ezStringUtils::IsEqual(extension.extensionName.data(), VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME))
      m_is_dxr_supported = true;
    if (ezStringUtils::IsEqual(extension.extensionName.data(), VK_NV_MESH_SHADER_EXTENSION_NAME))
      m_is_mesh_shading_supported = true;
    if (ezStringUtils::IsEqual(extension.extensionName.data(), VK_KHR_RAY_QUERY_EXTENSION_NAME))
      m_is_ray_query_supported = true;
  }

  void* deviceCreateInfoNext = nullptr;
  auto addExtension = [&](auto& extension) {
    extension.pNext = deviceCreateInfoNext;
    deviceCreateInfoNext = &extension;
  };

  if (m_is_variable_rate_shading_supported)
  {
    ezMap<ShadingRate, vk::Extent2D> shadingRatePalette;
    shadingRatePalette.Insert(ShadingRate::k1x1, vk::Extent2D{1, 1});
    shadingRatePalette.Insert(ShadingRate::k1x2, vk::Extent2D{1, 2});
    shadingRatePalette.Insert(ShadingRate::k2x1, vk::Extent2D{2, 1});
    shadingRatePalette.Insert(ShadingRate::k2x2, vk::Extent2D{2, 2});
    shadingRatePalette.Insert(ShadingRate::k2x4, vk::Extent2D{2, 4});
    shadingRatePalette.Insert(ShadingRate::k4x2, vk::Extent2D{4, 2});
    shadingRatePalette.Insert(ShadingRate::k4x4, vk::Extent2D{4, 4});

    ezUInt32 fragmentShadingRatesCount = 0;
    result = m_Adapter.GetPhysicalDevice().getFragmentShadingRatesKHR(&fragmentShadingRatesCount, nullptr);

    ezDynamicArray<vk::PhysicalDeviceFragmentShadingRateKHR> fragmentShadingRates;
    fragmentShadingRates.SetCountUninitialized(fragmentShadingRatesCount);
    result = m_Adapter.GetPhysicalDevice().getFragmentShadingRatesKHR(&fragmentShadingRatesCount, fragmentShadingRates.GetData());

    for (const auto& fragmentShadingRate : fragmentShadingRates)
    {
      vk::Extent2D size = fragmentShadingRate.fragmentSize;
      ezUInt8 shadingRate = ((size.width >> 1) << 2) | (size.height >> 1);
      EZ_ASSERT_ALWAYS((1 << ((shadingRate >> 2) & 3)) == size.width, "");
      EZ_ASSERT_ALWAYS((1 << (shadingRate & 3)) == size.height, "");
      EZ_ASSERT_ALWAYS(shadingRatePalette[(ShadingRate)shadingRate] == size, "");
      shadingRatePalette.Remove((ShadingRate)shadingRate);
    }
    EZ_ASSERT_ALWAYS(shadingRatePalette.IsEmpty(), "");

    vk::PhysicalDeviceFragmentShadingRatePropertiesKHR shadingRateImageProperties = {};
    vk::PhysicalDeviceProperties2 deviceProps2 = {};
    deviceProps2.pNext = &shadingRateImageProperties;
    m_Adapter.GetPhysicalDevice().getProperties2(&deviceProps2);
    EZ_ASSERT_ALWAYS(shadingRateImageProperties.minFragmentShadingRateAttachmentTexelSize == shadingRateImageProperties.maxFragmentShadingRateAttachmentTexelSize, "");
    EZ_ASSERT_ALWAYS(shadingRateImageProperties.minFragmentShadingRateAttachmentTexelSize.width == shadingRateImageProperties.minFragmentShadingRateAttachmentTexelSize.height, "");
    EZ_ASSERT_ALWAYS(shadingRateImageProperties.maxFragmentShadingRateAttachmentTexelSize.width == shadingRateImageProperties.maxFragmentShadingRateAttachmentTexelSize.height, "");
    m_shading_rate_image_tile_size = shadingRateImageProperties.maxFragmentShadingRateAttachmentTexelSize.width;

    vk::PhysicalDeviceFragmentShadingRateFeaturesKHR fragmentShadingRateFeatures = {};
    fragmentShadingRateFeatures.attachmentFragmentShadingRate = true;
    addExtension(fragmentShadingRateFeatures);
  }

  if (m_is_dxr_supported)
  {
    vk::PhysicalDeviceRayTracingPipelinePropertiesKHR rayTracingProperties = {};
    vk::PhysicalDeviceProperties2 deviceProps2 = {};
    deviceProps2.pNext = &rayTracingProperties;
    m_PhysicalDevice.getProperties2(&deviceProps2);
    m_shader_group_handle_size = rayTracingProperties.shaderGroupHandleSize;
    m_shader_record_alignment = rayTracingProperties.shaderGroupHandleSize;
    m_shader_table_alignment = rayTracingProperties.shaderGroupBaseAlignment;
  }

  const float queuePriority = 1.0f;
  ezDynamicArray<vk::DeviceQueueCreateInfo> queuesCreateInfo;
  for (const auto& queueInfo : m_QueuesInfo)
  {
    vk::DeviceQueueCreateInfo& queueCreateInfo = queuesCreateInfo.ExpandAndGetRef();
    queueCreateInfo.queueFamilyIndex = queueInfo.Value().QueueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
  }

  vk::PhysicalDeviceFeatures deviceFeatures = {};
  deviceFeatures.textureCompressionBC = true;
  deviceFeatures.vertexPipelineStoresAndAtomics = true;
  deviceFeatures.samplerAnisotropy = true;
  deviceFeatures.fragmentStoresAndAtomics = true;
  deviceFeatures.sampleRateShading = true;
  deviceFeatures.geometryShader = true;
  deviceFeatures.imageCubeArray = true;
  deviceFeatures.shaderImageGatherExtended = true;

  vk::PhysicalDeviceVulkan12Features deviceVulkan12Features = {};
  deviceVulkan12Features.drawIndirectCount = true;
  deviceVulkan12Features.bufferDeviceAddress = true;
  deviceVulkan12Features.timelineSemaphore = true;
  deviceVulkan12Features.runtimeDescriptorArray = true;
  deviceVulkan12Features.descriptorBindingVariableDescriptorCount = true;
  addExtension(deviceVulkan12Features);

  vk::PhysicalDeviceMeshShaderFeaturesNV meshShaderFeature = {};
  meshShaderFeature.taskShader = true;
  meshShaderFeature.meshShader = true;
  if (m_is_mesh_shading_supported)
  {
    addExtension(meshShaderFeature);
  }

  vk::PhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipelineFeature = {};
  rayTracingPipelineFeature.rayTracingPipeline = true;

  vk::PhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeature = {};
  accelerationStructureFeature.accelerationStructure = true;

  vk::PhysicalDeviceRayQueryFeaturesKHR rayQueryPipelineFeature = {};
  rayQueryPipelineFeature.rayQuery = true;

  if (m_is_dxr_supported)
  {
    addExtension(rayTracingPipelineFeature);
    addExtension(accelerationStructureFeature);

    if (m_is_ray_query_supported)
    {
      rayTracingPipelineFeature.rayTraversalPrimitiveCulling = true;
      addExtension(rayQueryPipelineFeature);
    }
  }

  vk::DeviceCreateInfo deviceCreateInfo = {};
  deviceCreateInfo.pNext = deviceCreateInfoNext;
  deviceCreateInfo.queueCreateInfoCount = queuesCreateInfo.GetCount();
  deviceCreateInfo.pQueueCreateInfos = queuesCreateInfo.GetData();
  deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
  deviceCreateInfo.enabledExtensionCount = foundExtensions.GetCount();
  deviceCreateInfo.ppEnabledExtensionNames = foundExtensions.GetData();

  m_Device = m_PhysicalDevice.createDeviceUnique(deviceCreateInfo);
  VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Device.get());

  for (auto& queueInfo : m_QueuesInfo)
  {
    vk::CommandPoolCreateInfo cmdPoolCreateInfo = {};
    cmdPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    cmdPoolCreateInfo.queueFamilyIndex = queueInfo.Value().QueueFamilyIndex;
    m_CmdPools.Insert(queueInfo.Key(), m_Device->createCommandPoolUnique(cmdPoolCreateInfo));
    m_CommandQueues[queueInfo.Key()] = EZ_DEFAULT_NEW(VKCommandQueue, *this, queueInfo.Key(), queueInfo.Value().QueueFamilyIndex);
  }
}

ezSharedPtr<Memory> VKDevice::AllocateMemory(ezUInt64 size, MemoryType memoryType, ezUInt32 memoryTypeBits)
{
  return EZ_DEFAULT_NEW(VKMemory, *this, size, memoryType, memoryTypeBits, nullptr);
}

ezSharedPtr<CommandQueue> VKDevice::GetCommandQueue(CommandListType type)
{
  return m_CommandQueues[GetAvailableCommandListType(type)];
}

ezSharedPtr<Swapchain> VKDevice::CreateSwapchain(Window window, ezUInt32 width, ezUInt32 height, ezUInt32 frameCount, bool vsync)
{
  return EZ_DEFAULT_NEW(VKSwapchain, *m_CommandQueues[CommandListType::kGraphics], window, width, height, frameCount, vsync);
}

ezSharedPtr<CommandList> VKDevice::CreateCommandList(CommandListType type)
{
  return EZ_DEFAULT_NEW(VKCommandList, *this, type);
}

ezSharedPtr<Fence> VKDevice::CreateFence(ezUInt64 initialValue)
{
  return EZ_DEFAULT_NEW(VKTimelineSemaphore, *this, initialValue);
}

ezSharedPtr<Resource> VKDevice::CreateTexture(TextureType type, ezUInt32 bindFlags, ezRHIResourceFormat::Enum format, ezUInt32 sample_count, int width, int height, int depth, int mipLevels)
{
  ezSharedPtr<VKResource> res = EZ_DEFAULT_NEW(VKResource, *this);
  res->format = format;
  res->resource_type = ResourceType::kTexture;
  res->image.size.height = height;
  res->image.size.width = width;
  res->image.format = VKUtils::ToVkFormat(format);
  res->image.level_count = mipLevels;
  res->image.sample_count = sample_count;
  res->image.array_layers = depth;

  vk::ImageUsageFlags usage = {};
  if (bindFlags & BindFlag::kDepthStencil)
    usage |= vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferDst;
  if (bindFlags & BindFlag::kShaderResource)
    usage |= vk::ImageUsageFlagBits::eSampled;
  if (bindFlags & BindFlag::kRenderTarget)
    usage |= vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
  if (bindFlags & BindFlag::kUnorderedAccess)
    usage |= vk::ImageUsageFlagBits::eStorage;
  if (bindFlags & BindFlag::kCopyDest)
    usage |= vk::ImageUsageFlagBits::eTransferDst;
  if (bindFlags & BindFlag::kCopySource)
    usage |= vk::ImageUsageFlagBits::eTransferSrc;
  if (bindFlags & BindFlag::kShadingRateSource)
    usage |= vk::ImageUsageFlagBits::eFragmentShadingRateAttachmentKHR;

  vk::ImageCreateInfo image_info = {};
  switch (type)
  {
    case TextureType::k1D:
      image_info.imageType = vk::ImageType::e1D;
      break;
    case TextureType::k2D:
      image_info.imageType = vk::ImageType::e2D;
      break;
    case TextureType::k3D:
      image_info.imageType = vk::ImageType::e3D;
      break;
  }
  image_info.extent.width = width;
  image_info.extent.height = height;
  if (type == TextureType::k3D)
    image_info.extent.depth = depth;
  else
    image_info.extent.depth = 1;
  image_info.mipLevels = mipLevels;
  if (type == TextureType::k3D)
    image_info.arrayLayers = 1;
  else
    image_info.arrayLayers = depth;
  image_info.format = res->image.format;
  image_info.tiling = vk::ImageTiling::eOptimal;
  image_info.initialLayout = vk::ImageLayout::eUndefined;
  image_info.usage = usage;
  image_info.samples = static_cast<vk::SampleCountFlagBits>(sample_count);
  image_info.sharingMode = vk::SharingMode::eExclusive;

  if (image_info.arrayLayers % 6 == 0)
    image_info.flags = vk::ImageCreateFlagBits::eCubeCompatible;

  res->image.res_owner = m_Device->createImageUnique(image_info);
  res->image.res = res->image.res_owner.get();

  res->SetInitialState(ResourceState::kUndefined);

  return res;
}

ezSharedPtr<Resource> VKDevice::CreateBuffer(ezUInt32 bind_flag, ezUInt32 buffer_size)
{
  if (buffer_size == 0)
    return {};

  ezSharedPtr<VKResource> res = EZ_DEFAULT_NEW(VKResource, *this);
  res->resource_type = ResourceType::kBuffer;
  res->buffer.size = buffer_size;

  vk::BufferCreateInfo buffer_info = {};
  buffer_info.size = buffer_size;
  buffer_info.usage = vk::BufferUsageFlagBits::eShaderDeviceAddress;

  if (bind_flag & BindFlag::kVertexBuffer)
    buffer_info.usage |= vk::BufferUsageFlagBits::eVertexBuffer;
  if (bind_flag & BindFlag::kIndexBuffer)
    buffer_info.usage |= vk::BufferUsageFlagBits::eIndexBuffer;
  if (bind_flag & BindFlag::kConstantBuffer)
    buffer_info.usage |= vk::BufferUsageFlagBits::eUniformBuffer;
  if (bind_flag & BindFlag::kUnorderedAccess)
  {
    buffer_info.usage |= vk::BufferUsageFlagBits::eStorageBuffer;
    buffer_info.usage |= vk::BufferUsageFlagBits::eStorageTexelBuffer;
  }
  if (bind_flag & BindFlag::kShaderResource)
  {
    buffer_info.usage |= vk::BufferUsageFlagBits::eStorageBuffer;
    buffer_info.usage |= vk::BufferUsageFlagBits::eUniformTexelBuffer;
  }
  if (bind_flag & BindFlag::kAccelerationStructure)
    buffer_info.usage |= vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR;
  if (bind_flag & BindFlag::kCopySource)
    buffer_info.usage |= vk::BufferUsageFlagBits::eTransferSrc;
  if (bind_flag & BindFlag::kCopyDest)
    buffer_info.usage |= vk::BufferUsageFlagBits::eTransferDst;
  if (bind_flag & BindFlag::kShaderTable)
    buffer_info.usage |= vk::BufferUsageFlagBits::eShaderBindingTableKHR;
  if (bind_flag & BindFlag::kIndirectBuffer)
    buffer_info.usage |= vk::BufferUsageFlagBits::eIndirectBuffer;

  res->buffer.res = m_Device->createBufferUnique(buffer_info);
  res->SetInitialState(ResourceState::kCommon);

  return res;
}

ezSharedPtr<Resource> VKDevice::CreateSampler(const SamplerDesc& desc)
{
  ezSharedPtr<VKResource> res = EZ_DEFAULT_NEW(VKResource, *this);

  vk::SamplerCreateInfo samplerInfo = {};
  samplerInfo.magFilter = vk::Filter::eLinear;
  samplerInfo.minFilter = vk::Filter::eLinear;
  samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
  samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
  samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
  samplerInfo.anisotropyEnable = true;
  samplerInfo.maxAnisotropy = 16;
  samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = vk::CompareOp::eAlways;
  samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = ezMath::MaxValue<float>();

  /*switch (desc.filter)
    {
    case SamplerFilter::kAnisotropic:
        sampler_desc.Filter = D3D12_FILTER_ANISOTROPIC;
        break;
    case SamplerFilter::kMinMagMipLinear:
        sampler_desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        break;
    case SamplerFilter::kComparisonMinMagMipLinear:
        sampler_desc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        break;
    }*/

  switch (desc.mode)
  {
    case SamplerTextureAddressMode::kWrap:
      samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
      samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
      samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
      break;
    case SamplerTextureAddressMode::kClamp:
      samplerInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
      samplerInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
      samplerInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
      break;
  }

  switch (desc.func)
  {
    case SamplerComparisonFunc::kNever:
      samplerInfo.compareOp = vk::CompareOp::eNever;
      break;
    case SamplerComparisonFunc::kAlways:
      samplerInfo.compareEnable = true;
      samplerInfo.compareOp = vk::CompareOp::eAlways;
      break;
    case SamplerComparisonFunc::kLess:
      samplerInfo.compareEnable = true;
      samplerInfo.compareOp = vk::CompareOp::eLess;
      break;
  }

  res->sampler.res = m_Device->createSamplerUnique(samplerInfo);

  res->resource_type = ResourceType::kSampler;
  return res;
}

ezSharedPtr<View> VKDevice::CreateView(const ezSharedPtr<Resource>& resource, const ViewDesc& view_desc)
{
  return EZ_DEFAULT_NEW(VKView, *this, resource.Downcast<VKResource>(), view_desc);
}

ezSharedPtr<BindingSetLayout> VKDevice::CreateBindingSetLayout(const std::vector<BindKey>& descs)
{
  return EZ_DEFAULT_NEW(VKBindingSetLayout, *this, descs);
}

ezSharedPtr<BindingSet> VKDevice::CreateBindingSet(const ezSharedPtr<BindingSetLayout>& layout)
{
  return EZ_DEFAULT_NEW(VKBindingSet, *this, layout.Downcast<VKBindingSetLayout>());
}

ezSharedPtr<RenderPass> VKDevice::CreateRenderPass(const RenderPassDesc& desc)
{
  return EZ_DEFAULT_NEW(VKRenderPass, *this, desc);
}

ezSharedPtr<Framebuffer> VKDevice::CreateFramebuffer(const FramebufferDesc& desc)
{
  return EZ_DEFAULT_NEW(VKFramebuffer, *this, desc);
}

ezSharedPtr<Shader> VKDevice::CreateShader(const ShaderDesc& desc, ezDynamicArray<ezUInt8> byteCode, ezSharedPtr<ShaderReflection> reflection)
{
  return EZ_DEFAULT_NEW(ShaderBase, desc, byteCode, reflection, ShaderBlobType::kSPIRV);
}

ezSharedPtr<Program> VKDevice::CreateProgram(const std::vector<ezSharedPtr<Shader>>& shaders)
{
  return EZ_DEFAULT_NEW(VKProgram, *this, shaders);
}

ezSharedPtr<Pipeline> VKDevice::CreateGraphicsPipeline(const GraphicsPipelineDesc& desc)
{
  return EZ_DEFAULT_NEW(VKGraphicsPipeline, *this, desc);
}

ezSharedPtr<Pipeline> VKDevice::CreateComputePipeline(const ComputePipelineDesc& desc)
{
  return EZ_DEFAULT_NEW(VKComputePipeline, *this, desc);
}

ezSharedPtr<Pipeline> VKDevice::CreateRayTracingPipeline(const RayTracingPipelineDesc& desc)
{
  return EZ_DEFAULT_NEW(VKRayTracingPipeline, *this, desc);
}

vk::AccelerationStructureGeometryKHR VKDevice::FillRaytracingGeometryTriangles(const BufferDesc& vertex, const BufferDesc& index, RaytracingGeometryFlags flags) const
{
  vk::AccelerationStructureGeometryKHR geometry_desc = {};
  geometry_desc.geometryType = vk::GeometryTypeNV::eTriangles;
  switch (flags)
  {
    case RaytracingGeometryFlags::kOpaque:
      geometry_desc.flags = vk::GeometryFlagBitsKHR::eOpaque;
      break;
    case RaytracingGeometryFlags::kNoDuplicateAnyHitInvocation:
      geometry_desc.flags = vk::GeometryFlagBitsKHR::eNoDuplicateAnyHitInvocation;
      break;
  }

  auto vk_vertex_res = vertex.res.Downcast<VKResource>();
  auto vk_index_res = index.res.Downcast<VKResource>();

  auto vertex_stride = ezRHIResourceFormat::GetFormatStride(vertex.format);
  geometry_desc.geometry.triangles.vertexData = m_Device->getBufferAddress({vk_vertex_res->buffer.res.get()}) + vertex.offset * vertex_stride;
  geometry_desc.geometry.triangles.vertexStride = vertex_stride;
  geometry_desc.geometry.triangles.vertexFormat = VKUtils::ToVkFormat(vertex.format);
  geometry_desc.geometry.triangles.maxVertex = vertex.count;
  if (vk_index_res)
  {
    auto index_stride = ezRHIResourceFormat::GetFormatStride(index.format);
    geometry_desc.geometry.triangles.indexData = m_Device->getBufferAddress({vk_index_res->buffer.res.get()}) + index.offset * index_stride;
    geometry_desc.geometry.triangles.indexType = GetVkIndexType(index.format);
  }
  else
  {
    geometry_desc.geometry.triangles.indexType = vk::IndexType::eNoneNV;
  }

  return geometry_desc;
}

RaytracingASPrebuildInfo VKDevice::GetAccelerationStructurePrebuildInfo(const vk::AccelerationStructureBuildGeometryInfoKHR& acceleration_structure_info, const std::vector<ezUInt32>& max_primitive_counts) const
{
  vk::AccelerationStructureBuildSizesInfoKHR size_info = {};
  m_Device->getAccelerationStructureBuildSizesKHR(vk::AccelerationStructureBuildTypeKHR::eDevice, &acceleration_structure_info, max_primitive_counts.data(), &size_info);
  RaytracingASPrebuildInfo prebuild_info = {};
  prebuild_info.acceleration_structure_size = size_info.accelerationStructureSize;
  prebuild_info.build_scratch_data_size = size_info.buildScratchSize;
  prebuild_info.update_scratch_data_size = size_info.updateScratchSize;
  return prebuild_info;
}

vk::BuildAccelerationStructureFlagsKHR Convert(BuildAccelerationStructureFlags flags)
{
  vk::BuildAccelerationStructureFlagsKHR vk_flags = {};
  if (flags & BuildAccelerationStructureFlags::kAllowUpdate)
    vk_flags |= vk::BuildAccelerationStructureFlagBitsKHR::eAllowUpdate;
  if (flags & BuildAccelerationStructureFlags::kAllowCompaction)
    vk_flags |= vk::BuildAccelerationStructureFlagBitsKHR::eAllowCompaction;
  if (flags & BuildAccelerationStructureFlags::kPreferFastTrace)
    vk_flags |= vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace;
  if (flags & BuildAccelerationStructureFlags::kPreferFastBuild)
    vk_flags |= vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastBuild;
  if (flags & BuildAccelerationStructureFlags::kMinimizeMemory)
    vk_flags |= vk::BuildAccelerationStructureFlagBitsKHR::eLowMemory;
  return vk_flags;
}

vk::AccelerationStructureTypeKHR Convert(AccelerationStructureType type)
{
  switch (type)
  {
    case AccelerationStructureType::kTopLevel:
      return vk::AccelerationStructureTypeKHR::eTopLevel;
    case AccelerationStructureType::kBottomLevel:
      return vk::AccelerationStructureTypeKHR::eBottomLevel;
  }
  assert(false);
  return {};
}

ezSharedPtr<Resource> VKDevice::CreateAccelerationStructure(AccelerationStructureType type, const ezSharedPtr<Resource>& resource, ezUInt64 offset)
{
  ezSharedPtr<VKResource> res = EZ_DEFAULT_NEW(VKResource, *this);
  res->resource_type = ResourceType::kAccelerationStructure;
  res->acceleration_structures_memory = resource;

  vk::AccelerationStructureCreateInfoKHR accelerationStructureCreateInfo = {};
  accelerationStructureCreateInfo.buffer = resource.Downcast<VKResource>()->buffer.res.get();
  accelerationStructureCreateInfo.offset = offset;
  accelerationStructureCreateInfo.size = 0;
  accelerationStructureCreateInfo.type = Convert(type);
  res->acceleration_structure_handle = m_Device->createAccelerationStructureKHRUnique(accelerationStructureCreateInfo);

  return res;
}

ezSharedPtr<QueryHeap> VKDevice::CreateQueryHeap(QueryHeapType type, ezUInt32 count)
{
  return EZ_DEFAULT_NEW(VKQueryHeap, *this, type, count);
}

ezUInt32 VKDevice::GetTextureDataPitchAlignment() const
{
  return 1;
}

bool VKDevice::IsDxrSupported() const
{
  return m_is_dxr_supported;
}

bool VKDevice::IsRayQuerySupported() const
{
  return m_is_ray_query_supported;
}

bool VKDevice::IsVariableRateShadingSupported() const
{
  return m_is_variable_rate_shading_supported;
}

bool VKDevice::IsMeshShadingSupported() const
{
  return m_is_mesh_shading_supported;
}

ezUInt32 VKDevice::GetShadingRateImageTileSize() const
{
  return m_shading_rate_image_tile_size;
}

MemoryBudget VKDevice::GetMemoryBudget() const
{
  vk::PhysicalDeviceMemoryBudgetPropertiesEXT memory_budget = {};
  vk::PhysicalDeviceMemoryProperties2 mem_properties = {};
  mem_properties.pNext = &memory_budget;
  m_Adapter.GetPhysicalDevice().getMemoryProperties2(&mem_properties);
  MemoryBudget res = {};
  for (size_t i = 0; i < VK_MAX_MEMORY_HEAPS; ++i)
  {
    res.budget += memory_budget.heapBudget[i];
    res.usage += memory_budget.heapUsage[i];
  }
  return res;
}

ezUInt32 VKDevice::GetShaderGroupHandleSize() const
{
  return m_shader_group_handle_size;
}

ezUInt32 VKDevice::GetShaderRecordAlignment() const
{
  return m_shader_record_alignment;
}

ezUInt32 VKDevice::GetShaderTableAlignment() const
{
  return m_shader_table_alignment;
}

RaytracingASPrebuildInfo VKDevice::GetBLASPrebuildInfo(const std::vector<RaytracingGeometryDesc>& descs, BuildAccelerationStructureFlags flags) const
{
  std::vector<vk::AccelerationStructureGeometryKHR> geometry_descs;
  std::vector<ezUInt32> max_primitive_counts;
  for (const auto& desc : descs)
  {
    geometry_descs.emplace_back(FillRaytracingGeometryTriangles(desc.vertex, desc.index, desc.flags));
    if (desc.index.res)
      max_primitive_counts.emplace_back(desc.index.count / 3);
    else
      max_primitive_counts.emplace_back(desc.vertex.count / 3);
  }
  vk::AccelerationStructureBuildGeometryInfoKHR acceleration_structure_info = {};
  acceleration_structure_info.type = vk::AccelerationStructureTypeKHR::eBottomLevel;
  acceleration_structure_info.geometryCount = (ezUInt32)geometry_descs.size();
  acceleration_structure_info.pGeometries = geometry_descs.data();
  acceleration_structure_info.flags = Convert(flags);
  return GetAccelerationStructurePrebuildInfo(acceleration_structure_info, max_primitive_counts);
}

RaytracingASPrebuildInfo VKDevice::GetTLASPrebuildInfo(ezUInt32 instance_count, BuildAccelerationStructureFlags flags) const
{
  vk::AccelerationStructureGeometryKHR geometry_info{};
  geometry_info.geometryType = vk::GeometryTypeKHR::eInstances;
  geometry_info.geometry.setInstances({});

  vk::AccelerationStructureBuildGeometryInfoKHR acceleration_structure_info = {};
  acceleration_structure_info.type = vk::AccelerationStructureTypeKHR::eTopLevel;
  acceleration_structure_info.pGeometries = &geometry_info;
  acceleration_structure_info.geometryCount = 1;
  acceleration_structure_info.flags = Convert(flags);
  return GetAccelerationStructurePrebuildInfo(acceleration_structure_info, {instance_count});
}

VKAdapter& VKDevice::GetAdapter()
{
  return m_Adapter;
}

vk::Device VKDevice::GetDevice()
{
  return m_Device.get();
}

CommandListType VKDevice::GetAvailableCommandListType(CommandListType type)
{
  if (m_QueuesInfo.Contains(type))
  {
    return type;
  }
  return CommandListType::kGraphics;
}

vk::CommandPool VKDevice::GetCmdPool(CommandListType type)
{
  return m_CmdPools[GetAvailableCommandListType(type)].get();
}

vk::ImageAspectFlags VKDevice::GetAspectFlags(vk::Format format) const
{
  switch (format)
  {
    case vk::Format::eD32SfloatS8Uint:
    case vk::Format::eD24UnormS8Uint:
    case vk::Format::eD16UnormS8Uint:
      return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
    case vk::Format::eD16Unorm:
    case vk::Format::eD32Sfloat:
    case vk::Format::eX8D24UnormPack32:
      return vk::ImageAspectFlagBits::eDepth;
    case vk::Format::eS8Uint:
      return vk::ImageAspectFlagBits::eStencil;
    default:
      return vk::ImageAspectFlagBits::eColor;
  }
}

ezUInt32 VKDevice::FindMemoryType(ezUInt32 type_filter, vk::MemoryPropertyFlags properties)
{
  vk::PhysicalDeviceMemoryProperties memProperties;
  m_PhysicalDevice.getMemoryProperties(&memProperties);

  for (ezUInt32 i = 0; i < memProperties.memoryTypeCount; ++i)
  {
    if ((type_filter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
      return i;
  }
  throw std::runtime_error("failed to find suitable memory type!");
}

ezUniquePtr<VKGPUBindlessDescriptorPoolTyped>& VKDevice::GetGPUBindlessDescriptorPool(vk::DescriptorType type)
{
  //auto it = m_GPUBindlessDescriptorPool.find(type);
  //if (it == m_GPUBindlessDescriptorPool.end())
  //  it = m_GPUBindlessDescriptorPool.emplace(std::piecewise_construct, std::forward_as_tuple(type), std::forward_as_tuple(*this, type)).first;
  //return it->second;

  auto it = m_GPUBindlessDescriptorPool.Find(type);
  if (it == end(m_GPUBindlessDescriptorPool))
  {
    it = m_GPUBindlessDescriptorPool.Insert(type, EZ_DEFAULT_NEW(VKGPUBindlessDescriptorPoolTyped, *this, type));
    //it = m_GPUBindlessDescriptorPool.emplace(std::piecewise_construct, std::forward_as_tuple(type), std::forward_as_tuple(*this, type)).first;
  }
  return it.Value();
}

VKGPUDescriptorPool& VKDevice::GetGPUDescriptorPool()
{
  return m_GPUDescriptorPool;
}
