#include <RendererVkPCH.h>

#include <Core/System/Window.h>
#include <Foundation/Basics/Platform/Win/IncludeWindows.h>
#include <Foundation/Configuration/Startup.h>
#include <RendererFoundation/CommandEncoder/RenderCommandEncoder.h>
#include <RendererFoundation/Device/DeviceFactory.h>
#include <RendererVk/CommandEncoder/CommandEncoderImplVk.h>
#include <RendererVk/Device/DeviceVk.h>
#include <RendererVk/Device/PassVk.h>
#include <RendererVk/Device/SwapChainVk.h>
#include <RendererVk/Resources/BufferVk.h>
#include <RendererVk/Resources/FenceVk.h>
#include <RendererVk/Resources/QueryVk.h>
#include <RendererVk/Resources/RenderTargetViewVk.h>
#include <RendererVk/Resources/ResourceViewVk.h>
#include <RendererVk/Resources/TextureVk.h>
#include <RendererVk/Resources/UnorderedAccessViewVk.h>
#include <RendererVk/Shader/ShaderVk.h>
#include <RendererVk/Shader/VertexDeclarationVk.h>
#include <RendererVk/State/StateVk.h>

#include <RendererVk/Implementation/Instance.h>
#include <RendererVk/Implementation/Adapter.h>
#include <RendererVk/Implementation/CommandQueue.h>
#include <RendererVk/Implementation/CommandList.h>

using namespace ezInternal::Vk;




ezInternal::NewInstance<ezGALDevice> CreateVkDevice(ezAllocatorBase* pAllocator, const ezGALDeviceCreationDescription& Description)
{
  return EZ_NEW(pAllocator, ezGALDeviceVk, Description);
}

// clang-format off
EZ_BEGIN_SUBSYSTEM_DECLARATION(RendererVk, DeviceFactory)

ON_CORESYSTEMS_STARTUP
{
  ezGALDeviceFactory::RegisterCreatorFunc("Vk", &CreateVkDevice, "VULKAN", "ezShaderCompilerDXC");
}

ON_CORESYSTEMS_SHUTDOWN
{
  ezGALDeviceFactory::UnregisterCreatorFunc("Vk");
}

EZ_END_SUBSYSTEM_DECLARATION;
// clang-format on

ezGALDeviceVk::ezGALDeviceVk(const ezGALDeviceCreationDescription& Description)
  : ezGALDevice(Description)
{
}

ezGALDeviceVk::~ezGALDeviceVk() = default;

// Init & shutdown functions

ezResult ezGALDeviceVk::InitPlatform(ezUInt32 dwFlags, void* pUsedAdapter)
{
  EZ_LOG_BLOCK("ezGALDeviceVk::InitPlatform");

  m_pInstance = EZ_DEFAULT_NEW(Instance);

  m_pAdapter = m_pInstance->EnumerateAdapters()[0];

  vk::PhysicalDevice& physicalDevice = m_pAdapter->GetPhysicalDevice();

  auto queueFamilies = physicalDevice.getQueueFamilyProperties();
  auto hasAllBits = [](auto flags, auto bits) {
    return (flags & bits) == bits;
  };
  auto hasAnyBits = [](auto flags, auto bits) {
    return flags & bits;
  };
  for (ezUInt32 i = 0; i < (ezUInt32)queueFamilies.size(); ++i)
  {
    const auto& queue = queueFamilies[i];
    if (queue.queueCount > 0 && hasAllBits(queue.queueFlags, vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute | vk::QueueFlagBits::eTransfer))
    {
      m_QueuesInfo[CommandListType::Graphics].QueueFamilyIndex = i;
      m_QueuesInfo[CommandListType::Graphics].QueueCount = queue.queueCount;
    }
    else if (queue.queueCount > 0 && hasAllBits(queue.queueFlags, vk::QueueFlagBits::eCompute | vk::QueueFlagBits::eTransfer) && !hasAnyBits(queue.queueFlags, vk::QueueFlagBits::eGraphics))
    {
      m_QueuesInfo[CommandListType::Compute].QueueFamilyIndex = i;
      m_QueuesInfo[CommandListType::Compute].QueueCount = queue.queueCount;
    }
    else if (queue.queueCount > 0 && hasAllBits(queue.queueFlags, vk::QueueFlagBits::eTransfer) && !hasAnyBits(queue.queueFlags, vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute))
    {
      m_QueuesInfo[CommandListType::Copy].QueueFamilyIndex = i;
      m_QueuesInfo[CommandListType::Copy].QueueCount = queue.queueCount;
    }
  }

  auto extensions = physicalDevice.enumerateDeviceExtensionProperties();
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
      m_IsVariableRateShadingSupported = true;
    if (ezStringUtils::IsEqual(extension.extensionName.data(), VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME))
      m_IsDxrSupported = true;
    if (ezStringUtils::IsEqual(extension.extensionName.data(), VK_NV_MESH_SHADER_EXTENSION_NAME))
      m_IsMeshShadingSupported = true;
    if (ezStringUtils::IsEqual(extension.extensionName.data(), VK_KHR_RAY_QUERY_EXTENSION_NAME))
      m_IsRayQuerySupported = true;
  }

  void* deviceCreateInfoNext = nullptr;
  auto addExtension = [&](auto& extension) {
    extension.pNext = deviceCreateInfoNext;
    deviceCreateInfoNext = &extension;
  };

  if (m_IsVariableRateShadingSupported)
  {
    ezMap<ezGALShadingRate, vk::Extent2D> shadingRatePalette;
    shadingRatePalette.Insert(ezGALShadingRate::ShadingRate1x1, vk::Extent2D{1, 1});
    shadingRatePalette.Insert(ezGALShadingRate::ShadingRate1x2, vk::Extent2D{1, 2});
    shadingRatePalette.Insert(ezGALShadingRate::ShadingRate2x1, vk::Extent2D{2, 1});
    shadingRatePalette.Insert(ezGALShadingRate::ShadingRate2x2, vk::Extent2D{2, 2});
    shadingRatePalette.Insert(ezGALShadingRate::ShadingRate2x4, vk::Extent2D{2, 4});
    shadingRatePalette.Insert(ezGALShadingRate::ShadingRate4x2, vk::Extent2D{4, 2});
    shadingRatePalette.Insert(ezGALShadingRate::ShadingRate4x4, vk::Extent2D{4, 4});

    decltype(auto) fragmentShadingRates = physicalDevice.getFragmentShadingRatesKHR();
    for (const auto& fragment_shading_rate : fragmentShadingRates)
    {
      vk::Extent2D size = fragment_shading_rate.fragmentSize;
      uint8_t shadingRate = ((size.width >> 1) << 2) | (size.height >> 1);
      EZ_ASSERT_ALWAYS((1 << ((shadingRate >> 2) & 3)) == size.width, "");
      EZ_ASSERT_ALWAYS((1 << (shadingRate & 3)) == size.height, "");
      EZ_ASSERT_ALWAYS(shadingRatePalette[(ezGALShadingRate)shadingRate] == size, "");
      shadingRatePalette.Remove((ezGALShadingRate)shadingRate);
    }
    EZ_ASSERT_ALWAYS(shadingRatePalette.IsEmpty(), "");

    vk::PhysicalDeviceFragmentShadingRatePropertiesKHR shadingRateImageProperties = {};
    vk::PhysicalDeviceProperties2 deviceProps2 = {};
    deviceProps2.pNext = &shadingRateImageProperties;
    physicalDevice.getProperties2(&deviceProps2);
    EZ_ASSERT_ALWAYS(shadingRateImageProperties.minFragmentShadingRateAttachmentTexelSize == shadingRateImageProperties.maxFragmentShadingRateAttachmentTexelSize, "");
    EZ_ASSERT_ALWAYS(shadingRateImageProperties.minFragmentShadingRateAttachmentTexelSize.width == shadingRateImageProperties.minFragmentShadingRateAttachmentTexelSize.height, "");
    EZ_ASSERT_ALWAYS(shadingRateImageProperties.maxFragmentShadingRateAttachmentTexelSize.width == shadingRateImageProperties.maxFragmentShadingRateAttachmentTexelSize.height, "");
    m_ShadingRateImageTileSize = shadingRateImageProperties.maxFragmentShadingRateAttachmentTexelSize.width;

    vk::PhysicalDeviceFragmentShadingRateFeaturesKHR fragmentShadingRateFeatures = {};
    fragmentShadingRateFeatures.attachmentFragmentShadingRate = true;
    addExtension(fragmentShadingRateFeatures);
  }

  if (m_IsDxrSupported)
  {
    vk::PhysicalDeviceRayTracingPipelinePropertiesKHR rayTracingProperties = {};
    vk::PhysicalDeviceProperties2 deviceProps2 = {};
    deviceProps2.pNext = &rayTracingProperties;
    physicalDevice.getProperties2(&deviceProps2);
    m_ShaderGroupHandleSize = rayTracingProperties.shaderGroupHandleSize;
    m_ShaderRecordAlignment = rayTracingProperties.shaderGroupHandleSize;
    m_ShaderTableAlignment = rayTracingProperties.shaderGroupBaseAlignment;
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
  if (m_IsMeshShadingSupported)
  {
    addExtension(meshShaderFeature);
  }

  vk::PhysicalDeviceRayTracingPipelineFeaturesKHR raytracingPipelineFeature = {};
  raytracingPipelineFeature.rayTracingPipeline = true;

  vk::PhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeature = {};
  accelerationStructureFeature.accelerationStructure = true;

  vk::PhysicalDeviceRayQueryFeaturesKHR rayqueryPipelineFeature = {};
  rayqueryPipelineFeature.rayQuery = true;

  if (m_IsDxrSupported)
  {
    addExtension(raytracingPipelineFeature);
    addExtension(accelerationStructureFeature);

    if (m_IsRayQuerySupported)
    {
      raytracingPipelineFeature.rayTraversalPrimitiveCulling = true;
      addExtension(rayqueryPipelineFeature);
    }
  }

  vk::DeviceCreateInfo device_create_info = {};
  device_create_info.pNext = deviceCreateInfoNext;
  device_create_info.queueCreateInfoCount = queuesCreateInfo.GetCount();
  device_create_info.pQueueCreateInfos = queuesCreateInfo.GetData();
  device_create_info.pEnabledFeatures = &deviceFeatures;
  device_create_info.enabledExtensionCount = foundExtensions.GetCount();
  device_create_info.ppEnabledExtensionNames = foundExtensions.GetData();

  m_VkDevice = physicalDevice.createDeviceUnique(device_create_info);
  VULKAN_HPP_DEFAULT_DISPATCHER.init(m_VkDevice.get());

  for (auto& queueInfo : m_QueuesInfo)
  {
    vk::CommandPoolCreateInfo cmdPoolCreateInfo = {};
    cmdPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    cmdPoolCreateInfo.queueFamilyIndex = queueInfo.Value().QueueFamilyIndex;
    m_CmdPools.Insert(queueInfo.Key(), m_VkDevice->createCommandPoolUnique(cmdPoolCreateInfo));
    m_CommandQueues[queueInfo.Key()] = EZ_DEFAULT_NEW(CommandQueue, *this, queueInfo.Key(), queueInfo.Value().QueueFamilyIndex);
  }


  return EZ_SUCCESS;
}

ezResult ezGALDeviceVk::InitPlatform()
{
  return InitPlatform(0, nullptr);
}

void ezGALDeviceVk::ReportLiveGpuObjects()
{
}

ezResult ezGALDeviceVk::ShutdownPlatform()
{

  ReportLiveGpuObjects();

  return EZ_SUCCESS;
}

// Pipeline & Pass functions

void ezGALDeviceVk::BeginPipelinePlatform(const char* szName)
{
  m_pDefaultPass->m_pRenderCommandEncoder->PushMarker(szName);
}

void ezGALDeviceVk::EndPipelinePlatform()
{
  m_pDefaultPass->m_pRenderCommandEncoder->PopMarker();
}

ezGALPass* ezGALDeviceVk::BeginPassPlatform(const char* szName)
{
  m_pDefaultPass->BeginPass(szName);

  return m_pDefaultPass.Borrow();
}

void ezGALDeviceVk::EndPassPlatform(ezGALPass* pPass)
{
  EZ_ASSERT_DEV(m_pDefaultPass.Borrow() == pPass, "Invalid pass");

  m_pDefaultPass->EndPass();
}

// State creation functions

ezGALBlendState* ezGALDeviceVk::CreateBlendStatePlatform(const ezGALBlendStateCreationDescription& Description)
{
  ezGALBlendStateVk* pState = EZ_NEW(&m_Allocator, ezGALBlendStateVk, Description);

  if (pState->InitPlatform(this).Succeeded())
  {
    return pState;
  }
  else
  {
    EZ_DELETE(&m_Allocator, pState);
    return nullptr;
  }
}

void ezGALDeviceVk::DestroyBlendStatePlatform(ezGALBlendState* pBlendState)
{
  ezGALBlendStateVk* pState = static_cast<ezGALBlendStateVk*>(pBlendState);
  pState->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pState);
}

ezGALDepthStencilState* ezGALDeviceVk::CreateDepthStencilStatePlatform(const ezGALDepthStencilStateCreationDescription& Description)
{
  ezGALDepthStencilStateVk* pVkDepthStencilState = EZ_NEW(&m_Allocator, ezGALDepthStencilStateVk, Description);

  if (pVkDepthStencilState->InitPlatform(this).Succeeded())
  {
    return pVkDepthStencilState;
  }
  else
  {
    EZ_DELETE(&m_Allocator, pVkDepthStencilState);
    return nullptr;
  }
}

void ezGALDeviceVk::DestroyDepthStencilStatePlatform(ezGALDepthStencilState* pDepthStencilState)
{
  ezGALDepthStencilStateVk* pVkDepthStencilState = static_cast<ezGALDepthStencilStateVk*>(pDepthStencilState);
  pVkDepthStencilState->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pVkDepthStencilState);
}

ezGALRasterizerState* ezGALDeviceVk::CreateRasterizerStatePlatform(const ezGALRasterizerStateCreationDescription& Description)
{
  ezGALRasterizerStateVk* pVkRasterizerState = EZ_NEW(&m_Allocator, ezGALRasterizerStateVk, Description);

  if (pVkRasterizerState->InitPlatform(this).Succeeded())
  {
    return pVkRasterizerState;
  }
  else
  {
    EZ_DELETE(&m_Allocator, pVkRasterizerState);
    return nullptr;
  }
}

void ezGALDeviceVk::DestroyRasterizerStatePlatform(ezGALRasterizerState* pRasterizerState)
{
  ezGALRasterizerStateVk* pVkRasterizerState = static_cast<ezGALRasterizerStateVk*>(pRasterizerState);
  pVkRasterizerState->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pVkRasterizerState);
}

ezGALSamplerState* ezGALDeviceVk::CreateSamplerStatePlatform(const ezGALSamplerStateCreationDescription& Description)
{
  ezGALSamplerStateVk* pVkSamplerState = EZ_NEW(&m_Allocator, ezGALSamplerStateVk, Description);

  if (pVkSamplerState->InitPlatform(this).Succeeded())
  {
    return pVkSamplerState;
  }
  else
  {
    EZ_DELETE(&m_Allocator, pVkSamplerState);
    return nullptr;
  }
}

void ezGALDeviceVk::DestroySamplerStatePlatform(ezGALSamplerState* pSamplerState)
{
  ezGALSamplerStateVk* pVkSamplerState = static_cast<ezGALSamplerStateVk*>(pSamplerState);
  pVkSamplerState->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pVkSamplerState);
}


// Resource creation functions

ezGALShader* ezGALDeviceVk::CreateShaderPlatform(const ezGALShaderCreationDescription& Description)
{
  ezGALShaderVk* pShader = EZ_NEW(&m_Allocator, ezGALShaderVk, Description);

  if (!pShader->InitPlatform(this).Succeeded())
  {
    EZ_DELETE(&m_Allocator, pShader);
    return nullptr;
  }

  return pShader;
}

void ezGALDeviceVk::DestroyShaderPlatform(ezGALShader* pShader)
{
  ezGALShaderVk* pVkShader = static_cast<ezGALShaderVk*>(pShader);
  pVkShader->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pVkShader);
}

ezGALBuffer* ezGALDeviceVk::CreateBufferPlatform(const ezGALBufferCreationDescription& Description, ezArrayPtr<const ezUInt8> pInitialData)
{
  ezGALBufferVk* pBuffer = EZ_NEW(&m_Allocator, ezGALBufferVk, Description);

  if (!pBuffer->InitPlatform(this, pInitialData).Succeeded())
  {
    EZ_DELETE(&m_Allocator, pBuffer);
    return nullptr;
  }

  return pBuffer;
}

void ezGALDeviceVk::DestroyBufferPlatform(ezGALBuffer* pBuffer)
{
  ezGALBufferVk* pVkBuffer = static_cast<ezGALBufferVk*>(pBuffer);
  pVkBuffer->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pVkBuffer);
}

ezGALTexture* ezGALDeviceVk::CreateTexturePlatform(const ezGALTextureCreationDescription& Description, ezArrayPtr<ezGALSystemMemoryDescription> pInitialData)
{
  ezGALTextureVk* pTexture = EZ_NEW(&m_Allocator, ezGALTextureVk, Description);

  if (!pTexture->InitPlatform(this, pInitialData).Succeeded())
  {
    EZ_DELETE(&m_Allocator, pTexture);
    return nullptr;
  }

  return pTexture;
}

void ezGALDeviceVk::DestroyTexturePlatform(ezGALTexture* pTexture)
{
  ezGALTextureVk* pVkTexture = static_cast<ezGALTextureVk*>(pTexture);
  pVkTexture->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pVkTexture);
}

ezGALResourceView* ezGALDeviceVk::CreateResourceViewPlatform(ezGALResourceBase* pResource, const ezGALResourceViewCreationDescription& Description)
{
  ezGALResourceViewVk* pResourceView = EZ_NEW(&m_Allocator, ezGALResourceViewVk, pResource, Description);

  if (!pResourceView->InitPlatform(this).Succeeded())
  {
    EZ_DELETE(&m_Allocator, pResourceView);
    return nullptr;
  }

  return pResourceView;
}

void ezGALDeviceVk::DestroyResourceViewPlatform(ezGALResourceView* pResourceView)
{
  ezGALResourceViewVk* pVkResourceView = static_cast<ezGALResourceViewVk*>(pResourceView);
  pVkResourceView->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pVkResourceView);
}

ezGALRenderTargetView* ezGALDeviceVk::CreateRenderTargetViewPlatform(ezGALTexture* pTexture, const ezGALRenderTargetViewCreationDescription& Description)
{
  ezGALRenderTargetViewVk* pRTView = EZ_NEW(&m_Allocator, ezGALRenderTargetViewVk, pTexture, Description);

  if (!pRTView->InitPlatform(this).Succeeded())
  {
    EZ_DELETE(&m_Allocator, pRTView);
    return nullptr;
  }

  return pRTView;
}

void ezGALDeviceVk::DestroyRenderTargetViewPlatform(ezGALRenderTargetView* pRenderTargetView)
{
  ezGALRenderTargetViewVk* pVkRenderTargetView = static_cast<ezGALRenderTargetViewVk*>(pRenderTargetView);
  pVkRenderTargetView->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pVkRenderTargetView);
}

ezGALUnorderedAccessView* ezGALDeviceVk::CreateUnorderedAccessViewPlatform(ezGALResourceBase* pTextureOfBuffer, const ezGALUnorderedAccessViewCreationDescription& Description)
{
  ezGALUnorderedAccessViewVk* pUnorderedAccessView = EZ_NEW(&m_Allocator, ezGALUnorderedAccessViewVk, pTextureOfBuffer, Description);

  if (!pUnorderedAccessView->InitPlatform(this).Succeeded())
  {
    EZ_DELETE(&m_Allocator, pUnorderedAccessView);
    return nullptr;
  }

  return pUnorderedAccessView;
}

void ezGALDeviceVk::DestroyUnorderedAccessViewPlatform(ezGALUnorderedAccessView* pUnorderedAccessView)
{
  ezGALUnorderedAccessViewVk* pUnorderedAccessViewVk = static_cast<ezGALUnorderedAccessViewVk*>(pUnorderedAccessView);
  pUnorderedAccessViewVk->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pUnorderedAccessViewVk);
}



// Other rendering creation functions
ezGALSwapChain* ezGALDeviceVk::CreateSwapChainPlatform(const ezGALSwapChainCreationDescription& Description)
{
  ezGALSwapChainVk* pSwapChain = EZ_NEW(&m_Allocator, ezGALSwapChainVk, Description, *m_CommandQueues[ezInternal::Vk::CommandListType::Graphics]);

  if (!pSwapChain->InitPlatform(this).Succeeded())
  {
    EZ_DELETE(&m_Allocator, pSwapChain);
    return nullptr;
  }

  return pSwapChain;
}

void ezGALDeviceVk::DestroySwapChainPlatform(ezGALSwapChain* pSwapChain)
{
  ezGALSwapChainVk* pSwapChainVk = static_cast<ezGALSwapChainVk*>(pSwapChain);
  pSwapChainVk->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pSwapChainVk);
}

ezGALFence* ezGALDeviceVk::CreateFencePlatform(ezUInt64 initialValue)
{
  ezGALFenceVk* pFence = EZ_NEW(&m_Allocator, ezGALFenceVk);

  if (!pFence->InitPlatform(this, initialValue).Succeeded())
  {
    EZ_DELETE(&m_Allocator, pFence);
    return nullptr;
  }

  return pFence;
}

void ezGALDeviceVk::DestroyFencePlatform(ezGALFence* pFence)
{
  ezGALFenceVk* pFenceVk = static_cast<ezGALFenceVk*>(pFence);
  pFenceVk->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pFenceVk);
}

ezGALQuery* ezGALDeviceVk::CreateQueryPlatform(const ezGALQueryCreationDescription& Description)
{
  ezGALQueryVk* pQuery = EZ_NEW(&m_Allocator, ezGALQueryVk, Description);

  if (!pQuery->InitPlatform(this).Succeeded())
  {
    EZ_DELETE(&m_Allocator, pQuery);
    return nullptr;
  }

  return pQuery;
}

void ezGALDeviceVk::DestroyQueryPlatform(ezGALQuery* pQuery)
{
  ezGALQueryVk* pQueryVk = static_cast<ezGALQueryVk*>(pQuery);
  pQueryVk->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pQueryVk);
}

ezGALVertexDeclaration* ezGALDeviceVk::CreateVertexDeclarationPlatform(const ezGALVertexDeclarationCreationDescription& Description)
{
  ezGALVertexDeclarationVk* pVertexDeclaration = EZ_NEW(&m_Allocator, ezGALVertexDeclarationVk, Description);

  if (pVertexDeclaration->InitPlatform(this).Succeeded())
  {
    return pVertexDeclaration;
  }
  else
  {
    EZ_DELETE(&m_Allocator, pVertexDeclaration);
    return nullptr;
  }
}

void ezGALDeviceVk::DestroyVertexDeclarationPlatform(ezGALVertexDeclaration* pVertexDeclaration)
{
  ezGALVertexDeclarationVk* pVertexDeclarationVk = static_cast<ezGALVertexDeclarationVk*>(pVertexDeclaration);
  pVertexDeclarationVk->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pVertexDeclarationVk);
}

ezGALTimestampHandle ezGALDeviceVk::GetTimestampPlatform()
{
  return {0, 0};
}

ezResult ezGALDeviceVk::GetTimestampResultPlatform(ezGALTimestampHandle hTimestamp, ezTime& result)
{
  return EZ_SUCCESS;
}

// Swap chain functions

void ezGALDeviceVk::PresentPlatform(ezGALSwapChain* pSwapChain, bool bVSync)
{
}

// Misc functions

void ezGALDeviceVk::BeginFramePlatform()
{
}

void ezGALDeviceVk::EndFramePlatform()
{
}

void ezGALDeviceVk::SetPrimarySwapChainPlatform(ezGALSwapChain* pSwapChain)
{
}


void ezGALDeviceVk::FillCapabilitiesPlatform()
{
}

ezInternal::Vk::CommandListType ezGALDeviceVk::GetAvailableCommandListType(ezInternal::Vk::CommandListType type) const
{
  if (m_QueuesInfo.Contains(type))
  {
    return type;
  }
  return ezInternal::Vk::CommandListType::Graphics;
}

vk::CommandPool ezGALDeviceVk::GetCmdPool(ezInternal::Vk::CommandListType type) const
{
  return (*m_CmdPools.GetValue(GetAvailableCommandListType(type))).get();
}

ezSharedPtr<ezInternal::Vk::CommandList> ezGALDeviceVk::CreateCommandList(ezInternal::Vk::CommandListType type)
{
  return EZ_DEFAULT_NEW(ezInternal::Vk::CommandList, *this, type);
}

EZ_STATICLINK_FILE(RendererVk, RendererVk_Device_Implementation_DeviceVk);
