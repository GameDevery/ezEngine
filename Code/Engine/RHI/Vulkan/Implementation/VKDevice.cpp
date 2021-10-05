#include <RHI/Vulkan/VKStd.h>

#include <RHI/Base/SPIRVUtils.h>
#include <RHI/Vulkan/States/VKGlobalBarrier.h>
#include <RHI/Vulkan/States/VKSampler.h>
#include <RHI/Vulkan/States/VKTextureBarrier.h>
#include <RHI/Vulkan/VKBuffer.h>
#include <RHI/Vulkan/VKCommandBuffer.h>
#include <RHI/Vulkan/VKDescriptorSet.h>
#include <RHI/Vulkan/VKDescriptorSetLayout.h>
#include <RHI/Vulkan/VKDevice.h>
#include <RHI/Vulkan/VKFramebuffer.h>
#include <RHI/Vulkan/VKInputAssembler.h>
#include <RHI/Vulkan/VKPipelineLayout.h>
#include <RHI/Vulkan/VKPipelineState.h>
#include <RHI/Vulkan/VKQueue.h>
#include <RHI/Vulkan/VKRenderPass.h>
#include <RHI/Vulkan/VKShader.h>
#include <RHI/Vulkan/VKSwapchain.h>
#include <RHI/Vulkan/VKTexture.h>
#include <RHI/Vulkan/VKUtils.h>

//CC_DISABLE_WARNINGS()
#define VMA_IMPLEMENTATION
#include <RHI/Vulkan/ThirdParty/vk_mem_alloc.h>
#define THSVS_ERROR_CHECK_MIXED_IMAGE_LAYOUT
#define THSVS_ERROR_CHECK_COULD_USE_GLOBAL_BARRIER
#define THSVS_ERROR_CHECK_POTENTIAL_HAZARD
#define THSVS_SIMPLER_VULKAN_SYNCHRONIZATION_IMPLEMENTATION
#include <RHI/Vulkan/ThirdParty/thsvs_simpler_vulkan_synchronization.h>
//CC_ENABLE_WARNINGS()

static VkResult VKAPI_PTR vkCreateRenderPass2KHRFallback(
  VkDevice device,
  const VkRenderPassCreateInfo2* pCreateInfo,
  const VkAllocationCallbacks* pAllocator,
  VkRenderPass* pRenderPass);

CCVKDevice* CCVKDevice::instance = nullptr;

CCVKDevice* CCVKDevice::getInstance()
{
  return CCVKDevice::instance;
}

CCVKDevice::CCVKDevice()
{
  _api = API::VULKAN;
  _deviceName = "Vulkan";

  _caps.clipSpaceMinZ = 0.0F;
  _caps.screenSpaceSignY = -1.0F;
  _caps.clipSpaceSignY = -1.0F;
  CCVKDevice::instance = this;
}

CCVKDevice::~CCVKDevice()
{
  CCVKDevice::instance = nullptr;
}

bool CCVKDevice::doInit(const DeviceInfo& /*info*/)
{
  _gpuContext = EZ_DEFAULT_NEW(CCVKGPUContext);
  if (!_gpuContext->initialize())
  {
    EZ_DEFAULT_DELETE(_gpuContext)
    return false;
  }

  const VkPhysicalDeviceFeatures2& deviceFeatures2 = _gpuContext->physicalDeviceFeatures2;
  const VkPhysicalDeviceFeatures& deviceFeatures = deviceFeatures2.features;
  //const VkPhysicalDeviceVulkan11Features &deviceVulkan11Features = _gpuContext->physicalDeviceVulkan11Features;
  //const VkPhysicalDeviceVulkan12Features &deviceVulkan12Features = _gpuContext->physicalDeviceVulkan12Features;

  ///////////////////// Device Creation /////////////////////

  _gpuDevice = EZ_DEFAULT_NEW(CCVKGPUDevice);
  _gpuDevice->minorVersion = _gpuContext->minorVersion;

  // only enable the absolute essentials
  ezDynamicArray<const char*> requestedLayers{};
  ezDynamicArray<const char*> requestedExtensions;

  requestedExtensions.PushBack(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
  if (_gpuDevice->minorVersion < 2)
  {
    requestedExtensions.PushBack(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME);
  }
  if (_gpuDevice->minorVersion < 1)
  {
    requestedExtensions.PushBack(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
    requestedExtensions.PushBack(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
    requestedExtensions.PushBack(VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME);
  }

  VkPhysicalDeviceFeatures2 requestedFeatures2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
  VkPhysicalDeviceVulkan11Features requestedVulkan11Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES};
  VkPhysicalDeviceVulkan12Features requestedVulkan12Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
  // features should be enabled like this:
  requestedFeatures2.features.textureCompressionASTC_LDR = deviceFeatures.textureCompressionASTC_LDR;
  requestedFeatures2.features.textureCompressionBC = deviceFeatures.textureCompressionBC;
  requestedFeatures2.features.textureCompressionETC2 = deviceFeatures.textureCompressionETC2;
  requestedFeatures2.features.samplerAnisotropy = deviceFeatures.samplerAnisotropy;
  requestedFeatures2.features.depthBounds = deviceFeatures.depthBounds;
  requestedFeatures2.features.multiDrawIndirect = deviceFeatures.multiDrawIndirect;

  if (_gpuContext->validationEnabled)
  {
    requestedLayers.PushBack("VK_LAYER_KHRONOS_validation");
  }

  // check extensions
  ezUInt32 availableLayerCount;
  VK_CHECK(vkEnumerateDeviceLayerProperties(_gpuContext->physicalDevice, &availableLayerCount, nullptr));
  _gpuDevice->layers.SetCount(availableLayerCount);
  VK_CHECK(vkEnumerateDeviceLayerProperties(_gpuContext->physicalDevice, &availableLayerCount, _gpuDevice->layers.GetData()));

  ezUInt32 availableExtensionCount;
  VK_CHECK(vkEnumerateDeviceExtensionProperties(_gpuContext->physicalDevice, nullptr, &availableExtensionCount, nullptr));
  _gpuDevice->extensions.SetCount(availableExtensionCount);
  VK_CHECK(vkEnumerateDeviceExtensionProperties(_gpuContext->physicalDevice, nullptr, &availableExtensionCount, _gpuDevice->extensions.GetData()));

  // just filter out the unsupported layers & extensions
  for (const char* layer : requestedLayers)
  {
    if (isLayerSupported(layer, _gpuDevice->layers))
    {
      _layers.PushBack(layer);
    }
  }
  for (const char* extension : requestedExtensions)
  {
    if (isExtensionSupported(extension, _gpuDevice->extensions))
    {
      _extensions.PushBack(extension);
    }
  }

  // prepare the device queues
  ezUInt32 queueFamilyPropertiesCount = _gpuContext->queueFamilyProperties.GetCount();
  ezDynamicArray<VkDeviceQueueCreateInfo> queueCreateInfos;
  queueCreateInfos.SetCount(queueFamilyPropertiesCount, {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO});
  ezDynamicArray<ezDynamicArray<float>> queuePriorities;
  queuePriorities.SetCount(queueFamilyPropertiesCount);

  for (ezUInt32 queueFamilyIndex = 0U; queueFamilyIndex < queueFamilyPropertiesCount; ++queueFamilyIndex)
  {
    const VkQueueFamilyProperties& queueFamilyProperty = _gpuContext->queueFamilyProperties[queueFamilyIndex];

    queuePriorities[queueFamilyIndex].SetCount(queueFamilyProperty.queueCount, 1.0F);

    VkDeviceQueueCreateInfo& queueCreateInfo = queueCreateInfos[queueFamilyIndex];

    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    queueCreateInfo.queueCount = queueFamilyProperty.queueCount;
    queueCreateInfo.pQueuePriorities = queuePriorities[queueFamilyIndex].GetData();
  }

  VkDeviceCreateInfo deviceCreateInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};

  deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.GetCount();
  deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.GetData();
  deviceCreateInfo.enabledLayerCount = _layers.GetCount();
  deviceCreateInfo.ppEnabledLayerNames = _layers.GetData();
  deviceCreateInfo.enabledExtensionCount = _extensions.GetCount();
  deviceCreateInfo.ppEnabledExtensionNames = _extensions.GetData();
  if (_gpuDevice->minorVersion < 1 && !_gpuContext->checkExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
  {
    deviceCreateInfo.pEnabledFeatures = &requestedFeatures2.features;
  }
  else
  {
    deviceCreateInfo.pNext = &requestedFeatures2;
    if (_gpuDevice->minorVersion >= 2)
    {
      requestedFeatures2.pNext = &requestedVulkan11Features;
      requestedVulkan11Features.pNext = &requestedVulkan12Features;
    }
  }

  VK_CHECK(vkCreateDevice(_gpuContext->physicalDevice, &deviceCreateInfo, nullptr, &_gpuDevice->vkDevice));

  volkLoadDevice(_gpuDevice->vkDevice);

  SPIRVUtils::getInstance()->initialize(static_cast<int>(_gpuDevice->minorVersion));

  ///////////////////// Gather Device Properties /////////////////////

  auto findPreferredDepthFormat = [this](const VkFormat* formats, ezUInt32 count, VkFormat* pFormat)
  {
    for (ezUInt32 i = 0; i < count; ++i)
    {
      VkFormat format = formats[i];
      VkFormatProperties formatProperties;
      vkGetPhysicalDeviceFormatProperties(_gpuContext->physicalDevice, format, &formatProperties);
      // Format must support depth stencil attachment for optimal tiling
      if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
      {
        if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)
        {
          *pFormat = format;
          break;
        }
      }
    }
  };

  VkFormat depthFormatPriorityList[]{
    VK_FORMAT_D32_SFLOAT,
    VK_FORMAT_X8_D24_UNORM_PACK32,
    VK_FORMAT_D16_UNORM,
  };
  findPreferredDepthFormat(depthFormatPriorityList, 3, &_gpuDevice->depthFormat);

  VkFormat depthStencilFormatPriorityList[]{
    VK_FORMAT_D32_SFLOAT_S8_UINT,
    VK_FORMAT_D24_UNORM_S8_UINT,
    VK_FORMAT_D16_UNORM_S8_UINT,
  };
  findPreferredDepthFormat(depthStencilFormatPriorityList, 3, &_gpuDevice->depthStencilFormat);

  _features[ToNumber(Feature::COLOR_FLOAT)] = true;
  _features[ToNumber(Feature::COLOR_HALF_FLOAT)] = true;
  _features[ToNumber(Feature::TEXTURE_FLOAT)] = true;
  _features[ToNumber(Feature::TEXTURE_HALF_FLOAT)] = true;
  _features[ToNumber(Feature::TEXTURE_FLOAT_LINEAR)] = true;
  _features[ToNumber(Feature::TEXTURE_HALF_FLOAT_LINEAR)] = true;
  _features[ToNumber(Feature::FORMAT_R11G11B10F)] = true;
  _features[ToNumber(Feature::FORMAT_SRGB)] = true;
  _features[ToNumber(Feature::ELEMENT_INDEX_UINT)] = true;
  _features[ToNumber(Feature::INSTANCED_ARRAYS)] = true;
  _features[ToNumber(Feature::MULTIPLE_RENDER_TARGETS)] = true;
  _features[ToNumber(Feature::BLEND_MINMAX)] = true;
  _features[ToNumber(Feature::COMPUTE_SHADER)] = true;
  _features[ToNumber(Feature::INPUT_ATTACHMENT_BENEFIT)] = true;

  _gpuDevice->useMultiDrawIndirect = deviceFeatures.multiDrawIndirect;
  _gpuDevice->useDescriptorUpdateTemplate = _gpuDevice->minorVersion > 0 || checkExtension(VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME);

  if (_gpuDevice->minorVersion > 1)
  {
    _gpuDevice->createRenderPass2 = vkCreateRenderPass2;
  }
  else if (checkExtension(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME))
  {
    _gpuDevice->createRenderPass2 = vkCreateRenderPass2KHR;
  }
  else
  {
    _gpuDevice->createRenderPass2 = vkCreateRenderPass2KHRFallback;
  }

  VkFormatFeatureFlags requiredFeatures = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
  VkFormatProperties formatProperties;
  vkGetPhysicalDeviceFormatProperties(_gpuContext->physicalDevice, VK_FORMAT_R8G8B8_UNORM, &formatProperties);
  if (formatProperties.optimalTilingFeatures & requiredFeatures)
  {
    _features[ToNumber(Feature::FORMAT_RGB8)] = true;
  }

  ezStringBuilder compressedFmts;
  if (deviceFeatures.textureCompressionETC2)
  {
    _features[ToNumber(Feature::FORMAT_ETC2)] = true;
    compressedFmts.Append("etc2 ");
  }
  if (deviceFeatures.textureCompressionASTC_LDR)
  {
    _features[ToNumber(Feature::FORMAT_ASTC)] = true;
    compressedFmts.Append("astc ");
  }

  const VkPhysicalDeviceLimits& limits = _gpuContext->physicalDeviceProperties.limits;
  _caps.maxVertexAttributes = limits.maxVertexInputAttributes;
  _caps.maxVertexUniformVectors = limits.maxUniformBufferRange / 16;
  _caps.maxFragmentUniformVectors = limits.maxUniformBufferRange / 16;
  _caps.maxUniformBufferBindings = limits.maxDescriptorSetUniformBuffers;
  _caps.maxUniformBlockSize = limits.maxUniformBufferRange;
  _caps.maxShaderStorageBlockSize = limits.maxStorageBufferRange;
  _caps.maxShaderStorageBufferBindings = limits.maxDescriptorSetStorageBuffers;
  _caps.maxTextureUnits = limits.maxDescriptorSetSampledImages;
  _caps.maxVertexTextureUnits = limits.maxPerStageDescriptorSampledImages;
  _caps.maxTextureSize = limits.maxImageDimension2D;
  _caps.maxCubeMapTextureSize = limits.maxImageDimensionCube;
  _caps.uboOffsetAlignment = static_cast<ezUInt32>(limits.minUniformBufferOffsetAlignment);
  // compute shaders
  _caps.maxComputeSharedMemorySize = limits.maxComputeSharedMemorySize;
  _caps.maxComputeWorkGroupInvocations = limits.maxComputeWorkGroupInvocations;
  _caps.maxComputeWorkGroupCount = {limits.maxComputeWorkGroupCount[0], limits.maxComputeWorkGroupCount[1], limits.maxComputeWorkGroupCount[2]};
  _caps.maxComputeWorkGroupSize = {limits.maxComputeWorkGroupSize[0], limits.maxComputeWorkGroupSize[1], limits.maxComputeWorkGroupSize[2]};
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
  // UNASSIGNED-BestPractices-vkCreateComputePipelines-compute-work-group-size
  _caps.maxComputeWorkGroupInvocations = 64;
#endif // defined(VK_USE_PLATFORM_ANDROID_KHR)

  ///////////////////// Resource Initialization /////////////////////

  QueueInfo queueInfo;
  queueInfo.type = QueueType::GRAPHICS;
  _queue = createQueue(queueInfo);

  CommandBufferInfo cmdBuffInfo;
  cmdBuffInfo.type = CommandBufferType::PRIMARY;
  cmdBuffInfo.queue = _queue;
  _cmdBuff = createCommandBuffer(cmdBuffInfo);

  VmaAllocatorCreateInfo allocatorInfo{};
  allocatorInfo.physicalDevice = _gpuContext->physicalDevice;
  allocatorInfo.device = _gpuDevice->vkDevice;
  allocatorInfo.instance = _gpuContext->vkInstance;

  VmaVulkanFunctions vmaVulkanFunc{};
  vmaVulkanFunc.vkAllocateMemory = vkAllocateMemory;
  vmaVulkanFunc.vkBindBufferMemory = vkBindBufferMemory;
  vmaVulkanFunc.vkBindImageMemory = vkBindImageMemory;
  vmaVulkanFunc.vkCreateBuffer = vkCreateBuffer;
  vmaVulkanFunc.vkCreateImage = vkCreateImage;
  vmaVulkanFunc.vkDestroyBuffer = vkDestroyBuffer;
  vmaVulkanFunc.vkDestroyImage = vkDestroyImage;
  vmaVulkanFunc.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
  vmaVulkanFunc.vkFreeMemory = vkFreeMemory;
  vmaVulkanFunc.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
  vmaVulkanFunc.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
  vmaVulkanFunc.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
  vmaVulkanFunc.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
  vmaVulkanFunc.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
  vmaVulkanFunc.vkMapMemory = vkMapMemory;
  vmaVulkanFunc.vkUnmapMemory = vkUnmapMemory;
  vmaVulkanFunc.vkCmdCopyBuffer = vkCmdCopyBuffer;

  if (_gpuDevice->minorVersion > 0)
  {
    allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
    vmaVulkanFunc.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2;
    vmaVulkanFunc.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2;
    vmaVulkanFunc.vkBindBufferMemory2KHR = vkBindBufferMemory2;
    vmaVulkanFunc.vkBindImageMemory2KHR = vkBindImageMemory2;
  }
  else
  {
    if (checkExtension(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME) &&
        checkExtension(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME))
    {
      allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
      vmaVulkanFunc.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR;
      vmaVulkanFunc.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;
    }
    if (checkExtension(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME))
    {
      vmaVulkanFunc.vkBindBufferMemory2KHR = vkBindBufferMemory2KHR;
      vmaVulkanFunc.vkBindImageMemory2KHR = vkBindImageMemory2KHR;
    }
  }
  if (checkExtension(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME))
  {
    if (_gpuDevice->minorVersion > 0)
    {
      vmaVulkanFunc.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2;
    }
    else if (checkExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
    {
      vmaVulkanFunc.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR;
    }
  }

  allocatorInfo.pVulkanFunctions = &vmaVulkanFunc;

  VK_CHECK(vmaCreateAllocator(&allocatorInfo, &_gpuDevice->memoryAllocator));

  ezUInt32 backBufferCount = _gpuDevice->backBufferCount;
  for (ezUInt32 i = 0U; i < backBufferCount; i++)
  {
    _gpuFencePools.PushBack(EZ_DEFAULT_NEW(CCVKGPUFencePool, _gpuDevice));
    _gpuRecycleBins.PushBack(EZ_DEFAULT_NEW(CCVKGPURecycleBin, _gpuDevice));
    _gpuStagingBufferPools.PushBack(EZ_DEFAULT_NEW(CCVKGPUStagingBufferPool, _gpuDevice));
  }

  _gpuBufferHub = EZ_DEFAULT_NEW(CCVKGPUBufferHub, _gpuDevice);
  _gpuTransportHub = EZ_DEFAULT_NEW(CCVKGPUTransportHub, _gpuDevice, static_cast<CCVKQueue*>(_queue)->gpuQueue());
  _gpuDescriptorHub = EZ_DEFAULT_NEW(CCVKGPUDescriptorHub, _gpuDevice);
  _gpuSemaphorePool = EZ_DEFAULT_NEW(CCVKGPUSemaphorePool, _gpuDevice);
  _gpuBarrierManager = EZ_DEFAULT_NEW(CCVKGPUBarrierManager, _gpuDevice);
  _gpuDescriptorSetHub = EZ_DEFAULT_NEW(CCVKGPUDescriptorSetHub, _gpuDevice);

  _gpuDescriptorHub->link(_gpuDescriptorSetHub);

  cmdFuncCCVKCreateSampler(this, &_gpuDevice->defaultSampler);

  _gpuDevice->defaultTexture.format = Format::RGBA8;
  _gpuDevice->defaultTexture.usage = TextureUsageBit::SAMPLED | TextureUsage::STORAGE;
  _gpuDevice->defaultTexture.width = _gpuDevice->defaultTexture.height = 1U;
  _gpuDevice->defaultTexture.size = FormatSize(Format::RGBA8, 1U, 1U, 1U);
  cmdFuncCCVKCreateTexture(this, &_gpuDevice->defaultTexture);

  _gpuDevice->defaultTextureView.gpuTexture = &_gpuDevice->defaultTexture;
  _gpuDevice->defaultTextureView.format = Format::RGBA8;
  cmdFuncCCVKCreateTextureView(this, &_gpuDevice->defaultTextureView);

  ThsvsImageBarrier barrier{};
  barrier.nextAccessCount = 1;
  barrier.pNextAccesses = &THSVS_ACCESS_TYPES[ToNumber(AccessType::VERTEX_SHADER_READ_TEXTURE)];
  barrier.image = _gpuDevice->defaultTexture.vkImage;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
  barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
  gpuTransportHub()->checkIn(
    [&barrier](const CCVKGPUCommandBuffer* gpuCommandBuffer)
    {
      cmdFuncCCVKImageMemoryBarrier(gpuCommandBuffer, barrier);
    },
    true);

  _gpuDevice->defaultBuffer.usage = BufferUsage::UNIFORM;
  _gpuDevice->defaultBuffer.memUsage = MemoryUsage::HOST | MemoryUsage::DEVICE;
  _gpuDevice->defaultBuffer.size = _gpuDevice->defaultBuffer.stride = 16U;
  _gpuDevice->defaultBuffer.count = 1U;
  cmdFuncCCVKCreateBuffer(this, &_gpuDevice->defaultBuffer);

  VkPipelineCacheCreateInfo pipelineCacheInfo{VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO};
  VK_CHECK(vkCreatePipelineCache(_gpuDevice->vkDevice, &pipelineCacheInfo, nullptr, &_gpuDevice->vkPipelineCache));

  ///////////////////// Print Debug Info /////////////////////

  ezStringBuilder instanceLayers;
  ezStringBuilder instanceExtensions;
  ezStringBuilder deviceLayers;
  ezStringBuilder deviceExtensions;
  for (const char* layer : _gpuContext->layers)
  {
    instanceLayers.Append(layer, " ");
  }
  for (const char* extension : _gpuContext->extensions)
  {
    instanceExtensions.Append(extension, " ");
  }
  for (const char* layer : _layers)
  {
    deviceLayers.Append(layer, " ");
  }
  for (const char* extension : _extensions)
  {
    deviceExtensions.Append(extension, " ");
  }

  ezUInt32 apiVersion = _gpuContext->physicalDeviceProperties.apiVersion;
  _renderer = _gpuContext->physicalDeviceProperties.deviceName;
  _vendor = mapVendorName(_gpuContext->physicalDeviceProperties.vendorID);
  ezStringBuilder versionString;
  ezFmt("{}.{}.{}", VK_VERSION_MAJOR(apiVersion),
    VK_VERSION_MINOR(apiVersion), VK_VERSION_PATCH(apiVersion))
    .GetText(versionString);
  _version = versionString;

  ezLog::Info("Vulkan device initialized.");
  ezLog::Info("RENDERER: {}", _renderer);
  ezLog::Info("VENDOR: {}", _vendor);
  ezLog::Info("VERSION: {}", _version);
  ezLog::Info("INSTANCE_LAYERS: {}", instanceLayers);
  ezLog::Info("INSTANCE_EXTENSIONS: {}", instanceExtensions);
  ezLog::Info("DEVICE_LAYERS: {}", deviceLayers);
  ezLog::Info("DEVICE_EXTENSIONS: {}", deviceExtensions);
  ezLog::Info("COMPRESSED_FORMATS: {}", compressedFmts);

  return true;
}

void CCVKDevice::doDestroy()
{
  waitAllFences();

  SPIRVUtils::getInstance()->destroy();

  for (CCVKTexture* texture : _depthStencilTextures)
  {
    EZ_DEFAULT_DELETE(texture)
  }
  _depthStencilTextures.Clear();

  EZ_DEFAULT_DELETE(_queue)
  EZ_DEFAULT_DELETE(_cmdBuff)

  EZ_DEFAULT_DELETE(_gpuBufferHub)
  EZ_DEFAULT_DELETE(_gpuTransportHub)
  EZ_DEFAULT_DELETE(_gpuSemaphorePool)
  EZ_DEFAULT_DELETE(_gpuDescriptorHub)
  EZ_DEFAULT_DELETE(_gpuBarrierManager)
  EZ_DEFAULT_DELETE(_gpuDescriptorSetHub)

  ezUInt32 backBufferCount = _gpuDevice->backBufferCount;
  for (ezUInt32 i = 0U; i < backBufferCount; i++)
  {
    _gpuRecycleBins[i]->clear();

    EZ_DEFAULT_DELETE(_gpuStagingBufferPools[i])
    EZ_DEFAULT_DELETE(_gpuRecycleBins[i])
    EZ_DEFAULT_DELETE(_gpuFencePools[i])
  }
  _gpuStagingBufferPools.Clear();
  _gpuRecycleBins.Clear();
  _gpuFencePools.Clear();

  if (_gpuDevice)
  {
    if (_gpuDevice->vkPipelineCache)
    {
      vkDestroyPipelineCache(_gpuDevice->vkDevice, _gpuDevice->vkPipelineCache, nullptr);
      _gpuDevice->vkPipelineCache = VK_NULL_HANDLE;
    }

    if (_gpuDevice->defaultBuffer.vkBuffer)
    {
      vmaDestroyBuffer(_gpuDevice->memoryAllocator, _gpuDevice->defaultBuffer.vkBuffer, _gpuDevice->defaultBuffer.vmaAllocation);
      _gpuDevice->defaultBuffer.vkBuffer = VK_NULL_HANDLE;
      _gpuDevice->defaultBuffer.vmaAllocation = VK_NULL_HANDLE;
    }
    if (_gpuDevice->defaultTextureView.vkImageView)
    {
      vkDestroyImageView(_gpuDevice->vkDevice, _gpuDevice->defaultTextureView.vkImageView, nullptr);
      _gpuDevice->defaultTextureView.vkImageView = VK_NULL_HANDLE;
    }
    if (_gpuDevice->defaultTexture.vkImage)
    {
      vmaDestroyImage(_gpuDevice->memoryAllocator, _gpuDevice->defaultTexture.vkImage, _gpuDevice->defaultTexture.vmaAllocation);
      _gpuDevice->defaultTexture.vkImage = VK_NULL_HANDLE;
      _gpuDevice->defaultTexture.vmaAllocation = VK_NULL_HANDLE;
    }
    cmdFuncCCVKDestroySampler(_gpuDevice, &_gpuDevice->defaultSampler);

    if (_gpuDevice->memoryAllocator != VK_NULL_HANDLE)
    {
      VmaStats stats;
      vmaCalculateStats(_gpuDevice->memoryAllocator, &stats);
      ezLog::Info("Total device memory leaked: {} bytes.", stats.total.usedBytes);
      EZ_ASSERT_DEV(!_memoryStatus.bufferSize, "Buffer memory leaked");
      EZ_ASSERT_DEV(!_memoryStatus.textureSize, "Texture memory leaked");

      vmaDestroyAllocator(_gpuDevice->memoryAllocator);
      _gpuDevice->memoryAllocator = VK_NULL_HANDLE;
    }

    for (auto& pool : _gpuDevice->_commandBufferPools)
    {
      EZ_DEFAULT_DELETE(pool.value);
    }

    //for (CCVKGPUDevice::CommandBufferPools::iterator it = _gpuDevice->_commandBufferPools.begin();
    //     it != _gpuDevice->_commandBufferPools.end(); ++it)
    //{
    //  EZ_DEFAULT_DELETE(it->value)
    //}
    _gpuDevice->_commandBufferPools.Clear();
    _gpuDevice->_descriptorSetPools.Clear();

    if (_gpuDevice->vkDevice != VK_NULL_HANDLE)
    {
      vkDestroyDevice(_gpuDevice->vkDevice, nullptr);
      _gpuDevice->vkDevice = VK_NULL_HANDLE;
    }

    EZ_DEFAULT_DELETE(_gpuDevice);
    _gpuDevice = nullptr;
  }

  EZ_DEFAULT_DELETE(_gpuContext)
}

namespace
{
  ezDynamicArray<VkSwapchainKHR> vkSwapchains;
  ezDynamicArray<ezUInt32> vkSwapchainIndices;
  ezDynamicArray<CCVKGPUSwapchain*> gpuSwapchains;
  ezDynamicArray<VkImageMemoryBarrier> vkAcquireBarriers;
  ezDynamicArray<VkImageMemoryBarrier> vkPresentBarriers;

  VkImageMemoryBarrier acquireBarrier{
    VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    nullptr,
    0,
    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    VK_QUEUE_FAMILY_IGNORED,
    VK_QUEUE_FAMILY_IGNORED,
    0,
    {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
  };
  VkImageMemoryBarrier presentBarrier{
    VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    nullptr,
    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    0,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    VK_QUEUE_FAMILY_IGNORED,
    VK_QUEUE_FAMILY_IGNORED,
    0,
    {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
  };
} // namespace

void CCVKDevice::acquire(Swapchain* const* swapchains, ezUInt32 count)
{
  auto* queue = static_cast<CCVKQueue*>(_queue);
  queue->gpuQueue()->lastSignaledSemaphores.Clear();
  vkSwapchainIndices.Clear();
  gpuSwapchains.Clear();
  vkSwapchains.Clear();
  vkAcquireBarriers.SetCount(count, acquireBarrier);
  vkPresentBarriers.SetCount(count, presentBarrier);

  for (ezUInt32 i = 0U; i < count; ++i)
  {
    auto* swapchain = static_cast<CCVKSwapchain*>(swapchains[i]);
    if (swapchain->gpuSwapchain()->lastPresentResult == VK_NOT_READY)
    {
      if (!swapchain->checkSwapchainStatus())
        continue;
    }
    vkSwapchains.PushBack(swapchain->gpuSwapchain()->vkSwapchain);
    gpuSwapchains.PushBack(swapchain->gpuSwapchain());
    vkSwapchainIndices.PushBack(swapchain->gpuSwapchain()->curImageIndex);
  }

  _gpuDescriptorSetHub->flush();
  _gpuSemaphorePool->reset();

  for (ezUInt32 i = 0; i < vkSwapchains.GetCount(); ++i)
  {
    VkSemaphore acquireSemaphore = _gpuSemaphorePool->alloc();
    VK_CHECK(vkAcquireNextImageKHR(_gpuDevice->vkDevice, vkSwapchains[i], ~0ULL,
      acquireSemaphore, VK_NULL_HANDLE, &vkSwapchainIndices[i]));
    gpuSwapchains[i]->curImageIndex = vkSwapchainIndices[i];
    queue->gpuQueue()->lastSignaledSemaphores.PushBack(acquireSemaphore);

    // swapchain indices may be different
    //CCASSERT(_gpuDevice->curBackBufferIndex == vkSwapchainIndices[i], "wrong image index?");

    vkAcquireBarriers[i].image = gpuSwapchains[i]->swapchainImages[vkSwapchainIndices[i]];
    vkPresentBarriers[i].image = gpuSwapchains[i]->swapchainImages[vkSwapchainIndices[i]];
  }

  _gpuTransportHub->checkIn(
    [&](const CCVKGPUCommandBuffer* gpuCommandBuffer)
    {
      vkCmdPipelineBarrier(gpuCommandBuffer->vkCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        0, 0, nullptr, 0, nullptr, vkSwapchains.GetCount(), vkAcquireBarriers.GetData());
    },
    false, false);

  _gpuTransportHub->checkIn(
    [&](const CCVKGPUCommandBuffer* gpuCommandBuffer)
    {
      vkCmdPipelineBarrier(gpuCommandBuffer->vkCommandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        0, 0, nullptr, 0, nullptr, vkSwapchains.GetCount(), vkPresentBarriers.GetData());
    },
    false, true);
}

void CCVKDevice::present()
{
  auto* queue = static_cast<CCVKQueue*>(_queue);
  _numDrawCalls = queue->_numDrawCalls;
  _numInstances = queue->_numInstances;
  _numTriangles = queue->_numTriangles;
  queue->_numDrawCalls = 0;
  queue->_numInstances = 0;
  queue->_numTriangles = 0;

  if (!_gpuTransportHub->empty(false))
    _gpuTransportHub->packageForFlight(false);
  if (!_gpuTransportHub->empty(true))
    _gpuTransportHub->packageForFlight(true);

  if (!vkSwapchains.IsEmpty())
  { // don't present if not acquired
    VkPresentInfoKHR presentInfo{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    presentInfo.waitSemaphoreCount = queue->gpuQueue()->lastSignaledSemaphores.GetCount();
    presentInfo.pWaitSemaphores = queue->gpuQueue()->lastSignaledSemaphores.GetData();
    presentInfo.swapchainCount = vkSwapchains.GetCount();
    presentInfo.pSwapchains = vkSwapchains.GetData();
    presentInfo.pImageIndices = vkSwapchainIndices.GetData();

    VkResult res = vkQueuePresentKHR(queue->gpuQueue()->vkQueue, &presentInfo);
    for (auto* gpuSwapchain : gpuSwapchains)
    {
      gpuSwapchain->lastPresentResult = res;
    }
  }

  _gpuDevice->curBackBufferIndex = (_gpuDevice->curBackBufferIndex + 1) % _gpuDevice->backBufferCount;

  ezUInt32 fenceCount = gpuFencePool()->size();
  if (fenceCount)
  {
    VK_CHECK(vkWaitForFences(_gpuDevice->vkDevice, fenceCount,
      gpuFencePool()->data(), VK_TRUE, DEFAULT_TIMEOUT));
  }

  gpuFencePool()->reset();
  gpuRecycleBin()->clear();
  gpuStagingBufferPool()->reset();
}

CCVKGPUFencePool* CCVKDevice::gpuFencePool()
{
  return _gpuFencePools[_gpuDevice->curBackBufferIndex];
}
CCVKGPURecycleBin* CCVKDevice::gpuRecycleBin()
{
  return _gpuRecycleBins[_gpuDevice->curBackBufferIndex];
}
CCVKGPUStagingBufferPool* CCVKDevice::gpuStagingBufferPool()
{
  return _gpuStagingBufferPools[_gpuDevice->curBackBufferIndex];
}

void CCVKDevice::waitAllFences()
{
  static ezDynamicArray<VkFence> fences;
  fences.Clear();

  for (auto* fencePool : _gpuFencePools)
  {
    //fences.insert(fences.end(), fencePool->data(), fencePool->data() + fencePool->size());
    for (ezUInt32 i = 0u; i < fencePool->size(); i++)
    {
      auto fence = fencePool->data() + i * (ezUInt32)sizeof(fencePool->data());
      fences.PushBack(*fence);
    }
  }

  if (!fences.IsEmpty())
  {
    VK_CHECK(vkWaitForFences(_gpuDevice->vkDevice, fences.GetCount(), fences.GetData(), VK_TRUE, DEFAULT_TIMEOUT));

    for (auto* fencePool : _gpuFencePools)
    {
      fencePool->reset();
    }
  }
}

CommandBuffer* CCVKDevice::createCommandBuffer(const CommandBufferInfo& /*info*/, bool /*hasAgent*/)
{
  return EZ_DEFAULT_NEW(CCVKCommandBuffer);
}

Queue* CCVKDevice::createQueue()
{
  return EZ_DEFAULT_NEW(CCVKQueue);
}

Swapchain* CCVKDevice::createSwapchain()
{
  return EZ_DEFAULT_NEW(CCVKSwapchain);
}

Buffer* CCVKDevice::createBuffer()
{
  return EZ_DEFAULT_NEW(CCVKBuffer);
}

Texture* CCVKDevice::createTexture()
{
  return EZ_DEFAULT_NEW(CCVKTexture);
}

Shader* CCVKDevice::createShader()
{
  return EZ_DEFAULT_NEW(CCVKShader);
}

InputAssembler* CCVKDevice::createInputAssembler()
{
  return EZ_DEFAULT_NEW(CCVKInputAssembler);
}

RenderPass* CCVKDevice::createRenderPass()
{
  return EZ_DEFAULT_NEW(CCVKRenderPass);
}

Framebuffer* CCVKDevice::createFramebuffer()
{
  return EZ_DEFAULT_NEW(CCVKFramebuffer);
}

DescriptorSet* CCVKDevice::createDescriptorSet()
{
  return EZ_DEFAULT_NEW(CCVKDescriptorSet);
}

DescriptorSetLayout* CCVKDevice::createDescriptorSetLayout()
{
  return EZ_DEFAULT_NEW(CCVKDescriptorSetLayout);
}

PipelineLayout* CCVKDevice::createPipelineLayout()
{
  return EZ_DEFAULT_NEW(CCVKPipelineLayout);
}

PipelineState* CCVKDevice::createPipelineState()
{
  return EZ_DEFAULT_NEW(CCVKPipelineState);
}

Sampler* CCVKDevice::createSampler(const SamplerInfo& info, ezUInt32 hash)
{
  return EZ_DEFAULT_NEW(CCVKSampler, info, hash);
}

GlobalBarrier* CCVKDevice::createGlobalBarrier(const GlobalBarrierInfo& info, ezUInt32 hash)
{
  return EZ_DEFAULT_NEW(CCVKGlobalBarrier, info, hash);
}

TextureBarrier* CCVKDevice::createTextureBarrier(const TextureBarrierInfo& info, ezUInt32 hash)
{
  return EZ_DEFAULT_NEW(CCVKTextureBarrier, info, hash);
}

void CCVKDevice::copyBuffersToTexture(const uint8_t* const* buffers, Texture* dst, const BufferTextureCopy* regions, ezUInt32 count)
{
  gpuTransportHub()->checkIn([this, buffers, dst, regions, count](CCVKGPUCommandBuffer* gpuCommandBuffer)
    { cmdFuncCCVKCopyBuffersToTexture(this, buffers, static_cast<CCVKTexture*>(dst)->gpuTexture(), regions, count, gpuCommandBuffer); });
}

void CCVKDevice::copyTextureToBuffers(Texture* srcTexture, uint8_t* const* buffers, const BufferTextureCopy* regions, ezUInt32 count)
{
  ezUInt32 totalSize = 0U;
  Format format = srcTexture->getFormat();
  ezDynamicArray<std::pair<ezUInt32, ezUInt32>> regionOffsetSizes;
  regionOffsetSizes.SetCount(count);
  for (size_t i = 0U; i < count; ++i)
  {
    const BufferTextureCopy& region = regions[i];
    ezUInt32 w = region.buffStride > 0 ? region.buffStride : region.texExtent.width;
    ezUInt32 h = region.buffTexHeight > 0 ? region.buffTexHeight : region.texExtent.height;
    ezUInt32 regionSize = FormatSize(format, w, h, region.texExtent.depth);
    regionOffsetSizes[i] = {totalSize, regionSize};
    totalSize += regionSize;
  }

  CCVKGPUBuffer stagingBuffer;
  stagingBuffer.size = totalSize;
  ezUInt32 texelSize = GFX_FORMAT_INFOS[ToNumber(format)].size;
  gpuStagingBufferPool()->alloc(&stagingBuffer, texelSize);

  // make sure the src texture is up-to-date
  waitAllFences();

  _gpuTransportHub->checkIn(
    [&](CCVKGPUCommandBuffer* cmdBuffer)
    {
      cmdFuncCCVKCopyTextureToBuffers(this, static_cast<CCVKTexture*>(srcTexture)->gpuTexture(), &stagingBuffer, regions, count, cmdBuffer);
    },
    true);

  for (ezUInt32 i = 0; i < count; ++i)
  {
    ezUInt32 regionOffset = 0;
    ezUInt32 regionSize = 0;
    std::tie(regionOffset, regionSize) = regionOffsetSizes[i];
    memcpy(buffers[i], stagingBuffer.mappedData + regionOffset, regionSize);
  }
}

//////////////////////////// Function Fallbacks /////////////////////////////////////////

static VkResult VKAPI_PTR vkCreateRenderPass2KHRFallback(
  VkDevice device,
  const VkRenderPassCreateInfo2* pCreateInfo,
  const VkAllocationCallbacks* pAllocator,
  VkRenderPass* pRenderPass)
{
  static ezDynamicArray<VkAttachmentDescription> attachmentDescriptions;
  static ezDynamicArray<VkSubpassDescription> subpassDescriptions;
  static ezDynamicArray<VkAttachmentReference> attachmentReferences;
  static ezDynamicArray<VkSubpassDependency> subpassDependencies;
  static ezDynamicArray<size_t> inputs;
  static ezDynamicArray<size_t> colors;
  static ezDynamicArray<size_t> resolves;
  static ezDynamicArray<size_t> depths;

  attachmentDescriptions.SetCount(pCreateInfo->attachmentCount);
  for (ezUInt32 i = 0; i < pCreateInfo->attachmentCount; ++i)
  {
    VkAttachmentDescription& desc{attachmentDescriptions[i]};
    const VkAttachmentDescription2& desc2{pCreateInfo->pAttachments[i]};
    desc.flags = desc2.flags;
    desc.format = desc2.format;
    desc.samples = desc2.samples;
    desc.loadOp = desc2.loadOp;
    desc.storeOp = desc2.storeOp;
    desc.stencilLoadOp = desc2.stencilLoadOp;
    desc.stencilStoreOp = desc2.stencilStoreOp;
    desc.initialLayout = desc2.initialLayout;
    desc.finalLayout = desc2.finalLayout;
  }

  subpassDescriptions.SetCount(pCreateInfo->subpassCount);
  attachmentReferences.Clear();

  inputs.Clear();
  inputs.SetCount(pCreateInfo->subpassCount, ezMath::MaxValue<ezUInt32>());

  colors.Clear();
  colors.SetCount(pCreateInfo->subpassCount, ezMath::MaxValue<ezUInt32>());

  resolves.Clear();
  resolves.SetCount(pCreateInfo->subpassCount, ezMath::MaxValue<ezUInt32>());

  depths.Clear();
  depths.SetCount(pCreateInfo->subpassCount, ezMath::MaxValue<ezUInt32>());

  for (ezUInt32 i = 0; i < pCreateInfo->subpassCount; ++i)
  {
    const VkSubpassDescription2& desc2{pCreateInfo->pSubpasses[i]};
    if (desc2.inputAttachmentCount)
    {
      inputs[i] = attachmentReferences.GetCount();
      for (ezUInt32 j = 0; j < desc2.inputAttachmentCount; ++j)
      {
        attachmentReferences.PushBack({desc2.pInputAttachments[j].attachment, desc2.pInputAttachments[j].layout});
      }
    }
    if (desc2.colorAttachmentCount)
    {
      colors[i] = attachmentReferences.GetCount();
      for (ezUInt32 j = 0; j < desc2.colorAttachmentCount; ++j)
      {
        attachmentReferences.PushBack({desc2.pColorAttachments[j].attachment, desc2.pColorAttachments[j].layout});
      }
      if (desc2.pResolveAttachments)
      {
        resolves[i] = attachmentReferences.GetCount();
        for (ezUInt32 j = 0; j < desc2.colorAttachmentCount; ++j)
        {
          attachmentReferences.PushBack({desc2.pResolveAttachments[j].attachment, desc2.pResolveAttachments[j].layout});
        }
      }
    }
    if (desc2.pDepthStencilAttachment)
    {
      depths[i] = attachmentReferences.GetCount();
      attachmentReferences.PushBack({desc2.pDepthStencilAttachment->attachment, desc2.pDepthStencilAttachment->layout});
    }
  }
  for (ezUInt32 i = 0; i < pCreateInfo->subpassCount; ++i)
  {
    VkSubpassDescription& desc{subpassDescriptions[i]};
    const VkSubpassDescription2& desc2{pCreateInfo->pSubpasses[i]};
    desc.flags = desc2.flags;
    desc.pipelineBindPoint = desc2.pipelineBindPoint;
    desc.inputAttachmentCount = desc2.inputAttachmentCount;
    desc.pInputAttachments = inputs[i] > attachmentReferences.GetCount() ? nullptr : &attachmentReferences[inputs[i]];
    desc.colorAttachmentCount = desc2.colorAttachmentCount;
    desc.pColorAttachments = colors[i] > attachmentReferences.GetCount() ? nullptr : &attachmentReferences[colors[i]];
    desc.pResolveAttachments = resolves[i] > attachmentReferences.GetCount() ? nullptr : &attachmentReferences[resolves[i]];
    desc.pDepthStencilAttachment = depths[i] > attachmentReferences.GetCount() ? nullptr : &attachmentReferences[depths[i]];
    desc.preserveAttachmentCount = desc2.preserveAttachmentCount;
    desc.pPreserveAttachments = desc2.pPreserveAttachments;
  }

  subpassDependencies.SetCount(pCreateInfo->dependencyCount);
  for (ezUInt32 i = 0; i < pCreateInfo->dependencyCount; ++i)
  {
    VkSubpassDependency& desc{subpassDependencies[i]};
    const VkSubpassDependency2& desc2{pCreateInfo->pDependencies[i]};
    desc.srcSubpass = desc2.srcSubpass;
    desc.dstSubpass = desc2.dstSubpass;
    desc.srcStageMask = desc2.srcStageMask;
    desc.dstStageMask = desc2.dstStageMask;
    desc.srcAccessMask = desc2.srcAccessMask;
    desc.dstAccessMask = desc2.dstAccessMask;
    desc.dependencyFlags = desc2.dependencyFlags;
  }

  VkRenderPassCreateInfo renderPassCreateInfo{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
  renderPassCreateInfo.attachmentCount = attachmentDescriptions.GetCount();
  renderPassCreateInfo.pAttachments = attachmentDescriptions.GetData();
  renderPassCreateInfo.subpassCount = subpassDescriptions.GetCount();
  renderPassCreateInfo.pSubpasses = subpassDescriptions.GetData();
  renderPassCreateInfo.dependencyCount = subpassDependencies.GetCount();
  renderPassCreateInfo.pDependencies = subpassDependencies.GetData();

  return vkCreateRenderPass(device, &renderPassCreateInfo, pAllocator, pRenderPass);
}
