#include <RHI/Vulkan/VKStd.h>

#include <RHI/Vulkan/VKCommands.h>
#include <RHI/Vulkan/VKDevice.h>
#include <RHI/Vulkan/VKGPUObjects.h>
#include <RHI/Base/SPIRVUtils.h>
#include <thread>

namespace
{
  constexpr bool ENABLE_LAZY_ALLOCATION = true;
} // namespace

CCVKGPUCommandBufferPool* CCVKGPUDevice::getCommandBufferPool()
{
  static thread_local size_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
  if (!_commandBufferPools.Contains(threadID))
  {
    _commandBufferPools[threadID] = EZ_DEFAULT_NEW(CCVKGPUCommandBufferPool, this);
  }
  return _commandBufferPools[threadID];
}

CCVKGPUDescriptorSetPool* CCVKGPUDevice::getDescriptorSetPool(ezUInt32 layoutID)
{
  return &_descriptorSetPools[layoutID];
}

void insertVkDynamicStates(ezDynamicArray<VkDynamicState>* out, const ezDynamicArray<DynamicStateFlagBit>& dynamicStates)
{
  for (DynamicStateFlagBit dynamicState : dynamicStates)
  {
    switch (dynamicState)
    {
      case DynamicStateFlagBit::LINE_WIDTH:
        out->PushBack(VK_DYNAMIC_STATE_LINE_WIDTH);
        break;
      case DynamicStateFlagBit::DEPTH_BIAS:
        out->PushBack(VK_DYNAMIC_STATE_DEPTH_BIAS);
        break;
      case DynamicStateFlagBit::BLEND_CONSTANTS:
        out->PushBack(VK_DYNAMIC_STATE_BLEND_CONSTANTS);
        break;
      case DynamicStateFlagBit::DEPTH_BOUNDS:
        out->PushBack(VK_DYNAMIC_STATE_DEPTH_BOUNDS);
        break;
      case DynamicStateFlagBit::STENCIL_WRITE_MASK:
        out->PushBack(VK_DYNAMIC_STATE_STENCIL_WRITE_MASK);
        break;
      case DynamicStateFlagBit::STENCIL_COMPARE_MASK:
        out->PushBack(VK_DYNAMIC_STATE_STENCIL_REFERENCE);
        out->PushBack(VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK);
        break;
      default:
      {
        EZ_ASSERT_DEV(false, "Unsupported DynamicStateFlagBit, convert to VkDynamicState failed.");
        break;
      }
    }
  }
}

void cmdFuncCCVKGetDeviceQueue(CCVKDevice* device, CCVKGPUQueue* gpuQueue)
{
  if (gpuQueue->possibleQueueFamilyIndices.IsEmpty())
  {
    ezUInt32 queueType = 0U;
    switch (gpuQueue->type)
    {
      case QueueType::GRAPHICS:
        queueType = VK_QUEUE_GRAPHICS_BIT;
        break;
      case QueueType::COMPUTE:
        queueType = VK_QUEUE_COMPUTE_BIT;
        break;
      case QueueType::TRANSFER:
        queueType = VK_QUEUE_TRANSFER_BIT;
        break;
    }

    const CCVKGPUContext* context = device->gpuContext();

    ezUInt32 queueCount = context->queueFamilyProperties.GetCount();
    for (ezUInt32 i = 0U; i < queueCount; ++i)
    {
      const VkQueueFamilyProperties& properties = context->queueFamilyProperties[i];
      if (properties.queueCount > 0 && (properties.queueFlags & queueType))
      {
        gpuQueue->possibleQueueFamilyIndices.PushBack(i);
      }
    }
  }

  vkGetDeviceQueue(device->gpuDevice()->vkDevice, gpuQueue->possibleQueueFamilyIndices[0], 0, &gpuQueue->vkQueue);
  gpuQueue->queueFamilyIndex = gpuQueue->possibleQueueFamilyIndices[0];
}

void cmdFuncCCVKCreateTexture(CCVKDevice* device, CCVKGPUTexture* gpuTexture)
{
  if (!gpuTexture->size)
    return;

  gpuTexture->aspectMask = mapVkImageAspectFlags(gpuTexture->format);
  // storage images has to be in general layout
  if (HasFlag(gpuTexture->usage, TextureUsageBit::STORAGE))
    gpuTexture->flags |= TextureFlagBit::GENERAL_LAYOUT;
  // remove stencil aspect for depth textures with sampled usage
  if (HasFlag(gpuTexture->usage, TextureUsageBit::SAMPLED))
    gpuTexture->aspectMask &= ~VK_IMAGE_ASPECT_STENCIL_BIT;

  auto createFn = [device, gpuTexture](VkImage* pVkImage, VmaAllocation* pVmaAllocation)
  {
    VkFormat vkFormat = mapVkFormat(gpuTexture->format, device->gpuDevice());
    VkFormatFeatureFlags features = mapVkFormatFeatureFlags(gpuTexture->usage);
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(device->gpuContext()->physicalDevice, vkFormat, &formatProperties);
    if (!(formatProperties.optimalTilingFeatures & features))
    {
      const char* formatName = GFX_FORMAT_INFOS[ToNumber(gpuTexture->format)].name.GetData();
      ezLog::Error("cmdFuncCCVKCreateTexture: The specified usage for {} is not supported on this platform", formatName);
      return;
    }

    VkImageUsageFlags usageFlags = mapVkImageUsageFlagBits(gpuTexture->usage);
    if (HasFlag(gpuTexture->flags, TextureFlags::GEN_MIPMAP))
    {
      usageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }

    VkImageCreateInfo createInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    createInfo.flags = mapVkImageCreateFlags(gpuTexture->type);
    createInfo.imageType = mapVkImageType(gpuTexture->type);
    createInfo.format = vkFormat;
    createInfo.extent = {gpuTexture->width, gpuTexture->height, gpuTexture->depth};
    createInfo.mipLevels = gpuTexture->mipLevels;
    createInfo.arrayLayers = gpuTexture->arrayLayers;
    createInfo.samples = device->gpuContext()->getSampleCountForAttachments(gpuTexture->format, vkFormat, gpuTexture->samples);
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.usage = usageFlags;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VmaAllocationInfo res;

    if (ENABLE_LAZY_ALLOCATION && HasAllFlags(TEXTURE_USAGE_TRANSIENT, gpuTexture->usage))
    {
      createInfo.usage = usageFlags | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
      allocInfo.usage = VMA_MEMORY_USAGE_GPU_LAZILY_ALLOCATED;
      VkResult result = vmaCreateImage(device->gpuDevice()->memoryAllocator, &createInfo, &allocInfo,
        pVkImage, pVmaAllocation, &res);
      if (!result)
      {
        gpuTexture->memoryless = true;
        return;
      }

      // feature not present, fallback
      createInfo.usage = usageFlags;
      allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    }

    gpuTexture->memoryless = false;
    VK_CHECK(vmaCreateImage(device->gpuDevice()->memoryAllocator, &createInfo, &allocInfo,
      pVkImage, pVmaAllocation, &res));
  };

  if (gpuTexture->swapchain)
  {
    ezUInt32 backBufferCount = device->gpuDevice()->backBufferCount;
    gpuTexture->swapchainVkImages.SetCount(backBufferCount);
    if (GFX_FORMAT_INFOS[ToNumber(gpuTexture->format)].hasDepth)
    {
      gpuTexture->swapchainVmaAllocations.SetCount(backBufferCount);
      for (ezUInt32 i = 0; i < backBufferCount; ++i)
      {
        createFn(&gpuTexture->swapchainVkImages[i], &gpuTexture->swapchainVmaAllocations[i]);
      }
    }
    else
    {
      for (ezUInt32 i = 0; i < backBufferCount; ++i)
      {
        gpuTexture->swapchainVkImages[i] = gpuTexture->swapchain->swapchainImages[i];
      }
    }
    gpuTexture->memoryless = true;
  }
  else
  {
    createFn(&gpuTexture->vkImage, &gpuTexture->vmaAllocation);
  }
}

void cmdFuncCCVKCreateTextureView(CCVKDevice* device, CCVKGPUTextureView* gpuTextureView)
{
  if (!gpuTextureView->gpuTexture)
    return;

  auto createFn = [device, gpuTextureView](VkImage vkImage, VkImageView* pVkImageView)
  {
    VkImageViewCreateInfo createInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    createInfo.image = vkImage;
    createInfo.viewType = mapVkImageViewType(gpuTextureView->type);
    createInfo.format = mapVkFormat(gpuTextureView->format, device->gpuDevice());
    createInfo.subresourceRange.aspectMask = gpuTextureView->gpuTexture->aspectMask;
    createInfo.subresourceRange.baseMipLevel = gpuTextureView->baseLevel;
    createInfo.subresourceRange.levelCount = gpuTextureView->levelCount;
    createInfo.subresourceRange.baseArrayLayer = gpuTextureView->baseLayer;
    createInfo.subresourceRange.layerCount = gpuTextureView->layerCount;

    VK_CHECK(vkCreateImageView(device->gpuDevice()->vkDevice, &createInfo, nullptr, pVkImageView));
  };

  if (gpuTextureView->gpuTexture->swapchain)
  {
    ezUInt32 backBufferCount = device->gpuDevice()->backBufferCount;
    gpuTextureView->swapchainVkImageViews.SetCount(backBufferCount);
    for (ezUInt32 i = 0; i < backBufferCount; ++i)
    {
      createFn(gpuTextureView->gpuTexture->swapchainVkImages[i], &gpuTextureView->swapchainVkImageViews[i]);
    }
  }
  else if (gpuTextureView->gpuTexture->vkImage)
  {
    createFn(gpuTextureView->gpuTexture->vkImage, &gpuTextureView->vkImageView);
  }
  else
  {
    return;
  }

  device->gpuDescriptorHub()->update(gpuTextureView);
}

void cmdFuncCCVKCreateSampler(CCVKDevice* device, CCVKGPUSampler* gpuSampler)
{
  VkSamplerCreateInfo createInfo{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
  CCVKGPUContext* context = device->gpuContext();
  float maxAnisotropy = context->physicalDeviceProperties.limits.maxSamplerAnisotropy;

  createInfo.magFilter = VK_FILTERS[ToNumber(gpuSampler->magFilter)];
  createInfo.minFilter = VK_FILTERS[ToNumber(gpuSampler->minFilter)];
  createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODES[ToNumber(gpuSampler->mipFilter)];
  createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODES[ToNumber(gpuSampler->addressU)];
  createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODES[ToNumber(gpuSampler->addressV)];
  createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODES[ToNumber(gpuSampler->addressW)];
  createInfo.mipLodBias = 0.F;
  createInfo.anisotropyEnable = gpuSampler->maxAnisotropy && context->physicalDeviceFeatures.samplerAnisotropy;
  createInfo.maxAnisotropy = ezMath::Min(maxAnisotropy, static_cast<float>(gpuSampler->maxAnisotropy));
  createInfo.compareEnable = gpuSampler->cmpFunc != ComparisonFunc::ALWAYS;
  createInfo.compareOp = VK_CMP_FUNCS[ToNumber(gpuSampler->cmpFunc)];
  // From UNASSIGNED-BestPractices-vkCreateSampler-lod-clamping:
  // Should use image views with baseMipLevel & levelCount in favor of this
  createInfo.minLod = 0.0;
  createInfo.maxLod = VK_LOD_CLAMP_NONE;

  VK_CHECK(vkCreateSampler(device->gpuDevice()->vkDevice, &createInfo, nullptr, &gpuSampler->vkSampler));
}

void cmdFuncCCVKCreateBuffer(CCVKDevice* device, CCVKGPUBuffer* gpuBuffer)
{
  if (!gpuBuffer->size)
  {
    return;
  }

  gpuBuffer->instanceSize = 0U;

  VkBufferCreateInfo bufferInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
  bufferInfo.size = gpuBuffer->size;
  bufferInfo.usage = mapVkBufferUsageFlagBits(gpuBuffer->usage);

  VmaAllocationCreateInfo allocInfo{};

  if (gpuBuffer->memUsage == MemoryUsage::HOST)
  {
    bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
  }
  else if (gpuBuffer->memUsage == MemoryUsage::DEVICE)
  {
    bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  }
  else if (gpuBuffer->memUsage == (MemoryUsage::HOST | MemoryUsage::DEVICE))
  {
    gpuBuffer->instanceSize = roundUp(gpuBuffer->size, device->getCapabilities().uboOffsetAlignment);
    bufferInfo.size = gpuBuffer->instanceSize * device->gpuDevice()->backBufferCount;
    allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  }

  VmaAllocationInfo res;
  VK_CHECK(vmaCreateBuffer(device->gpuDevice()->memoryAllocator, &bufferInfo, &allocInfo,
    &gpuBuffer->vkBuffer, &gpuBuffer->vmaAllocation, &res));

  gpuBuffer->mappedData = reinterpret_cast<ezUInt8*>(res.pMappedData);
  gpuBuffer->startOffset = 0; // we are creating one VkBuffer each for now

  // add special access types directly from usage
  if (HasFlag(gpuBuffer->usage, BufferUsageBit::VERTEX))
    gpuBuffer->renderAccessTypes.PushBack(THSVS_ACCESS_VERTEX_BUFFER);
  if (HasFlag(gpuBuffer->usage, BufferUsageBit::INDEX))
    gpuBuffer->renderAccessTypes.PushBack(THSVS_ACCESS_INDEX_BUFFER);
  if (HasFlag(gpuBuffer->usage, BufferUsageBit::INDIRECT))
    gpuBuffer->renderAccessTypes.PushBack(THSVS_ACCESS_INDIRECT_BUFFER);
}

struct AttachmentStatistics final
{
  enum class SubpassUsage
  {
    COLOR = 0x1,
    COLOR_RESOLVE = 0x2,
    DEPTH = 0x4,
    DEPTH_RESOLVE = 0x8,
    INPUT = 0x10,
  };
  struct SubpassRef final
  {
    VkImageLayout layout{VK_IMAGE_LAYOUT_UNDEFINED};
    SubpassUsage usage{SubpassUsage::COLOR};

    bool hasDepth() const { return usage == SubpassUsage::DEPTH || usage == SubpassUsage::DEPTH_RESOLVE; }
  };

  ezUInt32 loadSubpass{VK_SUBPASS_EXTERNAL};
  ezUInt32 storeSubpass{VK_SUBPASS_EXTERNAL};
  ezMap<ezUInt32, SubpassRef> records; // ordered

  void clear()
  {
    loadSubpass = VK_SUBPASS_EXTERNAL;
    storeSubpass = VK_SUBPASS_EXTERNAL;
    records.Clear();
  }
};
EZ_ENUM_BITWISE_OPERATORS(AttachmentStatistics::SubpassUsage)

struct SubpassDependencyManager final
{
  ezDynamicArray<VkSubpassDependency2> subpassDependencies;

  void clear()
  {
    subpassDependencies.Clear();
    _hashes.Clear();
  }

  void append(const VkSubpassDependency2& dependency)
  {
    ezUInt32 hash = 6U;
    hash ^= (dependency.srcSubpass) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    hash ^= (dependency.srcStageMask) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    hash ^= (dependency.srcAccessMask) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    hash ^= (dependency.dstSubpass) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    hash ^= (dependency.dstStageMask) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    hash ^= (dependency.dstAccessMask) + 0x9e3779b9 + (hash << 6) + (hash >> 2);

    if (_hashes.Contains(hash))
      return;
    subpassDependencies.PushBack(dependency);
    _hashes.Insert(hash);
  }

private:
  ezSet<ezUInt32> _hashes;
};

void cmdFuncCCVKCreateRenderPass(CCVKDevice* device, CCVKGPURenderPass* gpuRenderPass)
{
  static ezDynamicArray<VkSubpassDescriptionDepthStencilResolve> depthStencilResolves;
  static ezDynamicArray<VkAttachmentDescription2> attachmentDescriptions;
  static ezDynamicArray<VkAttachmentReference2> attachmentReferences;
  static ezDynamicArray<VkSubpassDescription2> subpassDescriptions;
  static ezDynamicArray<ThsvsAccessType> accessTypeCaches;
  static ezDynamicArray<CCVKAccessInfo> beginAccessInfos;
  static ezDynamicArray<CCVKAccessInfo> endAccessInfos;
  static ezDynamicArray<AttachmentStatistics> attachmentStatistics;
  static SubpassDependencyManager dependencyManager;

  const ezUInt32 colorAttachmentCount = gpuRenderPass->colorAttachments.GetCount();
  const ezUInt32 hasDepth = gpuRenderPass->depthStencilAttachment.format != Format::UNKNOWN ? 1 : 0;
  //attachmentDescriptions.assign(colorAttachmentCount + hasDepth, {VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2});
  {
    attachmentDescriptions.SetCount(colorAttachmentCount + hasDepth);
    for (auto& desc : attachmentDescriptions)
    {
      desc = {VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2};
    }
  }
  gpuRenderPass->clearValues.SetCount(colorAttachmentCount + hasDepth);
  gpuRenderPass->beginAccesses.SetCount(colorAttachmentCount + hasDepth);
  gpuRenderPass->endAccesses.SetCount(colorAttachmentCount + hasDepth);
  beginAccessInfos.SetCount(colorAttachmentCount + hasDepth);
  endAccessInfos.SetCount(colorAttachmentCount + hasDepth);

  for (ezUInt32 i = 0U; i < colorAttachmentCount; ++i)
  {
    const ColorAttachment& attachment = gpuRenderPass->colorAttachments[i];

    ezDynamicArray<ThsvsAccessType>& beginAccesses = gpuRenderPass->beginAccesses[i];
    ezDynamicArray<ThsvsAccessType>& endAccesses = gpuRenderPass->endAccesses[i];
    CCVKAccessInfo& beginAccessInfo = beginAccessInfos[i];
    CCVKAccessInfo& endAccessInfo = endAccessInfos[i];

    for (AccessType type : attachment.beginAccesses)
    {
      beginAccesses.PushBack(THSVS_ACCESS_TYPES[ToNumber(type)]);
    }
    for (AccessType type : attachment.endAccesses)
    {
      endAccesses.PushBack(THSVS_ACCESS_TYPES[ToNumber(type)]);
    }
    thsvsGetAccessInfo(beginAccesses.GetCount(), beginAccesses.GetData(), &beginAccessInfo.stageMask,
      &beginAccessInfo.accessMask, &beginAccessInfo.imageLayout, &beginAccessInfo.hasWriteAccess);
    thsvsGetAccessInfo(endAccesses.GetCount(), endAccesses.GetData(), &endAccessInfo.stageMask,
      &endAccessInfo.accessMask, &endAccessInfo.imageLayout, &endAccessInfo.hasWriteAccess);

    VkFormat vkFormat = mapVkFormat(attachment.format, device->gpuDevice());
    bool hasStencil = GFX_FORMAT_INFOS[ToNumber(attachment.format)].hasStencil;
    VkSampleCountFlagBits samples = device->gpuContext()->getSampleCountForAttachments(attachment.format, vkFormat, attachment.sampleCount);

    attachmentDescriptions[i].format = vkFormat;
    attachmentDescriptions[i].samples = samples;
    attachmentDescriptions[i].loadOp = mapVkLoadOp(attachment.loadOp);
    attachmentDescriptions[i].storeOp = mapVkStoreOp(attachment.storeOp);
    attachmentDescriptions[i].stencilLoadOp = hasStencil ? attachmentDescriptions[i].loadOp : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[i].stencilStoreOp = hasStencil ? attachmentDescriptions[i].storeOp : VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[i].initialLayout = attachment.loadOp == LoadOp::LOAD ? beginAccessInfo.imageLayout : VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescriptions[i].finalLayout = endAccessInfo.imageLayout;
  }
  if (hasDepth)
  {
    const DepthStencilAttachment& depthStencilAttachment = gpuRenderPass->depthStencilAttachment;

    ezDynamicArray<ThsvsAccessType>& beginAccesses = gpuRenderPass->beginAccesses[colorAttachmentCount];
    ezDynamicArray<ThsvsAccessType>& endAccesses = gpuRenderPass->endAccesses[colorAttachmentCount];
    CCVKAccessInfo& beginAccessInfo = beginAccessInfos[colorAttachmentCount];
    CCVKAccessInfo& endAccessInfo = endAccessInfos[colorAttachmentCount];

    for (AccessType type : depthStencilAttachment.beginAccesses)
    {
      beginAccesses.PushBack(THSVS_ACCESS_TYPES[ToNumber(type)]);
    }
    for (AccessType type : depthStencilAttachment.endAccesses)
    {
      endAccesses.PushBack(THSVS_ACCESS_TYPES[ToNumber(type)]);
    }
    thsvsGetAccessInfo(beginAccesses.GetCount(), beginAccesses.GetData(), &beginAccessInfo.stageMask,
      &beginAccessInfo.accessMask, &beginAccessInfo.imageLayout, &beginAccessInfo.hasWriteAccess);
    thsvsGetAccessInfo(endAccesses.GetCount(), endAccesses.GetData(), &endAccessInfo.stageMask,
      &endAccessInfo.accessMask, &endAccessInfo.imageLayout, &endAccessInfo.hasWriteAccess);

    VkFormat vkFormat = mapVkFormat(depthStencilAttachment.format, device->gpuDevice());
    bool hasStencil = GFX_FORMAT_INFOS[ToNumber(depthStencilAttachment.format)].hasStencil;
    VkSampleCountFlagBits samples = device->gpuContext()->getSampleCountForAttachments(depthStencilAttachment.format, vkFormat, depthStencilAttachment.sampleCount);

    attachmentDescriptions[colorAttachmentCount].format = vkFormat;
    attachmentDescriptions[colorAttachmentCount].samples = samples;
    attachmentDescriptions[colorAttachmentCount].loadOp = mapVkLoadOp(depthStencilAttachment.depthLoadOp);
    attachmentDescriptions[colorAttachmentCount].storeOp = mapVkStoreOp(depthStencilAttachment.depthStoreOp);
    attachmentDescriptions[colorAttachmentCount].stencilLoadOp = hasStencil ? mapVkLoadOp(depthStencilAttachment.stencilLoadOp) : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[colorAttachmentCount].stencilStoreOp = hasStencil ? mapVkStoreOp(depthStencilAttachment.stencilStoreOp) : VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[colorAttachmentCount].initialLayout = depthStencilAttachment.depthLoadOp == LoadOp::LOAD ? beginAccessInfo.imageLayout : VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescriptions[colorAttachmentCount].finalLayout = endAccessInfo.imageLayout;
  }

  ezUInt32 subpassCount = gpuRenderPass->subpasses.GetCount();
  attachmentReferences.Clear();
  gpuRenderPass->sampleCounts.Clear();

  for (const auto& subpassInfo : gpuRenderPass->subpasses)
  {
    VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;

    for (ezUInt32 input : subpassInfo.inputs)
    {
      VkImageLayout layout = gpuRenderPass->colorAttachments[input].IsGeneralLayout ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      attachmentReferences.PushBack({VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2, nullptr, input, layout, VK_IMAGE_ASPECT_COLOR_BIT});
    }
    for (ezUInt32 color : subpassInfo.colors)
    {
      const ColorAttachment& desc = gpuRenderPass->colorAttachments[color];
      const VkAttachmentDescription2& attachment = attachmentDescriptions[color];
      VkImageLayout layout = desc.IsGeneralLayout ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      attachmentReferences.PushBack({VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2, nullptr, color, layout, VK_IMAGE_ASPECT_COLOR_BIT});
      sampleCount = ezMath::Max(sampleCount, attachment.samples);
    }
    for (ezUInt32 resolve : subpassInfo.resolves)
    {
      VkImageLayout layout = gpuRenderPass->colorAttachments[resolve].IsGeneralLayout ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      attachmentReferences.PushBack({VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2, nullptr, resolve, layout, VK_IMAGE_ASPECT_COLOR_BIT});
    }

    if (subpassInfo.depthStencil != INVALID_BINDING)
    {
      Format dsFormat{Format::UNKNOWN};
      bool isGeneralLayout{false};
      if (subpassInfo.depthStencil >= colorAttachmentCount)
      {
        const DepthStencilAttachment& desc = gpuRenderPass->depthStencilAttachment;
        const VkAttachmentDescription2& attachment = attachmentDescriptions.PeekBack();
        isGeneralLayout = desc.IsGeneralLayout;
        dsFormat = desc.format;
        sampleCount = ezMath::Max(sampleCount, attachment.samples);
      }
      else
      {
        const ColorAttachment& desc = gpuRenderPass->colorAttachments[subpassInfo.depthStencil];
        const VkAttachmentDescription2& attachment = attachmentDescriptions[subpassInfo.depthStencil];
        isGeneralLayout = desc.IsGeneralLayout;
        dsFormat = desc.format;
        sampleCount = ezMath::Max(sampleCount, attachment.samples);
      }

      VkImageAspectFlags aspect = GFX_FORMAT_INFOS[ToNumber(dsFormat)].hasStencil
                                    ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT
                                    : VK_IMAGE_ASPECT_DEPTH_BIT;
      VkImageLayout layout = isGeneralLayout ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      attachmentReferences.PushBack({VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2, nullptr, subpassInfo.depthStencil, layout, aspect});
    }

    if (subpassInfo.depthStencilResolve != INVALID_BINDING)
    {
      Format dsFormat{Format::UNKNOWN};
      bool isGeneralLayout{false};
      if (subpassInfo.depthStencil >= colorAttachmentCount)
      {
        const DepthStencilAttachment& desc = gpuRenderPass->depthStencilAttachment;
        const VkAttachmentDescription2& attachment = attachmentDescriptions.PeekBack();
        isGeneralLayout = desc.IsGeneralLayout;
        dsFormat = desc.format;
        sampleCount = ezMath::Max(sampleCount, attachment.samples);
      }
      else
      {
        const ColorAttachment& desc = gpuRenderPass->colorAttachments[subpassInfo.depthStencil];
        const VkAttachmentDescription2& attachment = attachmentDescriptions[subpassInfo.depthStencil];
        isGeneralLayout = desc.IsGeneralLayout;
        dsFormat = desc.format;
        sampleCount = ezMath::Max(sampleCount, attachment.samples);
      }

      VkImageAspectFlags aspect = GFX_FORMAT_INFOS[ToNumber(dsFormat)].hasStencil
                                    ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT
                                    : VK_IMAGE_ASPECT_DEPTH_BIT;
      VkImageLayout layout = isGeneralLayout ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      attachmentReferences.PushBack({VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2, nullptr, subpassInfo.depthStencilResolve, layout, aspect});
    }

    gpuRenderPass->sampleCounts.PushBack(sampleCount);
  }

  ezUInt32 offset{0U};
  //subpassDescriptions.assign(subpassCount, {VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2}); // init to zeros first
  {
    subpassDescriptions.SetCount(subpassCount);
    for (ezUInt32 i = 0u; i < subpassCount; i++)
    {
      subpassDescriptions[i] = {VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2};
    }
  }
  depthStencilResolves.SetCount(subpassCount, {VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_DEPTH_STENCIL_RESOLVE});
  const VkPhysicalDeviceDepthStencilResolveProperties& prop{device->gpuContext()->physicalDeviceDepthStencilResolveProperties};
  for (ezUInt32 i = 0U; i < gpuRenderPass->subpasses.GetCount(); ++i)
  {
    const SubpassInfo subpassInfo = gpuRenderPass->subpasses[i];

    VkSubpassDescription2& desc = subpassDescriptions[i];
    desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    if (!subpassInfo.inputs.IsEmpty())
    {
      desc.inputAttachmentCount = subpassInfo.inputs.GetCount();
      desc.pInputAttachments = attachmentReferences.GetData() + offset;
      offset += subpassInfo.inputs.GetCount();
    }

    if (!subpassInfo.colors.IsEmpty())
    {
      desc.colorAttachmentCount = subpassInfo.colors.GetCount();
      desc.pColorAttachments = attachmentReferences.GetData() + offset;
      offset += subpassInfo.colors.GetCount();
      if (!subpassInfo.resolves.IsEmpty())
      {
        desc.pResolveAttachments = attachmentReferences.GetData() + offset;
        offset += subpassInfo.resolves.GetCount();
      }
    }

    if (!subpassInfo.preserves.IsEmpty())
    {
      desc.preserveAttachmentCount = subpassInfo.preserves.GetCount();
      desc.pPreserveAttachments = subpassInfo.preserves.GetData();
    }

    if (subpassInfo.depthStencil != INVALID_BINDING)
    {
      desc.pDepthStencilAttachment = attachmentReferences.GetData() + offset++;
    }
    else
    {
      desc.pDepthStencilAttachment = nullptr;
    }

    if (subpassInfo.depthStencilResolve != INVALID_BINDING)
    {
      VkSubpassDescriptionDepthStencilResolve& resolveDesc{depthStencilResolves[i]};

      VkResolveModeFlagBits depthResolveMode = VK_RESOLVE_MODES[ToNumber(subpassInfo.depthResolveMode)];
      VkResolveModeFlagBits stencilResolveMode = VK_RESOLVE_MODES[ToNumber(subpassInfo.stencilResolveMode)];

      if ((depthResolveMode & prop.supportedDepthResolveModes) == 0)
        depthResolveMode = VK_RESOLVE_MODE_SAMPLE_ZERO_BIT;
      if ((stencilResolveMode & prop.supportedStencilResolveModes) == 0)
        stencilResolveMode = VK_RESOLVE_MODE_SAMPLE_ZERO_BIT;

      if (!prop.independentResolveNone && stencilResolveMode != depthResolveMode)
      {
        stencilResolveMode = depthResolveMode;
      }
      else if (prop.independentResolveNone && !prop.independentResolve && stencilResolveMode &&
               depthResolveMode && stencilResolveMode != depthResolveMode)
      {
        stencilResolveMode = VK_RESOLVE_MODE_NONE;
      }

      resolveDesc.depthResolveMode = depthResolveMode;
      resolveDesc.stencilResolveMode = stencilResolveMode;
      resolveDesc.pDepthStencilResolveAttachment = attachmentReferences.GetData() + offset++;
      desc.pNext = &resolveDesc;
    }
  }

  ezUInt32 dependencyCount = gpuRenderPass->dependencies.GetCount();
  accessTypeCaches.Clear();
  dependencyManager.clear();

  if (dependencyCount)
  {
    for (const auto& dependency : gpuRenderPass->dependencies)
    {
      for (AccessType type : dependency.srcAccesses)
      {
        accessTypeCaches.PushBack(THSVS_ACCESS_TYPES[ToNumber(type)]);
      }
      for (AccessType type : dependency.dstAccesses)
      {
        accessTypeCaches.PushBack(THSVS_ACCESS_TYPES[ToNumber(type)]);
      }
    }

    offset = 0U;
    VkImageLayout imageLayout{VK_IMAGE_LAYOUT_UNDEFINED};
    bool hasWriteAccess{false};

    for (ezUInt32 i = 0U; i < dependencyCount; ++i)
    {
      const SubpassDependency& dependency = gpuRenderPass->dependencies[i];
      VkSubpassDependency2 vkDependency{VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2};
      vkDependency.srcSubpass = dependency.srcSubpass;
      vkDependency.dstSubpass = dependency.dstSubpass;
      vkDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

      thsvsGetAccessInfo(
        dependency.srcAccesses.GetCount(),
        accessTypeCaches.GetData() + offset,
        &vkDependency.srcStageMask,
        &vkDependency.srcAccessMask,
        &imageLayout, &hasWriteAccess);

      offset += dependency.srcAccesses.GetCount();

      thsvsGetAccessInfo(
        dependency.dstAccesses.GetCount(),
        accessTypeCaches.GetData() + offset,
        &vkDependency.dstStageMask,
        &vkDependency.dstAccessMask,
        &imageLayout, &hasWriteAccess);

      dependencyManager.append(vkDependency);

      offset += dependency.dstAccesses.GetCount();
    }
  }
  else
  {
    // try to deduce dependencies if not specified

    // first, gather necessary statistics for each attachment
    auto updateLifeCycle = [subpassCount](AttachmentStatistics& statistics, ezUInt32 index, VkImageLayout layout, AttachmentStatistics::SubpassUsage usage)
    {
      if (statistics.records.Contains(index))
      {
        EZ_ASSERT_DEV(statistics.records[index].layout == layout, "");
        statistics.records[index].usage |= usage;
      }
      else
      {
        statistics.records[index] = {layout, usage};
      }
      if (statistics.loadSubpass == VK_SUBPASS_EXTERNAL)
        statistics.loadSubpass = index;
      statistics.storeSubpass = index;
    };
    auto calculateLifeCycle = [&](ezUInt32 targetAttachment, AttachmentStatistics& statistics)
    {
      for (ezUInt32 j = 0U; j < subpassCount; ++j)
      {
        auto& subpass = subpassDescriptions[j];
        for (size_t k = 0U; k < subpass.colorAttachmentCount; ++k)
        {
          if (subpass.pColorAttachments[k].attachment == targetAttachment)
          {
            updateLifeCycle(statistics, j, subpass.pColorAttachments[k].layout, AttachmentStatistics::SubpassUsage::COLOR);
          }
          if (subpass.pResolveAttachments && subpass.pResolveAttachments[k].attachment == targetAttachment)
          {
            updateLifeCycle(statistics, j, subpass.pResolveAttachments[k].layout, AttachmentStatistics::SubpassUsage::COLOR_RESOLVE);
          }
        }
        for (ezUInt32 k = 0U; k < subpass.inputAttachmentCount; ++k)
        {
          if (subpass.pInputAttachments[k].attachment == targetAttachment)
          {
            updateLifeCycle(statistics, j, subpass.pInputAttachments[k].layout, AttachmentStatistics::SubpassUsage::INPUT);
          }
        }
        if (subpass.pDepthStencilAttachment && subpass.pDepthStencilAttachment->attachment == targetAttachment)
        {
          updateLifeCycle(statistics, j, subpass.pDepthStencilAttachment->layout, AttachmentStatistics::SubpassUsage::DEPTH);
        }
        if (depthStencilResolves[j].pDepthStencilResolveAttachment &&
            depthStencilResolves[j].pDepthStencilResolveAttachment->attachment == targetAttachment)
        {
          updateLifeCycle(statistics, j, depthStencilResolves[j].pDepthStencilResolveAttachment->layout, AttachmentStatistics::SubpassUsage::DEPTH_RESOLVE);
        }
      }
    };
    attachmentStatistics.SetCount(colorAttachmentCount + hasDepth);
    for (ezUInt32 i = 0U; i < colorAttachmentCount + hasDepth; ++i)
    {
      attachmentStatistics[i].clear();
      calculateLifeCycle(i, attachmentStatistics[i]);
      EZ_ASSERT_DEV(attachmentStatistics[i].loadSubpass != VK_SUBPASS_EXTERNAL &&
                      attachmentStatistics[i].storeSubpass != VK_SUBPASS_EXTERNAL,
        "");
    }

    // wait for resources to become available (begin accesses)
    auto beginDependencyCheck = [](VkSubpassDependency2& dependency, ezUInt32 attachment, const AttachmentStatistics::SubpassRef& ref)
    {
      const VkAttachmentDescription2& desc = attachmentDescriptions[attachment];
      const CCVKAccessInfo& info = beginAccessInfos[attachment];
      if (desc.initialLayout != ref.layout || info.hasWriteAccess || desc.loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR)
      {
        VkPipelineStageFlagBits dstStage{ref.hasDepth() ? VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT : VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkAccessFlagBits dstAccessRead{ref.hasDepth() ? VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT : VK_ACCESS_COLOR_ATTACHMENT_READ_BIT};
        VkAccessFlagBits dstAccessWrite{ref.hasDepth() ? VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT : VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT};
        dependency.srcStageMask |= info.stageMask;
        dependency.dstStageMask |= dstStage;
        dependency.srcAccessMask |= info.hasWriteAccess ? info.accessMask : 0;
        dependency.dstAccessMask |= (desc.loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR ? dstAccessWrite : dstAccessRead);
        return true;
      }
      return false;
    };
    VkSubpassDependency2 beginDependency;
    ezUInt32 lastLoadSubpass{VK_SUBPASS_EXTERNAL};
    bool beginDependencyValid{false};
    for (ezUInt32 i = 0U; i < colorAttachmentCount + hasDepth; ++i)
    {
      auto& statistics = attachmentStatistics[i];
      if (lastLoadSubpass != statistics.loadSubpass)
      {
        if (beginDependencyValid)
          dependencyManager.append(beginDependency);
        beginDependency = {VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2, nullptr,
          VK_SUBPASS_EXTERNAL, statistics.loadSubpass,
          VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT};
        lastLoadSubpass = statistics.loadSubpass;
        beginDependencyValid = false;
      }
      beginDependencyValid |= beginDependencyCheck(beginDependency, i, statistics.records[statistics.loadSubpass]);
    }
    if (beginDependencyValid)
      dependencyManager.append(beginDependency);

    // make rendering result visible (end accesses)
    auto endDependencyCheck = [](VkSubpassDependency2& dependency, ezUInt32 attachment, const AttachmentStatistics::SubpassRef& ref)
    {
      const VkAttachmentDescription2& desc = attachmentDescriptions[attachment];
      const CCVKAccessInfo& info = endAccessInfos[attachment];
      if (desc.initialLayout != ref.layout || info.hasWriteAccess || desc.storeOp == VK_ATTACHMENT_STORE_OP_STORE)
      {
        VkPipelineStageFlagBits srcStage{ref.hasDepth() ? VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT : VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkAccessFlagBits srcAccess{ref.hasDepth() ? VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT : VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT};
        dependency.srcStageMask |= srcStage;
        dependency.srcAccessMask |= srcAccess;
        dependency.dstStageMask |= info.stageMask;
        dependency.dstAccessMask |= info.accessMask;
        return true;
      }
      return false;
    };
    VkSubpassDependency2 endDependency;
    ezUInt32 lastStoreSubpass{VK_SUBPASS_EXTERNAL};
    bool endDependencyValid{false};
    for (ezUInt32 i = 0U; i < colorAttachmentCount + hasDepth; ++i)
    {
      auto& statistics = attachmentStatistics[i];
      if (lastStoreSubpass != statistics.storeSubpass)
      {
        if (endDependencyValid)
          dependencyManager.append(endDependency);
        endDependency = {VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2, nullptr,
          statistics.storeSubpass, VK_SUBPASS_EXTERNAL,
          VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT};
        lastStoreSubpass = statistics.storeSubpass;
        endDependencyValid = false;
      }
      endDependencyValid |= endDependencyCheck(endDependency, i, statistics.records[statistics.storeSubpass]);
    }
    if (endDependencyValid)
      dependencyManager.append(endDependency);

    // other transitioning dependencies
    auto mapAccessFlags = [](AttachmentStatistics::SubpassUsage usage)
    {
      // there may be more kind of dependencies
      if (HasFlag(usage, AttachmentStatistics::SubpassUsage::INPUT))
      {
        return std::make_pair(VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_INPUT_ATTACHMENT_READ_BIT);
      }
      return std::make_pair(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    };
    auto genDependency = [&](ezUInt32 srcIdx, AttachmentStatistics::SubpassUsage srcUsage,
                           ezUInt32 dstIdx, AttachmentStatistics::SubpassUsage dstUsage)
    {
      VkSubpassDependency2 dependency{VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2, nullptr, srcIdx, dstIdx};
      std::tie(dependency.srcStageMask, dependency.srcAccessMask) = mapAccessFlags(srcUsage);
      std::tie(dependency.dstStageMask, dependency.dstAccessMask) = mapAccessFlags(dstUsage);
      dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
      return dependency;
    };
    for (ezUInt32 i = 0U; i < colorAttachmentCount + hasDepth; ++i)
    {
      auto& statistics{attachmentStatistics[i]};

      const AttachmentStatistics::SubpassRef* prevRef{nullptr};
      ezUInt32 prevIdx{0U};
      for (const auto& it : statistics.records)
      {
        if (prevRef && prevRef->usage != it.Value().usage)
        {
          dependencyManager.append(genDependency(prevIdx, prevRef->usage, it.Key(), it.Value().usage));
        }
        prevIdx = it.Key();
        prevRef = &it.Value();
      }
    }
  }

  VkRenderPassCreateInfo2 renderPassCreateInfo{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2};
  renderPassCreateInfo.attachmentCount = attachmentDescriptions.GetCount();
  renderPassCreateInfo.pAttachments = attachmentDescriptions.GetData();
  renderPassCreateInfo.subpassCount = subpassDescriptions.GetCount();
  renderPassCreateInfo.pSubpasses = subpassDescriptions.GetData();
  renderPassCreateInfo.dependencyCount = dependencyManager.subpassDependencies.GetCount();
  renderPassCreateInfo.pDependencies = dependencyManager.subpassDependencies.GetData();

  VK_CHECK(device->gpuDevice()->createRenderPass2(device->gpuDevice()->vkDevice, &renderPassCreateInfo,
    nullptr, &gpuRenderPass->vkRenderPass));
}

void cmdFuncCCVKCreateFramebuffer(CCVKDevice* device, CCVKGPUFramebuffer* gpuFramebuffer)
{
  ezUInt32 colorViewCount = gpuFramebuffer->gpuColorViews.GetCount();
  bool hasDepth = gpuFramebuffer->gpuRenderPass->depthStencilAttachment.format != Format::UNKNOWN;
  ezDynamicArray<VkImageView> attachments;
  attachments.SetCount(colorViewCount + hasDepth);
  VkFramebufferCreateInfo createInfo{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
  createInfo.width = createInfo.height = UINT_MAX;

  ezUInt32 swapchainImageIndices = 0;
  for (ezUInt32 i = 0U; i < colorViewCount; ++i)
  {
    CCVKGPUTextureView* texView = gpuFramebuffer->gpuColorViews[i];
    if (texView->gpuTexture->swapchain)
    {
      gpuFramebuffer->swapchain = texView->gpuTexture->swapchain;
      swapchainImageIndices |= (1 << i);
    }
    else
    {
      attachments[i] = gpuFramebuffer->gpuColorViews[i]->vkImageView;
    }
    createInfo.width = ezMath::Min(createInfo.width, gpuFramebuffer->gpuColorViews[i]->gpuTexture->width);
    createInfo.height = ezMath::Min(createInfo.height, gpuFramebuffer->gpuColorViews[i]->gpuTexture->height);
  }
  if (hasDepth)
  {
    if (gpuFramebuffer->gpuDepthStencilView->gpuTexture->swapchain)
    {
      gpuFramebuffer->swapchain = gpuFramebuffer->gpuDepthStencilView->gpuTexture->swapchain;
      swapchainImageIndices |= (1 << colorViewCount);
    }
    else
    {
      attachments[colorViewCount] = gpuFramebuffer->gpuDepthStencilView->vkImageView;
    }
    createInfo.width = ezMath::Min(createInfo.width, gpuFramebuffer->gpuDepthStencilView->gpuTexture->width);
    createInfo.height = ezMath::Min(createInfo.height, gpuFramebuffer->gpuDepthStencilView->gpuTexture->height);
  }
  gpuFramebuffer->isOffscreen = !swapchainImageIndices;

  if (gpuFramebuffer->isOffscreen)
  {
    CCVKGPUTextureView* gpuTextureView{colorViewCount ? gpuFramebuffer->gpuColorViews[0] : gpuFramebuffer->gpuDepthStencilView};
    createInfo.renderPass = gpuFramebuffer->gpuRenderPass->vkRenderPass;
    createInfo.attachmentCount = attachments.GetCount();
    createInfo.pAttachments = attachments.GetData();
    createInfo.layers = 1;
    VK_CHECK(vkCreateFramebuffer(device->gpuDevice()->vkDevice, &createInfo, nullptr, &gpuFramebuffer->vkFramebuffer));
  }
  else
  {
    // swapchain-related framebuffers need special treatments: rebuild is needed
    // whenever a user-specified attachment or swapchain itself is changed

    FramebufferListMap& fboListMap = gpuFramebuffer->swapchain->vkSwapchainFramebufferListMap;
    auto fboListMapIter = fboListMap.Find(gpuFramebuffer);
    if (fboListMapIter != ezInvalidIndex && !fboListMap[fboListMapIter].IsEmpty())
    {
      return;
    }
    ezUInt32 swapchainImageCount = gpuFramebuffer->swapchain->swapchainImages.GetCount();
    if (fboListMapIter != ezInvalidIndex)
    {
      fboListMap[fboListMapIter].SetCount(swapchainImageCount);
    }
    else
    {
      // eztodo:
      //fboListMap.emplace(std::piecewise_construct, std::forward_as_tuple(gpuFramebuffer), std::forward_as_tuple(swapchainImageCount));
      ezDynamicArray<VkFramebuffer> frameBufferList;
      //frameBufferList.PushBack({(VkAccessFlags)swapchainImageCount});
      fboListMap.Insert(gpuFramebuffer, std::move(frameBufferList));
    }
    createInfo.renderPass = gpuFramebuffer->gpuRenderPass->vkRenderPass;
    createInfo.attachmentCount = attachments.GetCount();
    createInfo.pAttachments = attachments.GetData();
    createInfo.layers = 1;
    for (ezUInt32 i = 0U; i < swapchainImageCount; ++i)
    {
      for (ezUInt32 j = 0U; j < colorViewCount; ++j)
      {
        if (swapchainImageIndices & (1 << j))
        {
          attachments[j] = gpuFramebuffer->gpuColorViews[j]->swapchainVkImageViews[i];
        }
      }
      if (swapchainImageIndices & (1 << colorViewCount))
      {
        attachments[colorViewCount] = gpuFramebuffer->gpuDepthStencilView->swapchainVkImageViews[i];
      }
      VK_CHECK(vkCreateFramebuffer(device->gpuDevice()->vkDevice, &createInfo, nullptr, &fboListMap[gpuFramebuffer][i]));
    }
  }
}

void cmdFuncCCVKCreateShader(CCVKDevice* device, CCVKGPUShader* gpuShader)
{
  SPIRVUtils* spirv = SPIRVUtils::getInstance();

  for (CCVKGPUShaderStage& stage : gpuShader->gpuStages)
  {
    spirv->compileGLSL(stage.type, ezStringBuilder("#version 450\n", stage.source));
    if (stage.type == ShaderStageFlagBit::VERTEX)
      spirv->compressInputLocations(gpuShader->attributes);

    VkShaderModuleCreateInfo createInfo{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    createInfo.codeSize = spirv->getOutputSize();
    createInfo.pCode = spirv->getOutputData();
    VK_CHECK(vkCreateShaderModule(device->gpuDevice()->vkDevice, &createInfo, nullptr, &stage.vkShader));
  }

  ezLog::Info("Shader '{}' compilation succeeded.", gpuShader->name.GetData());
}

void cmdFuncCCVKCreateDescriptorSetLayout(CCVKDevice* device, CCVKGPUDescriptorSetLayout* gpuDescriptorSetLayout)
{
  CCVKGPUDevice* gpuDevice = device->gpuDevice();
  ezUInt32 bindingCount = gpuDescriptorSetLayout->bindings.GetCount();

  gpuDescriptorSetLayout->vkBindings.SetCount(bindingCount);
  for (ezUInt32 i = 0U; i < bindingCount; ++i)
  {
    const DescriptorSetLayoutBinding& binding = gpuDescriptorSetLayout->bindings[i];
    VkDescriptorSetLayoutBinding& vkBinding = gpuDescriptorSetLayout->vkBindings[i];
    vkBinding.stageFlags = mapVkShaderStageFlags(binding.stageFlags);
    vkBinding.descriptorType = mapVkDescriptorType(binding.descriptorType);
    vkBinding.binding = binding.binding;
    vkBinding.descriptorCount = binding.count;
  }

  VkDescriptorSetLayoutCreateInfo setCreateInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
  setCreateInfo.bindingCount = bindingCount;
  setCreateInfo.pBindings = gpuDescriptorSetLayout->vkBindings.GetData();
  VK_CHECK(vkCreateDescriptorSetLayout(gpuDevice->vkDevice, &setCreateInfo, nullptr, &gpuDescriptorSetLayout->vkDescriptorSetLayout));

  CCVKGPUDescriptorSetPool* pool = gpuDevice->getDescriptorSetPool(gpuDescriptorSetLayout->id);
  pool->link(gpuDevice, gpuDescriptorSetLayout->maxSetsPerPool, gpuDescriptorSetLayout->vkBindings, gpuDescriptorSetLayout->vkDescriptorSetLayout);

  gpuDescriptorSetLayout->defaultDescriptorSet = pool->request(0);

  if (gpuDevice->useDescriptorUpdateTemplate && bindingCount)
  {
    const ezDynamicArray<VkDescriptorSetLayoutBinding>& bindings = gpuDescriptorSetLayout->vkBindings;

    ezDynamicArray<VkDescriptorUpdateTemplateEntry> entries;
    entries.SetCount(bindingCount);
    for (ezUInt32 j = 0U, k = 0U; j < bindingCount; ++j)
    {
      const VkDescriptorSetLayoutBinding& binding = bindings[j];
      if (binding.descriptorType != VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT)
      {
        entries[j].dstBinding = binding.binding;
        entries[j].dstArrayElement = 0;
        entries[j].descriptorCount = binding.descriptorCount;
        entries[j].descriptorType = binding.descriptorType;
        entries[j].offset = sizeof(CCVKDescriptorInfo) * k;
        entries[j].stride = sizeof(CCVKDescriptorInfo);
        k += binding.descriptorCount;
      }
    }

    VkDescriptorUpdateTemplateCreateInfo createInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO};
    createInfo.descriptorUpdateEntryCount = bindingCount;
    createInfo.pDescriptorUpdateEntries = entries.GetData();
    createInfo.templateType = VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_DESCRIPTOR_SET;
    createInfo.descriptorSetLayout = gpuDescriptorSetLayout->vkDescriptorSetLayout;
    if (gpuDevice->minorVersion > 0)
    {
      VK_CHECK(vkCreateDescriptorUpdateTemplate(gpuDevice->vkDevice, &createInfo, nullptr, &gpuDescriptorSetLayout->vkDescriptorUpdateTemplate));
    }
    else
    {
      VK_CHECK(vkCreateDescriptorUpdateTemplateKHR(gpuDevice->vkDevice, &createInfo, nullptr, &gpuDescriptorSetLayout->vkDescriptorUpdateTemplate));
    }
  }
}

void cmdFuncCCVKCreatePipelineLayout(CCVKDevice* device, CCVKGPUPipelineLayout* gpuPipelineLayout)
{
  CCVKGPUDevice* gpuDevice = device->gpuDevice();
  ezUInt32 layoutCount = gpuPipelineLayout->setLayouts.GetCount();

  ezDynamicArray<VkDescriptorSetLayout> descriptorSetLayouts;
  descriptorSetLayouts.SetCount(layoutCount);
  for (ezUInt32 i = 0; i < layoutCount; ++i)
  {
    descriptorSetLayouts[i] = gpuPipelineLayout->setLayouts[i]->vkDescriptorSetLayout;
  }

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
  pipelineLayoutCreateInfo.setLayoutCount = layoutCount;
  pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.GetData();
  VK_CHECK(vkCreatePipelineLayout(gpuDevice->vkDevice, &pipelineLayoutCreateInfo, nullptr, &gpuPipelineLayout->vkPipelineLayout));
}

void cmdFuncCCVKCreateComputePipelineState(CCVKDevice* device, CCVKGPUPipelineState* gpuPipelineState)
{
  VkComputePipelineCreateInfo createInfo{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};

  ///////////////////// Shader Stage /////////////////////

  const auto& stages = gpuPipelineState->gpuShader->gpuStages;
  VkPipelineShaderStageCreateInfo stageInfo{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
  stageInfo.stage = mapVkShaderStageFlagBits(stages[0].type);
  stageInfo.module = stages[0].vkShader;
  stageInfo.pName = "main";

  createInfo.stage = stageInfo;
  createInfo.layout = gpuPipelineState->gpuPipelineLayout->vkPipelineLayout;

  ///////////////////// Creation /////////////////////

  VK_CHECK(vkCreateComputePipelines(device->gpuDevice()->vkDevice, device->gpuDevice()->vkPipelineCache,
    1, &createInfo, nullptr, &gpuPipelineState->vkPipeline));
}

void cmdFuncCCVKCreateGraphicsPipelineState(CCVKDevice* device, CCVKGPUPipelineState* gpuPipelineState)
{
  static ezDynamicArray<VkPipelineShaderStageCreateInfo> stageInfos;
  static ezDynamicArray<VkVertexInputBindingDescription> bindingDescriptions;
  static ezDynamicArray<VkVertexInputAttributeDescription> attributeDescriptions;
  static ezDynamicArray<ezUInt32> offsets;
  static ezDynamicArray<VkDynamicState> dynamicStates;
  static ezDynamicArray<VkPipelineColorBlendAttachmentState> blendTargets;

  VkGraphicsPipelineCreateInfo createInfo{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};

  ///////////////////// Shader Stage /////////////////////

  const auto& stages = gpuPipelineState->gpuShader->gpuStages;
  const ezUInt32 stageCount = stages.GetCount();

  stageInfos.SetCount(stageCount, {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO});

  for (ezUInt32 i = 0U; i < stageCount; ++i)
  {
    stageInfos[i].stage = mapVkShaderStageFlagBits(stages[i].type);
    stageInfos[i].module = stages[i].vkShader;
    stageInfos[i].pName = "main";
  }
  createInfo.stageCount = stageCount;
  createInfo.pStages = stageInfos.GetData();

  ///////////////////// Input State /////////////////////

  const AttributeList& attributes = gpuPipelineState->inputState.attributes;
  const ezUInt32 attributeCount = attributes.GetCount();
  ezUInt32 bindingCount = 1U;
  for (ezUInt32 i = 0U; i < attributeCount; ++i)
  {
    const Attribute& attr = attributes[i];
    bindingCount = ezMath::Max(bindingCount, attr.stream + 1);
  }

  bindingDescriptions.SetCount(bindingCount);
  for (ezUInt32 i = 0U; i < bindingCount; ++i)
  {
    bindingDescriptions[i].binding = i;
    bindingDescriptions[i].stride = 0;
    bindingDescriptions[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  }
  for (ezUInt32 i = 0U; i < attributeCount; ++i)
  {
    const Attribute& attr = attributes[i];
    bindingDescriptions[attr.stream].stride += GFX_FORMAT_INFOS[ToNumber(attr.format)].size;
    if (attr.isInstanced)
    {
      bindingDescriptions[attr.stream].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
    }
  }

  const AttributeList& shaderAttrs = gpuPipelineState->gpuShader->attributes;
  const ezUInt32 shaderAttrCount = shaderAttrs.GetCount();

  attributeDescriptions.SetCount(shaderAttrCount);
  for (ezUInt32 i = 0; i < shaderAttrCount; ++i)
  {
    bool attributeFound = false;
    //offsets.assign(bindingCount, 0);
    {
      offsets.SetCount(bindingCount);
      for (ezUInt32 j = 0; j < bindingCount; j++)
      {
        offsets[j] = 0u;
      }
    }
    for (const Attribute& attr : attributes)
    {
      if (shaderAttrs[i].name == attr.name)
      {
        attributeDescriptions[i].location = shaderAttrs[i].location;
        attributeDescriptions[i].binding = attr.stream;
        attributeDescriptions[i].format = mapVkFormat(attr.format, device->gpuDevice());
        attributeDescriptions[i].offset = offsets[attr.stream];
        attributeFound = true;
        break;
      }
      offsets[attr.stream] += GFX_FORMAT_INFOS[ToNumber(attr.format)].size;
    }
    if (!attributeFound)
    { // handle absent attribute
      attributeDescriptions[i].location = shaderAttrs[i].location;
      attributeDescriptions[i].binding = 0;
      attributeDescriptions[i].format = mapVkFormat(shaderAttrs[i].format, device->gpuDevice());
      attributeDescriptions[i].offset = 0; // reuse the first attribute as dummy data
    }
  }

  VkPipelineVertexInputStateCreateInfo vertexInput{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
  vertexInput.vertexBindingDescriptionCount = bindingCount;
  vertexInput.pVertexBindingDescriptions = bindingDescriptions.GetData();
  vertexInput.vertexAttributeDescriptionCount = shaderAttrCount;
  vertexInput.pVertexAttributeDescriptions = attributeDescriptions.GetData();
  createInfo.pVertexInputState = &vertexInput;

  ///////////////////// Input Asembly State /////////////////////

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
  inputAssembly.topology = VK_PRIMITIVE_MODES[ToNumber(gpuPipelineState->primitive)];
  createInfo.pInputAssemblyState = &inputAssembly;

  ///////////////////// Dynamic State /////////////////////

  //dynamicStates.assign({VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR});
  {
    dynamicStates.SetCount(0);
    dynamicStates.PushBack(VK_DYNAMIC_STATE_VIEWPORT);
    dynamicStates.PushBack(VK_DYNAMIC_STATE_SCISSOR);
  }
  insertVkDynamicStates(&dynamicStates, gpuPipelineState->dynamicStates);

  VkPipelineDynamicStateCreateInfo dynamicState{VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
  dynamicState.dynamicStateCount = dynamicStates.GetCount();
  dynamicState.pDynamicStates = dynamicStates.GetData();
  createInfo.pDynamicState = &dynamicState;

  ///////////////////// Viewport State /////////////////////

  VkPipelineViewportStateCreateInfo viewportState{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
  viewportState.viewportCount = 1; // dynamic by default
  viewportState.scissorCount = 1;  // dynamic by default
  createInfo.pViewportState = &viewportState;

  ///////////////////// Rasterization State /////////////////////

  VkPipelineRasterizationStateCreateInfo rasterizationState{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};

  //rasterizationState.depthClampEnable;
  rasterizationState.rasterizerDiscardEnable = gpuPipelineState->rs.isDiscard;
  rasterizationState.polygonMode = VK_POLYGON_MODES[ToNumber(gpuPipelineState->rs.polygonMode)];
  rasterizationState.cullMode = VK_CULL_MODES[ToNumber(gpuPipelineState->rs.cullMode)];
  rasterizationState.frontFace = gpuPipelineState->rs.isFrontFaceCCW ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
  rasterizationState.depthBiasEnable = gpuPipelineState->rs.depthBiasEnabled;
  rasterizationState.depthBiasConstantFactor = gpuPipelineState->rs.depthBias;
  rasterizationState.depthBiasClamp = gpuPipelineState->rs.depthBiasClamp;
  rasterizationState.depthBiasSlopeFactor = gpuPipelineState->rs.depthBiasSlop;
  rasterizationState.lineWidth = gpuPipelineState->rs.lineWidth;
  createInfo.pRasterizationState = &rasterizationState;

  ///////////////////// Multisample State /////////////////////

  VkPipelineMultisampleStateCreateInfo multisampleState{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
  multisampleState.rasterizationSamples = gpuPipelineState->gpuRenderPass->sampleCounts[gpuPipelineState->subpass];
  multisampleState.alphaToCoverageEnable = gpuPipelineState->bs.isA2C;
  //multisampleState.sampleShadingEnable;
  //multisampleState.minSampleShading;
  //multisampleState.pSampleMask;
  //multisampleState.alphaToOneEnable;
  createInfo.pMultisampleState = &multisampleState;

  ///////////////////// Depth Stencil State /////////////////////

  VkPipelineDepthStencilStateCreateInfo depthStencilState = {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
  depthStencilState.depthTestEnable = gpuPipelineState->dss.depthTest;
  depthStencilState.depthWriteEnable = gpuPipelineState->dss.depthWrite;
  depthStencilState.depthCompareOp = VK_CMP_FUNCS[ToNumber(gpuPipelineState->dss.depthFunc)];
  depthStencilState.stencilTestEnable = gpuPipelineState->dss.stencilTestFront;

  depthStencilState.front = {
    VK_STENCIL_OPS[ToNumber(gpuPipelineState->dss.stencilFailOpFront)],
    VK_STENCIL_OPS[ToNumber(gpuPipelineState->dss.stencilPassOpFront)],
    VK_STENCIL_OPS[ToNumber(gpuPipelineState->dss.stencilZFailOpFront)],
    VK_CMP_FUNCS[ToNumber(gpuPipelineState->dss.stencilFuncFront)],
    gpuPipelineState->dss.stencilReadMaskFront,
    gpuPipelineState->dss.stencilWriteMaskFront,
    gpuPipelineState->dss.stencilRefFront,
  };
  depthStencilState.back = {
    VK_STENCIL_OPS[ToNumber(gpuPipelineState->dss.stencilFailOpBack)],
    VK_STENCIL_OPS[ToNumber(gpuPipelineState->dss.stencilPassOpBack)],
    VK_STENCIL_OPS[ToNumber(gpuPipelineState->dss.stencilZFailOpBack)],
    VK_CMP_FUNCS[ToNumber(gpuPipelineState->dss.stencilFuncBack)],
    gpuPipelineState->dss.stencilReadMaskBack,
    gpuPipelineState->dss.stencilWriteMaskBack,
    gpuPipelineState->dss.stencilRefBack,
  };
  //depthStencilState.depthBoundsTestEnable;
  //depthStencilState.minDepthBounds;
  //depthStencilState.maxDepthBounds;
  createInfo.pDepthStencilState = &depthStencilState;

  ///////////////////// Blend State /////////////////////

  ezUInt32 blendTargetCount = gpuPipelineState->gpuRenderPass->subpasses[gpuPipelineState->subpass].colors.GetCount();
  blendTargets.SetCount(blendTargetCount, {});

  for (ezUInt32 i = 0U; i < blendTargetCount; ++i)
  {
    BlendTarget& target = i >= gpuPipelineState->bs.targets.GetCount()
                            ? gpuPipelineState->bs.targets[0]
                            : gpuPipelineState->bs.targets[i];

    blendTargets[i].blendEnable = target.blend;
    blendTargets[i].srcColorBlendFactor = VK_BLEND_FACTORS[ToNumber(target.blendSrc)];
    blendTargets[i].dstColorBlendFactor = VK_BLEND_FACTORS[ToNumber(target.blendDst)];
    blendTargets[i].colorBlendOp = VK_BLEND_OPS[ToNumber(target.blendEq)];
    blendTargets[i].srcAlphaBlendFactor = VK_BLEND_FACTORS[ToNumber(target.blendSrcAlpha)];
    blendTargets[i].dstAlphaBlendFactor = VK_BLEND_FACTORS[ToNumber(target.blendDstAlpha)];
    blendTargets[i].alphaBlendOp = VK_BLEND_OPS[ToNumber(target.blendAlphaEq)];
    blendTargets[i].colorWriteMask = mapVkColorComponentFlags(target.blendColorMask);
  }
  Color& blendColor = gpuPipelineState->bs.blendColor;

  VkPipelineColorBlendStateCreateInfo colorBlendState{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
  //colorBlendState.logicOpEnable;
  //colorBlendState.logicOp;
  colorBlendState.attachmentCount = blendTargetCount;
  colorBlendState.pAttachments = blendTargets.GetData();
  colorBlendState.blendConstants[0] = blendColor.x;
  colorBlendState.blendConstants[1] = blendColor.y;
  colorBlendState.blendConstants[2] = blendColor.z;
  colorBlendState.blendConstants[3] = blendColor.w;
  createInfo.pColorBlendState = &colorBlendState;

  ///////////////////// References /////////////////////

  createInfo.layout = gpuPipelineState->gpuPipelineLayout->vkPipelineLayout;
  createInfo.renderPass = gpuPipelineState->gpuRenderPass->vkRenderPass;
  createInfo.subpass = gpuPipelineState->subpass;

  ///////////////////// Creation /////////////////////

  VK_CHECK(vkCreateGraphicsPipelines(device->gpuDevice()->vkDevice, device->gpuDevice()->vkPipelineCache,
    1, &createInfo, nullptr, &gpuPipelineState->vkPipeline));
}

void cmdFuncCCVKUpdateBuffer(CCVKDevice* device, CCVKGPUBuffer* gpuBuffer, const void* buffer, ezUInt32 size, const CCVKGPUCommandBuffer* cmdBuffer)
{
  if (!gpuBuffer)
    return;

  const void* dataToUpload = nullptr;
  ezUInt32 sizeToUpload = 0U;

  if (HasFlag(gpuBuffer->usage, BufferUsageBit::INDIRECT))
  {
    ezUInt32 drawInfoCount = size / sizeof(DrawInfo);
    const auto* drawInfo = static_cast<const DrawInfo*>(buffer);
    if (drawInfoCount > 0)
    {
      if (drawInfo->indexCount)
      {
        for (ezUInt32 i = 0u; i < drawInfoCount; ++i)
        {
          gpuBuffer->indexedIndirectCmds[i].indexCount = drawInfo->indexCount;
          gpuBuffer->indexedIndirectCmds[i].instanceCount = ezMath::Max(drawInfo->instanceCount, 1U);
          gpuBuffer->indexedIndirectCmds[i].firstIndex = drawInfo->firstIndex;
          gpuBuffer->indexedIndirectCmds[i].vertexOffset = drawInfo->vertexOffset;
          gpuBuffer->indexedIndirectCmds[i].firstInstance = drawInfo->firstInstance;
          drawInfo++;
        }
        dataToUpload = gpuBuffer->indexedIndirectCmds.GetData();
        sizeToUpload = drawInfoCount * sizeof(VkDrawIndexedIndirectCommand);
        gpuBuffer->isDrawIndirectByIndex = true;
      }
      else
      {
        for (ezUInt32 i = 0; i < drawInfoCount; ++i)
        {
          gpuBuffer->indirectCmds[i].vertexCount = drawInfo->vertexCount;
          gpuBuffer->indirectCmds[i].instanceCount = drawInfo->instanceCount;
          gpuBuffer->indirectCmds[i].firstVertex = drawInfo->firstVertex;
          gpuBuffer->indirectCmds[i].firstInstance = drawInfo->firstInstance;
          drawInfo++;
        }
        dataToUpload = gpuBuffer->indirectCmds.GetData();
        sizeToUpload = drawInfoCount * sizeof(VkDrawIndirectCommand);
        gpuBuffer->isDrawIndirectByIndex = false;
      }
    }
  }
  else
  {
    dataToUpload = buffer;
    sizeToUpload = size;
  }

  // back buffer instances update command
  ezUInt32 backBufferIndex = device->gpuDevice()->curBackBufferIndex;
  if (gpuBuffer->instanceSize)
  {
    device->gpuBufferHub()->record(gpuBuffer, backBufferIndex, sizeToUpload, !cmdBuffer);
    if (!cmdBuffer)
    {
      ezUInt8* dst = gpuBuffer->mappedData + backBufferIndex * gpuBuffer->instanceSize;
      memcpy(dst, dataToUpload, sizeToUpload);
      return;
    }
  }

  CCVKGPUBuffer stagingBuffer;
  stagingBuffer.size = sizeToUpload;
  device->gpuStagingBufferPool()->alloc(&stagingBuffer);
  memcpy(stagingBuffer.mappedData, dataToUpload, sizeToUpload);

  VkBufferCopy region{
    stagingBuffer.startOffset,
    gpuBuffer->getStartOffset(backBufferIndex),
    sizeToUpload,
  };
  auto upload = [&stagingBuffer, &gpuBuffer, &region](const CCVKGPUCommandBuffer* gpuCommandBuffer)
  {
#if BARRIER_DEDUCTION_LEVEL >= BARRIER_DEDUCTION_LEVEL_BASIC
    if (gpuBuffer->transferAccess)
    {
      // guard against WAW hazard
      VkMemoryBarrier vkBarrier{VK_STRUCTURE_TYPE_MEMORY_BARRIER};
      vkBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      vkBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      vkCmdPipelineBarrier(gpuCommandBuffer->vkCommandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0, 1, &vkBarrier, 0, nullptr, 0, nullptr);
    }
#endif
    vkCmdCopyBuffer(gpuCommandBuffer->vkCommandBuffer, stagingBuffer.vkBuffer, gpuBuffer->vkBuffer, 1, &region);
  };

  if (cmdBuffer)
  {
    upload(cmdBuffer);
  }
  else
  {
    device->gpuTransportHub()->checkIn(upload);
  }

  gpuBuffer->transferAccess = THSVS_ACCESS_TRANSFER_WRITE;
  device->gpuBarrierManager()->checkIn(gpuBuffer);
}

void cmdFuncCCVKCopyBuffersToTexture(CCVKDevice* device, const ezUInt8* const* buffers, CCVKGPUTexture* gpuTexture,
  const BufferTextureCopy* regions, ezUInt32 count, const CCVKGPUCommandBuffer* gpuCommandBuffer)
{
  ezDynamicArray<ThsvsAccessType>& curTypes = gpuTexture->currentAccessTypes;

  ThsvsImageBarrier barrier{};
  barrier.image = gpuTexture->vkImage;
  barrier.discardContents = false;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
  barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
  barrier.subresourceRange.aspectMask = gpuTexture->aspectMask;
  barrier.prevAccessCount = curTypes.GetCount();
  barrier.pPrevAccesses = curTypes.GetData();
  barrier.nextAccessCount = 1;
  barrier.pNextAccesses = &THSVS_ACCESS_TYPES[ToNumber(AccessType::TRANSFER_WRITE)];

  if (gpuTexture->transferAccess != THSVS_ACCESS_TRANSFER_WRITE)
  {
    cmdFuncCCVKImageMemoryBarrier(gpuCommandBuffer, barrier);
  }
  else
  {
    // guard against WAW hazard
    VkMemoryBarrier vkBarrier{VK_STRUCTURE_TYPE_MEMORY_BARRIER};
    vkBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    vkBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    vkCmdPipelineBarrier(gpuCommandBuffer->vkCommandBuffer,
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      0, 1, &vkBarrier, 0, nullptr, 0, nullptr);
  }

  ezUInt32 totalSize = 0U;
  ezDynamicArray<ezUInt32> regionSizes;
  regionSizes.SetCount(count);
  for (ezUInt32 i = 0U; i < count; ++i)
  {
    const BufferTextureCopy& region = regions[i];
    ezUInt32 w = region.buffStride > 0 ? region.buffStride : region.texExtent.width;
    ezUInt32 h = region.buffTexHeight > 0 ? region.buffTexHeight : region.texExtent.height;
    totalSize += regionSizes[i] = FormatSize(gpuTexture->format, w, h, region.texExtent.depth);
  }

  CCVKGPUBuffer stagingBuffer;
  stagingBuffer.size = totalSize;
  ezUInt32 texelSize = GFX_FORMAT_INFOS[ToNumber(gpuTexture->format)].size;
  device->gpuStagingBufferPool()->alloc(&stagingBuffer, texelSize);

  ezDynamicArray<VkBufferImageCopy> stagingRegions;
  stagingRegions.SetCount(count);
  VkDeviceSize offset = 0;
  for (ezUInt32 i = 0U; i < count; ++i)
  {
    const BufferTextureCopy& region = regions[i];
    VkBufferImageCopy& stagingRegion = stagingRegions[i];
    stagingRegion.bufferOffset = stagingBuffer.startOffset + offset;
    stagingRegion.bufferRowLength = region.buffStride;
    stagingRegion.bufferImageHeight = region.buffTexHeight;
    stagingRegion.imageSubresource = {gpuTexture->aspectMask, region.texSubres.mipLevel, region.texSubres.baseArrayLayer, region.texSubres.layerCount};
    stagingRegion.imageOffset = {region.texOffset.x, region.texOffset.y, region.texOffset.z};
    stagingRegion.imageExtent = {region.texExtent.width, region.texExtent.height, region.texExtent.depth};

    memcpy(stagingBuffer.mappedData + offset, buffers[i], regionSizes[i]);
    offset += regionSizes[i];
  }

  vkCmdCopyBufferToImage(gpuCommandBuffer->vkCommandBuffer, stagingBuffer.vkBuffer, gpuTexture->vkImage,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, stagingRegions.GetCount(), stagingRegions.GetData());

  if (HasFlag(gpuTexture->flags, TextureFlags::GEN_MIPMAP))
  {
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(device->gpuContext()->physicalDevice, mapVkFormat(gpuTexture->format, device->gpuDevice()), &formatProperties);
    VkFormatFeatureFlags mipmapFeatures = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

    if (formatProperties.optimalTilingFeatures & mipmapFeatures)
    {
      int width = static_cast<int>(gpuTexture->width);
      int height = static_cast<int>(gpuTexture->height);

      VkImageBlit blitInfo{};
      blitInfo.srcSubresource.aspectMask = gpuTexture->aspectMask;
      blitInfo.srcSubresource.layerCount = gpuTexture->arrayLayers;
      blitInfo.dstSubresource.aspectMask = gpuTexture->aspectMask;
      blitInfo.dstSubresource.layerCount = gpuTexture->arrayLayers;
      blitInfo.srcOffsets[1] = {width, height, 1};
      blitInfo.dstOffsets[1] = {ezMath::Max(width >> 1, 1), ezMath::Max(height >> 1, 1), 1};
      barrier.subresourceRange.levelCount = 1;
      barrier.prevAccessCount = 1;
      barrier.pPrevAccesses = &THSVS_ACCESS_TYPES[ToNumber(AccessType::TRANSFER_WRITE)];
      barrier.pNextAccesses = &THSVS_ACCESS_TYPES[ToNumber(AccessType::TRANSFER_READ)];

      for (ezUInt32 i = 1U; i < gpuTexture->mipLevels; ++i)
      {
        barrier.subresourceRange.baseMipLevel = i - 1;
        cmdFuncCCVKImageMemoryBarrier(gpuCommandBuffer, barrier);

        blitInfo.srcSubresource.mipLevel = i - 1;
        blitInfo.dstSubresource.mipLevel = i;
        vkCmdBlitImage(gpuCommandBuffer->vkCommandBuffer, gpuTexture->vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
          gpuTexture->vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitInfo, VK_FILTER_LINEAR);

        const int32_t w = blitInfo.srcOffsets[1].x = blitInfo.dstOffsets[1].x;
        const int32_t h = blitInfo.srcOffsets[1].y = blitInfo.dstOffsets[1].y;
        blitInfo.dstOffsets[1].x = ezMath::Max(w >> 1, 1);
        blitInfo.dstOffsets[1].y = ezMath::Max(h >> 1, 1);
      }

      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = gpuTexture->mipLevels - 1;
      barrier.pPrevAccesses = &THSVS_ACCESS_TYPES[ToNumber(AccessType::TRANSFER_READ)];
      barrier.pNextAccesses = &THSVS_ACCESS_TYPES[ToNumber(AccessType::TRANSFER_WRITE)];

      cmdFuncCCVKImageMemoryBarrier(gpuCommandBuffer, barrier);
    }
    else
    {
      const char* formatName = GFX_FORMAT_INFOS[ToNumber(gpuTexture->format)].name.GetData();
      ezLog::Warning("cmdFuncCCVKCopyBuffersToTexture: generate mipmap for {} is not supported on this platform", formatName);
    }
  }

  //curTypes.assign({THSVS_ACCESS_TRANSFER_WRITE});
  {
    curTypes.SetCount(0);
    curTypes.PushBack(THSVS_ACCESS_TRANSFER_WRITE);
  }
  gpuTexture->transferAccess = THSVS_ACCESS_TRANSFER_WRITE;
  device->gpuBarrierManager()->checkIn(gpuTexture);
}

EZ_VULKAN_API void cmdFuncCCVKCopyTextureToBuffers(CCVKDevice* device, CCVKGPUTexture* srcTexture, CCVKGPUBuffer* destBuffer,
  const BufferTextureCopy* regions, ezUInt32 count, const CCVKGPUCommandBuffer* gpuCommandBuffer)
{
  ezDynamicArray<ThsvsAccessType>& curTypes = srcTexture->currentAccessTypes;

  ThsvsImageBarrier barrier{};
  barrier.image = srcTexture->vkImage;
  barrier.discardContents = false;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
  barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
  barrier.subresourceRange.aspectMask = srcTexture->aspectMask;
  barrier.prevAccessCount = curTypes.GetCount();
  barrier.pPrevAccesses = curTypes.GetData();
  barrier.nextAccessCount = 1;
  barrier.pNextAccesses = &THSVS_ACCESS_TYPES[static_cast<ezUInt32>(AccessType::TRANSFER_READ)];

  if (srcTexture->transferAccess != THSVS_ACCESS_TRANSFER_READ)
  {
    cmdFuncCCVKImageMemoryBarrier(gpuCommandBuffer, barrier);
  }

  ezDynamicArray<VkBufferImageCopy> stagingRegions;
  stagingRegions.SetCount(count);
  VkDeviceSize offset = 0;
  for (ezUInt32 i = 0U; i < count; ++i)
  {
    const BufferTextureCopy& region = regions[i];
    VkBufferImageCopy& stagingRegion = stagingRegions[i];
    stagingRegion.bufferOffset = destBuffer->startOffset + offset;
    stagingRegion.bufferRowLength = region.buffStride;
    stagingRegion.bufferImageHeight = region.buffTexHeight;
    stagingRegion.imageSubresource = {srcTexture->aspectMask, region.texSubres.mipLevel, region.texSubres.baseArrayLayer, region.texSubres.layerCount};
    stagingRegion.imageOffset = {region.texOffset.x, region.texOffset.y, region.texOffset.z};
    stagingRegion.imageExtent = {region.texExtent.width, region.texExtent.height, region.texExtent.depth};

    ezUInt32 w = region.buffStride > 0 ? region.buffStride : region.texExtent.width;
    ezUInt32 h = region.buffTexHeight > 0 ? region.buffTexHeight : region.texExtent.height;
    ezUInt32 regionSize = FormatSize(srcTexture->format, w, h, region.texExtent.depth);

    offset += regionSize;
  }
  vkCmdCopyImageToBuffer(gpuCommandBuffer->vkCommandBuffer, srcTexture->vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
    destBuffer->vkBuffer, stagingRegions.GetCount(), stagingRegions.GetData());

  //curTypes.assign({THSVS_ACCESS_TRANSFER_READ});
  {
    curTypes.SetCount(0);
    curTypes.PushBack(THSVS_ACCESS_TRANSFER_READ);
  }
  srcTexture->transferAccess = THSVS_ACCESS_TRANSFER_READ;
  device->gpuBarrierManager()->checkIn(srcTexture);
}

void cmdFuncCCVKDestroyRenderPass(CCVKGPUDevice* gpuDevice, CCVKGPURenderPass* gpuRenderPass)
{
  gpuRenderPass->beginAccesses.Clear();
  gpuRenderPass->endAccesses.Clear();

  if (gpuRenderPass->vkRenderPass != VK_NULL_HANDLE)
  {
    vkDestroyRenderPass(gpuDevice->vkDevice, gpuRenderPass->vkRenderPass, nullptr);
    gpuRenderPass->vkRenderPass = VK_NULL_HANDLE;
  }
}

void cmdFuncCCVKDestroySampler(CCVKGPUDevice* gpuDevice, CCVKGPUSampler* gpuSampler)
{
  if (gpuSampler->vkSampler != VK_NULL_HANDLE)
  {
    vkDestroySampler(gpuDevice->vkDevice, gpuSampler->vkSampler, nullptr);
    gpuSampler->vkSampler = VK_NULL_HANDLE;
  }
}

void cmdFuncCCVKDestroyShader(CCVKGPUDevice* gpuDevice, CCVKGPUShader* gpuShader)
{
  for (CCVKGPUShaderStage& stage : gpuShader->gpuStages)
  {
    vkDestroyShaderModule(gpuDevice->vkDevice, stage.vkShader, nullptr);
    stage.vkShader = VK_NULL_HANDLE;
  }
}

void cmdFuncCCVKDestroyFramebuffer(CCVKGPUDevice* gpuDevice, CCVKGPUFramebuffer* gpuFramebuffer)
{
  if (gpuFramebuffer->isOffscreen)
  {
    if (gpuFramebuffer->vkFramebuffer != VK_NULL_HANDLE)
    {
      vkDestroyFramebuffer(gpuDevice->vkDevice, gpuFramebuffer->vkFramebuffer, nullptr);
      gpuFramebuffer->vkFramebuffer = VK_NULL_HANDLE;
    }
  }
  else if (gpuDevice->swapchains.Contains(gpuFramebuffer->swapchain))
  {
    FramebufferListMap& fboListMap = gpuFramebuffer->swapchain->vkSwapchainFramebufferListMap;
    auto fboListMapIter = fboListMap.Find(gpuFramebuffer);
    if (fboListMapIter != ezInvalidIndex)
    {
      for (auto& i : fboListMap[fboListMapIter])
      {
        vkDestroyFramebuffer(gpuDevice->vkDevice, i, nullptr);
      }
      fboListMap[fboListMapIter].Clear();
      fboListMap.RemoveAtAndCopy(fboListMapIter);
    }
  }
}

void cmdFuncCCVKDestroyDescriptorSetLayout(CCVKGPUDevice* gpuDevice, CCVKGPUDescriptorSetLayout* gpuDescriptorSetLayout)
{
  if (gpuDescriptorSetLayout->defaultDescriptorSet != VK_NULL_HANDLE)
  {
    gpuDevice->getDescriptorSetPool(gpuDescriptorSetLayout->id)->yield(gpuDescriptorSetLayout->defaultDescriptorSet, 0);
    gpuDescriptorSetLayout->defaultDescriptorSet = VK_NULL_HANDLE;
  }

  if (gpuDescriptorSetLayout->vkDescriptorUpdateTemplate != VK_NULL_HANDLE)
  {
    if (gpuDevice->minorVersion > 0)
    {
      vkDestroyDescriptorUpdateTemplate(gpuDevice->vkDevice, gpuDescriptorSetLayout->vkDescriptorUpdateTemplate, nullptr);
    }
    else
    {
      vkDestroyDescriptorUpdateTemplateKHR(gpuDevice->vkDevice, gpuDescriptorSetLayout->vkDescriptorUpdateTemplate, nullptr);
    }
    gpuDescriptorSetLayout->vkDescriptorUpdateTemplate = VK_NULL_HANDLE;
  }

  if (gpuDescriptorSetLayout->vkDescriptorSetLayout != VK_NULL_HANDLE)
  {
    vkDestroyDescriptorSetLayout(gpuDevice->vkDevice, gpuDescriptorSetLayout->vkDescriptorSetLayout, nullptr);
    gpuDescriptorSetLayout->vkDescriptorSetLayout = VK_NULL_HANDLE;
  }
}

void cmdFuncCCVKDestroyPipelineLayout(CCVKGPUDevice* gpuDevice, CCVKGPUPipelineLayout* gpuPipelineLayout)
{
  if (gpuPipelineLayout->vkPipelineLayout != VK_NULL_HANDLE)
  {
    vkDestroyPipelineLayout(gpuDevice->vkDevice, gpuPipelineLayout->vkPipelineLayout, nullptr);
    gpuPipelineLayout->vkPipelineLayout = VK_NULL_HANDLE;
  }
}

void cmdFuncCCVKDestroyPipelineState(CCVKGPUDevice* gpuDevice, CCVKGPUPipelineState* gpuPipelineState)
{
  if (gpuPipelineState->vkPipeline != VK_NULL_HANDLE)
  {
    vkDestroyPipeline(gpuDevice->vkDevice, gpuPipelineState->vkPipeline, nullptr);
    gpuPipelineState->vkPipeline = VK_NULL_HANDLE;
  }
}

void cmdFuncCCVKImageMemoryBarrier(const CCVKGPUCommandBuffer* gpuCommandBuffer, const ThsvsImageBarrier& imageBarrier)
{
  VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
  VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  VkPipelineStageFlags tempSrcStageMask = 0;
  VkPipelineStageFlags tempDstStageMask = 0;
  VkImageMemoryBarrier vkBarrier;
  thsvsGetVulkanImageMemoryBarrier(imageBarrier, &tempSrcStageMask, &tempDstStageMask, &vkBarrier);
  srcStageMask |= tempSrcStageMask;
  dstStageMask |= tempDstStageMask;
  vkCmdPipelineBarrier(gpuCommandBuffer->vkCommandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &vkBarrier);
}

void CCVKGPURecycleBin::clear()
{
  for (ezUInt32 i = 0U; i < _count; ++i)
  {
    Resource& res = _resources[i];
    switch (res.type)
    {
      case RecycledType::BUFFER:
        if (res.buffer.vkBuffer)
        {
          vmaDestroyBuffer(_device->memoryAllocator, res.buffer.vkBuffer, res.buffer.vmaAllocation);
          res.buffer.vkBuffer = VK_NULL_HANDLE;
          res.buffer.vmaAllocation = VK_NULL_HANDLE;
        }
        break;
      case RecycledType::TEXTURE:
        if (res.image.vkImage)
        {
          vmaDestroyImage(_device->memoryAllocator, res.image.vkImage, res.image.vmaAllocation);
          res.image.vkImage = VK_NULL_HANDLE;
          res.image.vmaAllocation = VK_NULL_HANDLE;
        }
        break;
      case RecycledType::TEXTURE_VIEW:
        if (res.vkImageView)
        {
          vkDestroyImageView(_device->vkDevice, res.vkImageView, nullptr);
          res.vkImageView = VK_NULL_HANDLE;
        }
        break;
      case RecycledType::RENDER_PASS:
        if (res.gpuRenderPass)
        {
          cmdFuncCCVKDestroyRenderPass(_device, res.gpuRenderPass);
          EZ_DEFAULT_DELETE(res.gpuRenderPass);
          res.gpuRenderPass = nullptr;
        }
        break;
      case RecycledType::FRAMEBUFFER:
        if (res.gpuFramebuffer)
        {
          cmdFuncCCVKDestroyFramebuffer(_device, res.gpuFramebuffer);
          EZ_DEFAULT_DELETE(res.gpuFramebuffer);
          res.gpuFramebuffer = nullptr;
        }
        break;
      case RecycledType::SAMPLER:
        if (res.gpuSampler)
        {
          cmdFuncCCVKDestroySampler(_device, res.gpuSampler);
          EZ_DEFAULT_DELETE(res.gpuSampler);
          res.gpuSampler = nullptr;
        }
        break;
      case RecycledType::SHADER:
        if (res.gpuShader)
        {
          cmdFuncCCVKDestroyShader(_device, res.gpuShader);
          EZ_DEFAULT_DELETE(res.gpuShader);
          res.gpuShader = nullptr;
        }
        break;
      case RecycledType::DESCRIPTOR_SET_LAYOUT:
        if (res.gpuDescriptorSetLayout)
        {
          cmdFuncCCVKDestroyDescriptorSetLayout(_device, res.gpuDescriptorSetLayout);
          EZ_DEFAULT_DELETE(res.gpuDescriptorSetLayout);
          res.gpuDescriptorSetLayout = nullptr;
        }
        break;
      case RecycledType::PIPELINE_LAYOUT:
        if (res.gpuPipelineLayout)
        {
          cmdFuncCCVKDestroyPipelineLayout(_device, res.gpuPipelineLayout);
          EZ_DEFAULT_DELETE(res.gpuPipelineLayout);
          res.gpuPipelineLayout = nullptr;
        }
        break;
      case RecycledType::PIPELINE_STATE:
        if (res.gpuPipelineState)
        {
          cmdFuncCCVKDestroyPipelineState(_device, res.gpuPipelineState);
          EZ_DEFAULT_DELETE(res.gpuPipelineState);
          res.gpuPipelineState = nullptr;
        }
        break;
      default:
        break;
    }
    res.type = RecycledType::UNKNOWN;
  }
  _count = 0;
}

VkSampleCountFlagBits CCVKGPUContext::getSampleCountForAttachments(Format format, VkFormat vkFormat, SampleCount sampleCount) const
{
  if (sampleCount <= SampleCount::ONE)
    return VK_SAMPLE_COUNT_1_BIT;

  static ezArrayMap<Format, VkSampleCountFlags> cacheMap;
  if (!cacheMap.Contains(format))
  {
    bool hasDepth = GFX_FORMAT_INFOS[ToNumber(format)].hasDepth;
    VkImageUsageFlags usages = hasDepth ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VkImageFormatProperties properties;
    vkGetPhysicalDeviceImageFormatProperties(physicalDevice, vkFormat, VK_IMAGE_TYPE_2D,
      VK_IMAGE_TILING_OPTIMAL, usages, 0, &properties);
    cacheMap[format] = properties.sampleCounts;
  }

  VkSampleCountFlags availableSampleCounts = cacheMap[format];

  auto requestedSampleCount = VK_SAMPLE_COUNT_FLAGS[ToNumber(sampleCount)];
  if (requestedSampleCount & availableSampleCounts & VK_SAMPLE_COUNT_64_BIT)
    return VK_SAMPLE_COUNT_64_BIT;
  if (requestedSampleCount & availableSampleCounts & VK_SAMPLE_COUNT_32_BIT)
    return VK_SAMPLE_COUNT_32_BIT;
  if (requestedSampleCount & availableSampleCounts & VK_SAMPLE_COUNT_16_BIT)
    return VK_SAMPLE_COUNT_16_BIT;
  if (requestedSampleCount & availableSampleCounts & VK_SAMPLE_COUNT_8_BIT)
    return VK_SAMPLE_COUNT_8_BIT;
  if (requestedSampleCount & availableSampleCounts & VK_SAMPLE_COUNT_4_BIT)
    return VK_SAMPLE_COUNT_4_BIT;
  if (requestedSampleCount & availableSampleCounts & VK_SAMPLE_COUNT_2_BIT)
    return VK_SAMPLE_COUNT_2_BIT;

  return VK_SAMPLE_COUNT_1_BIT;
}

void CCVKGPUBarrierManager::update(CCVKGPUTransportHub* transportHub)
{
  if (_buffersToBeChecked.IsEmpty() && _texturesToBeChecked.IsEmpty())
    return;

  static ezDynamicArray<ThsvsAccessType> prevAccesses;
  static ezDynamicArray<ThsvsAccessType> nextAccesses;
  static ezDynamicArray<VkImageMemoryBarrier> vkImageBarriers;
  VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
  VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  vkImageBarriers.Clear();
  prevAccesses.Clear();
  nextAccesses.Clear();

  for (CCVKGPUBuffer* gpuBuffer : _buffersToBeChecked)
  {
    ezDynamicArray<ThsvsAccessType>& render = gpuBuffer->renderAccessTypes;
    if (gpuBuffer->transferAccess == THSVS_ACCESS_NONE)
      continue;
    if (!prevAccesses.Contains(gpuBuffer->transferAccess))
    {
      prevAccesses.PushBack(gpuBuffer->transferAccess);
    }
    nextAccesses.PushBackRange(render);
    gpuBuffer->transferAccess = THSVS_ACCESS_NONE;
  }

  VkMemoryBarrier vkBarrier;
  VkMemoryBarrier* pVkBarrier = nullptr;
  if (!prevAccesses.IsEmpty())
  {
    ThsvsGlobalBarrier globalBarrier{};
    globalBarrier.prevAccessCount = prevAccesses.GetCount();
    globalBarrier.pPrevAccesses = prevAccesses.GetData();
    globalBarrier.nextAccessCount = nextAccesses.GetCount();
    globalBarrier.pNextAccesses = nextAccesses.GetData();
    VkPipelineStageFlags tempSrcStageMask = 0;
    VkPipelineStageFlags tempDstStageMask = 0;
    thsvsGetVulkanMemoryBarrier(globalBarrier, &tempSrcStageMask, &tempDstStageMask, &vkBarrier);
    srcStageMask |= tempSrcStageMask;
    dstStageMask |= tempDstStageMask;
    pVkBarrier = &vkBarrier;
  }

  ThsvsImageBarrier imageBarrier{};
  imageBarrier.discardContents = false;
  imageBarrier.prevLayout = THSVS_IMAGE_LAYOUT_OPTIMAL;
  imageBarrier.nextLayout = THSVS_IMAGE_LAYOUT_OPTIMAL;
  imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  imageBarrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
  imageBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
  imageBarrier.prevAccessCount = 1;

  for (CCVKGPUTexture* gpuTexture : _texturesToBeChecked)
  {
    ezDynamicArray<ThsvsAccessType>& render = gpuTexture->renderAccessTypes;
    if (gpuTexture->transferAccess == THSVS_ACCESS_NONE || render.IsEmpty())
      continue;
    ezDynamicArray<ThsvsAccessType>& current = gpuTexture->currentAccessTypes;
    imageBarrier.pPrevAccesses = &gpuTexture->transferAccess;
    imageBarrier.nextAccessCount = render.GetCount();
    imageBarrier.pNextAccesses = render.GetData();
    imageBarrier.image = gpuTexture->vkImage;
    imageBarrier.subresourceRange.aspectMask = gpuTexture->aspectMask;

    VkPipelineStageFlags tempSrcStageMask = 0;
    VkPipelineStageFlags tempDstStageMask = 0;
    vkImageBarriers.ExpandAndGetRef();
    thsvsGetVulkanImageMemoryBarrier(imageBarrier, &tempSrcStageMask, &tempDstStageMask, &(vkImageBarriers.PeekBack()));
    srcStageMask |= tempSrcStageMask;
    dstStageMask |= tempDstStageMask;

    // don't override any other access changes since this barrier always happens first
    if (current.GetCount() == 1 && current[0] == gpuTexture->transferAccess)
    {
      current = render;
    }
    gpuTexture->transferAccess = THSVS_ACCESS_NONE;
  }

  if (pVkBarrier || !vkImageBarriers.IsEmpty())
  {
    transportHub->checkIn([&](CCVKGPUCommandBuffer* gpuCommandBuffer)
      { vkCmdPipelineBarrier(gpuCommandBuffer->vkCommandBuffer, srcStageMask, dstStageMask, 0,
          pVkBarrier ? 1 : 0, pVkBarrier, 0, nullptr, vkImageBarriers.GetCount(), vkImageBarriers.GetData()); });
  }

  _buffersToBeChecked.Clear();
  _texturesToBeChecked.Clear();
}

void CCVKGPUBufferHub::flush(CCVKGPUTransportHub* transportHub)
{
  auto& buffers = _buffersToBeUpdated[_device->curBackBufferIndex];
  if (buffers.IsEmpty())
    return;

  bool needTransferCmds = false;
  for (auto& buffer : buffers)
  {
    if (buffer.value.canMemcpy)
    {
      ezUInt8* src = buffer.key->mappedData + buffer.value.srcIndex * buffer.key->instanceSize;
      ezUInt8* dst = buffer.key->mappedData + _device->curBackBufferIndex * buffer.key->instanceSize;
      memcpy(dst, src, buffer.value.size);
    }
    else
    {
      needTransferCmds = true;
    }
  }
  if (needTransferCmds)
  {
    transportHub->checkIn([&](const CCVKGPUCommandBuffer* gpuCommandBuffer)
      {
        VkBufferCopy region;
        for (auto& buffer : buffers)
        {
          if (buffer.value.canMemcpy)
            continue;
          region.srcOffset = buffer.key->getStartOffset(buffer.value.srcIndex);
          region.dstOffset = buffer.key->getStartOffset(_device->curBackBufferIndex);
          region.size = buffer.value.size;
          vkCmdCopyBuffer(gpuCommandBuffer->vkCommandBuffer, buffer.key->vkBuffer, buffer.key->vkBuffer, 1, &region);
        }
      });
  }

  buffers.Clear();
}
