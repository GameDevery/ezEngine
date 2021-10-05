#pragma once

#include <Foundation/Containers/ArrayMap.h>
#include <RHI/Base/DefCommon.h>
#include <RHI/Vulkan/VKStd.h>
#include <RHI/Vulkan/VKUtils.h>
#include <vulkan/vulkan_core.h>

#define TBB_USE_EXCEPTIONS 0 // no-rtti for now
//#include "tbb/concurrent_unordered_map.h"

EZ_DEFINE_AS_POD_TYPE(ThsvsAccessType);

class CCVKGPUContext final
{
public:
  bool initialize();
  void destroy();

  VkInstance vkInstance = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT vkDebugUtilsMessenger = VK_NULL_HANDLE;
  VkDebugReportCallbackEXT vkDebugReport = VK_NULL_HANDLE;

  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkPhysicalDeviceFeatures physicalDeviceFeatures{};
  VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
  VkPhysicalDeviceVulkan11Features physicalDeviceVulkan11Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES};
  VkPhysicalDeviceVulkan12Features physicalDeviceVulkan12Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
  VkPhysicalDeviceDepthStencilResolveProperties physicalDeviceDepthStencilResolveProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES};
  VkPhysicalDeviceProperties physicalDeviceProperties{};
  VkPhysicalDeviceProperties2 physicalDeviceProperties2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
  VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties{};
  ezDynamicArray<VkQueueFamilyProperties> queueFamilyProperties;

  ezUInt32 majorVersion = 0;
  ezUInt32 minorVersion = 0;

  bool validationEnabled = false;

  ezDynamicArray<const char*> layers;
  ezDynamicArray<const char*> extensions;

  inline bool checkExtension(const ezString& extension) const
  {
    for (auto& currentExtension : extensions)
    {
      if (ezStringUtils::IsEqual(currentExtension, extension))
        return true;
    }
    return false;
    //return std::any_of(extensions.begin(), extensions.end(), [&extension](auto& ext)
    //  { return std::strcmp(ext, extension.c_str()) == 0; });
  }

  VkSampleCountFlagBits getSampleCountForAttachments(Format format, VkFormat vkFormat, SampleCount sampleCount) const;
};

struct CCVKAccessInfo
{
  VkPipelineStageFlags stageMask;
  VkAccessFlags accessMask;
  VkImageLayout imageLayout;
  bool hasWriteAccess;
};

class CCVKGPURenderPass final
{
public:
  ColorAttachmentList colorAttachments;
  DepthStencilAttachment depthStencilAttachment;
  SubpassInfoList subpasses;
  SubpassDependencyList dependencies;

  // per attachment
  ezDynamicArray<ezDynamicArray<ThsvsAccessType>> beginAccesses;
  ezDynamicArray<ezDynamicArray<ThsvsAccessType>> endAccesses;

  VkRenderPass vkRenderPass;

  // helper storage
  ezDynamicArray<VkClearValue> clearValues;
  ezDynamicArray<VkSampleCountFlagBits> sampleCounts; // per subpass
};

class CCVKGPUSwapchain;
class CCVKGPUTexture final
{
public:
  TextureType type = TextureType::TEX2D;
  Format format = Format::UNKNOWN;
  TextureUsage usage = TextureUsageBit::NONE;
  ezUInt32 width = 0U;
  ezUInt32 height = 0U;
  ezUInt32 depth = 1U;
  ezUInt32 size = 0U;
  ezUInt32 arrayLayers = 1U;
  ezUInt32 mipLevels = 1U;
  SampleCount samples = SampleCount::ONE;
  TextureFlags flags = TextureFlagBit::NONE;
  VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  bool memoryless = false;

  VkImage vkImage = VK_NULL_HANDLE;
  VmaAllocation vmaAllocation = VK_NULL_HANDLE;

  CCVKGPUSwapchain* swapchain = nullptr;
  ezDynamicArray<VkImage> swapchainVkImages;
  ezDynamicArray<VmaAllocation> swapchainVmaAllocations;

  ezDynamicArray<ThsvsAccessType> currentAccessTypes;

  // for barrier manager
  ezDynamicArray<ThsvsAccessType> renderAccessTypes; // gathered from descriptor sets
  ThsvsAccessType transferAccess = THSVS_ACCESS_NONE;
};

class CCVKGPUTextureView final
{
public:
  CCVKGPUTexture* gpuTexture = nullptr;
  TextureType type = TextureType::TEX2D;
  Format format = Format::UNKNOWN;
  ezUInt32 baseLevel = 0U;
  ezUInt32 levelCount = 1U;
  ezUInt32 baseLayer = 0U;
  ezUInt32 layerCount = 1U;

  ezDynamicArray<VkImageView> swapchainVkImageViews;

  // descriptor infos
  VkImageView vkImageView = VK_NULL_HANDLE;
};

class CCVKGPUSampler final
{
public:
  Filter minFilter = Filter::LINEAR;
  Filter magFilter = Filter::LINEAR;
  Filter mipFilter = Filter::NONE;
  Address addressU = Address::WRAP;
  Address addressV = Address::WRAP;
  Address addressW = Address::WRAP;
  ezUInt32 maxAnisotropy = 0U;
  ComparisonFunc cmpFunc = ComparisonFunc::NEVER;

  // descriptor infos
  VkSampler vkSampler;
};

class CCVKGPUBuffer final
{
public:
  BufferUsage usage = BufferUsage::NONE;
  MemoryUsage memUsage = MemoryUsage::NONE;
  ezUInt32 stride = 0U;
  ezUInt32 count = 0U;
  void* buffer = nullptr;

  bool isDrawIndirectByIndex = false;
  ezDynamicArray<VkDrawIndirectCommand> indirectCmds;
  ezDynamicArray<VkDrawIndexedIndirectCommand> indexedIndirectCmds;

  ezUInt8* mappedData = nullptr;
  VmaAllocation vmaAllocation = VK_NULL_HANDLE;

  // descriptor infos
  VkBuffer vkBuffer = VK_NULL_HANDLE;
  VkDeviceSize startOffset = 0U;
  VkDeviceSize size = 0U;

  VkDeviceSize instanceSize = 0U; // per-back-buffer instance

  // for barrier manager
  ezDynamicArray<ThsvsAccessType> renderAccessTypes; // gathered from descriptor sets
  ThsvsAccessType transferAccess = THSVS_ACCESS_NONE;

  VkDeviceSize getStartOffset(ezUInt32 curBackBufferIndex) const
  {
    return startOffset + instanceSize * curBackBufferIndex;
  }
};

class CCVKGPUBufferView final
{
public:
  CCVKGPUBuffer* gpuBuffer = nullptr;
  ezUInt32 offset = 0U;
  ezUInt32 range = 0U;

  VkDeviceSize getStartOffset(ezUInt32 curBackBufferIndex) const
  {
    return gpuBuffer->getStartOffset(curBackBufferIndex) + offset;
  }
};

class CCVKGPUFramebuffer final
{
public:
  CCVKGPURenderPass* gpuRenderPass = nullptr;
  ezDynamicArray<CCVKGPUTextureView*> gpuColorViews;
  CCVKGPUTextureView* gpuDepthStencilView = nullptr;
  VkFramebuffer vkFramebuffer = VK_NULL_HANDLE;
  CCVKGPUSwapchain* swapchain = nullptr;
  bool isOffscreen = true;
};

using FramebufferList = ezDynamicArray<VkFramebuffer>;
using FramebufferListMap = ezArrayMap<CCVKGPUFramebuffer*, FramebufferList>;
using FramebufferListMapIter = FramebufferListMap::iterator;

class CCVKGPUSwapchain final
{
public:
  VkSurfaceKHR vkSurface = VK_NULL_HANDLE;
  VkSwapchainCreateInfoKHR createInfo{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};

  ezUInt32 curImageIndex = 0U;
  VkSwapchainKHR vkSwapchain = VK_NULL_HANDLE;
  FramebufferListMap vkSwapchainFramebufferListMap;
  ezDynamicArray<VkBool32> queueFamilyPresentables;
  VkResult lastPresentResult = VK_NOT_READY;

  // external references
  ezDynamicArray<VkImage> swapchainImages;
};

class CCVKGPUCommandBuffer final
{
public:
  VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;
  VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  ezUInt32 queueFamilyIndex = 0U;
  bool began = false;
  mutable ezSet<VkBuffer> recordedBuffers;
};

class CCVKGPUQueue final
{
public:
  QueueType type = QueueType::GRAPHICS;
  VkQueue vkQueue = VK_NULL_HANDLE;
  ezUInt32 queueFamilyIndex = 0U;
  ezDynamicArray<ezUInt32> possibleQueueFamilyIndices;
  ezDynamicArray<VkSemaphore> lastSignaledSemaphores;
  ezDynamicArray<VkPipelineStageFlags> submitStageMasks;
  ezDynamicArray<VkCommandBuffer> commandBuffers;
};

struct CCVKGPUShaderStage
{
  CCVKGPUShaderStage(ShaderStageFlagBit t, ezString s)
    : type(t)
    , source(std::move(s))
  {
  }
  ShaderStageFlagBit type = ShaderStageFlagBit::NONE;
  ezString source;
  VkShaderModule vkShader = VK_NULL_HANDLE;
};

class CCVKGPUShader final
{
public:
  ezString name;
  AttributeList attributes;
  UniformBlockList blocks;
  UniformSamplerList samplers;
  ezDynamicArray<CCVKGPUShaderStage> gpuStages;
};

class CCVKGPUInputAssembler final
{
public:
  AttributeList attributes;
  ezDynamicArray<CCVKGPUBufferView*> gpuVertexBuffers;
  CCVKGPUBufferView* gpuIndexBuffer = nullptr;
  CCVKGPUBufferView* gpuIndirectBuffer = nullptr;
  ezDynamicArray<VkBuffer> vertexBuffers;
  ezDynamicArray<VkDeviceSize> vertexBufferOffsets;
};

union CCVKDescriptorInfo
{
  VkDescriptorImageInfo image;
  VkDescriptorBufferInfo buffer;
  VkBufferView texelBufferView;
};
struct CCVKGPUDescriptor
{
  DescriptorType type = DescriptorType::UNKNOWN;
  ezDynamicArray<ThsvsAccessType> accessTypes;
  CCVKGPUBufferView* gpuBufferView = nullptr;
  CCVKGPUTextureView* gpuTextureView = nullptr;
  CCVKGPUSampler* gpuSampler = nullptr;
};

class CCVKGPUDescriptorSetLayout;
class CCVKGPUDescriptorSet final
{
public:
  ezDynamicArray<CCVKGPUDescriptor> gpuDescriptors;

  // references
  CCVKGPUDescriptorSetLayout* gpuLayout = nullptr;

  struct Instance
  {
    VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
    ezDynamicArray<CCVKDescriptorInfo> descriptorInfos;
    ezDynamicArray<VkWriteDescriptorSet> descriptorUpdateEntries;
  };
  ezDynamicArray<Instance> instances; // per swapchain image

  ezUInt32 layoutID = 0U;
};

class CCVKGPUPipelineLayout final
{
public:
  ezDynamicArray<CCVKGPUDescriptorSetLayout*> setLayouts;

  VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;

  // helper storage
  ezDynamicArray<ezUInt32> dynamicOffsetOffsets;
  ezUInt32 dynamicOffsetCount;
};

class CCVKGPUPipelineState final
{
public:
  PipelineBindPoint bindPoint = PipelineBindPoint::GRAPHICS;
  PrimitiveMode primitive = PrimitiveMode::TRIANGLE_LIST;
  CCVKGPUShader* gpuShader = nullptr;
  CCVKGPUPipelineLayout* gpuPipelineLayout = nullptr;
  InputState inputState;
  RasterizerState rs;
  DepthStencilState dss;
  BlendState bs;
  DynamicStateList dynamicStates;
  CCVKGPURenderPass* gpuRenderPass = nullptr;
  ezUInt32 subpass = 0U;
  VkPipeline vkPipeline = VK_NULL_HANDLE;
};

class CCVKGPUGlobalBarrier final
{
public:
  VkPipelineStageFlags srcStageMask = 0U;
  VkPipelineStageFlags dstStageMask = 0U;
  VkMemoryBarrier vkBarrier{};

  ezDynamicArray<ThsvsAccessType> accessTypes;
  ThsvsGlobalBarrier barrier{};
};

class CCVKGPUTextureBarrier final
{
public:
  VkPipelineStageFlags srcStageMask = 0U;
  VkPipelineStageFlags dstStageMask = 0U;
  VkImageMemoryBarrier vkBarrier{};

  ezDynamicArray<ThsvsAccessType> accessTypes;
  ThsvsImageBarrier barrier{};
};

class CCVKGPUCommandBufferPool;
class CCVKGPUDescriptorSetPool;
class CCVKGPUDevice final
{
public:
  VkDevice vkDevice{VK_NULL_HANDLE};
  ezDynamicArray<VkLayerProperties> layers;
  ezDynamicArray<VkExtensionProperties> extensions;
  VmaAllocator memoryAllocator{VK_NULL_HANDLE};
  VkPipelineCache vkPipelineCache{VK_NULL_HANDLE};
  ezUInt32 minorVersion{0U};

  VkFormat depthFormat{VK_FORMAT_UNDEFINED};
  VkFormat depthStencilFormat{VK_FORMAT_UNDEFINED};

  ezUInt32 curBackBufferIndex{0U};
  ezUInt32 backBufferCount{3U};

  bool useDescriptorUpdateTemplate{false};
  bool useMultiDrawIndirect{false};

  PFN_vkCreateRenderPass2 createRenderPass2{nullptr};

  // for default backup usages
  CCVKGPUSampler defaultSampler;
  CCVKGPUTexture defaultTexture;
  CCVKGPUTextureView defaultTextureView;
  CCVKGPUBuffer defaultBuffer;

  ezSet<CCVKGPUSwapchain*> swapchains;

  CCVKGPUCommandBufferPool* getCommandBufferPool();
  CCVKGPUDescriptorSetPool* getDescriptorSetPool(ezUInt32 layoutID);

private:
  friend class CCVKDevice;

  // cannot use thread_local here because we need explicit control over their destruction
  //using CommandBufferPools = tbb::concurrent_unordered_map<size_t, CCVKGPUCommandBufferPool*, std::hash<size_t>>;
  using CommandBufferPools = ezArrayMap<ezUInt32, CCVKGPUCommandBufferPool*>;
  CommandBufferPools _commandBufferPools;

  ezArrayMap<ezUInt32, CCVKGPUDescriptorSetPool> _descriptorSetPools;
};

/**
 * A simple pool for reusing fences.
 */
class CCVKGPUFencePool final
{
public:
  explicit CCVKGPUFencePool(CCVKGPUDevice* device)
    : _device(device)
  {
  }

  ~CCVKGPUFencePool()
  {
    for (VkFence fence : _fences)
    {
      vkDestroyFence(_device->vkDevice, fence, nullptr);
    }
    _fences.Clear();
    _count = 0;
  }

  VkFence alloc()
  {
    if (_count < _fences.GetCount())
    {
      return _fences[_count++];
    }

    VkFence fence = VK_NULL_HANDLE;
    VkFenceCreateInfo createInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    VK_CHECK(vkCreateFence(_device->vkDevice, &createInfo, nullptr, &fence));
    _fences.PushBack(fence);
    _count++;

    return fence;
  }

  void reset()
  {
    if (_count)
    {
      VK_CHECK(vkResetFences(_device->vkDevice, _count, _fences.GetData()));
      _count = 0;
    }
  }

  VkFence* data()
  {
    return _fences.GetData();
  }

  ezUInt32 size() const
  {
    return _count;
  }

private:
  CCVKGPUDevice* _device = nullptr;
  ezUInt32 _count = 0U;
  ezDynamicArray<VkFence> _fences;
};

/**
 * A simple pool for reusing semaphores.
 */
class CCVKGPUSemaphorePool final
{
public:
  explicit CCVKGPUSemaphorePool(CCVKGPUDevice* device)
    : _device(device)
  {
  }

  ~CCVKGPUSemaphorePool()
  {
    for (VkSemaphore semaphore : _semaphores)
    {
      vkDestroySemaphore(_device->vkDevice, semaphore, nullptr);
    }
    _semaphores.Clear();
    _count = 0;
  }

  VkSemaphore alloc()
  {
    if (_count < _semaphores.GetCount())
    {
      return _semaphores[_count++];
    }

    VkSemaphore semaphore = VK_NULL_HANDLE;
    VkSemaphoreCreateInfo createInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    VK_CHECK(vkCreateSemaphore(_device->vkDevice, &createInfo, nullptr, &semaphore));
    _semaphores.PushBack(semaphore);
    _count++;

    return semaphore;
  }

  void reset()
  {
    _count = 0;
  }

  ezUInt32 size() const
  {
    return _count;
  }

private:
  CCVKGPUDevice* _device;
  ezUInt32 _count = 0U;
  ezDynamicArray<VkSemaphore> _semaphores;
};

/**
 * Unlimited descriptor set pool, based on multiple fix-sized VkDescriptorPools.
 */
class CCVKGPUDescriptorSetPool final
{
public:
  ~CCVKGPUDescriptorSetPool()
  {
    for (ezDynamicArray<VkDescriptorSet>& market : _fleaMarkets)
    {
      for (VkDescriptorSet set : market)
      {
        for (DescriptorSetPool& pool : _pools)
        {
          if (pool.activeSets.Contains(set))
          {
            pool.activeSets.Remove(set);
            break;
          }
        }
      }
    }

    ezUInt32 leakedSetCount = 0U;
    for (DescriptorSetPool& pool : _pools)
    {
      leakedSetCount += pool.activeSets.GetCount();
      vkDestroyDescriptorPool(_device->vkDevice, pool.vkPool, nullptr);
    }
    if (leakedSetCount)
      ezLog::Debug("Leaked {0} descriptor sets.", leakedSetCount);

    _pools.Clear();
  }

  void link(CCVKGPUDevice* device, ezUInt32 maxSetsPerPool, const ezDynamicArray<VkDescriptorSetLayoutBinding>& bindings, VkDescriptorSetLayout setLayout)
  {
    _device = device;
    _maxSetsPerPool = maxSetsPerPool;
    //_setLayouts.insert(_setLayouts.cbegin(), _maxSetsPerPool, setLayout);
    {
      auto oldSetLayouts = std::move(_setLayouts);
      _setLayouts = ezDynamicArray<VkDescriptorSetLayout>();
      for (ezUInt32 i = 0; i < maxSetsPerPool; i++)
      {
        _setLayouts.PushBack(setLayout);
      }
      _setLayouts.PushBackRange(std::move(oldSetLayouts));
    }

    _fleaMarkets.SetCount(device->backBufferCount);

    ezArrayMap<VkDescriptorType, ezUInt32> typeMap;
    for (const auto& vkBinding : bindings)
    {
      typeMap[vkBinding.descriptorType] += maxSetsPerPool * vkBinding.descriptorCount;
    }

    // minimal reserve for empty set layouts
    if (bindings.IsEmpty())
    {
      typeMap[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER] = 1;
    }

    _poolSizes.Clear();
    for (auto& it : typeMap)
    {
      _poolSizes.PushBack({it.key, it.value});
    }
  }

  VkDescriptorSet request(ezUInt32 backBufferIndex)
  {
    VkDescriptorSet output = VK_NULL_HANDLE;

    if (!_fleaMarkets[backBufferIndex].IsEmpty())
    {
      output = _fleaMarkets[backBufferIndex].PeekBack();
      _fleaMarkets[backBufferIndex].PopBack();
      return output;
    }

    ezUInt32 size = _pools.GetCount();
    ezUInt32 idx = 0U;
    for (; idx < size; idx++)
    {
      if (!_pools[idx].freeSets.IsEmpty())
      {
        output = _pools[idx].freeSets.PeekBack();
        _pools[idx].freeSets.PopBack();
        _pools[idx].activeSets.Insert(output);
        return output;
      }
    }

    if (idx >= size)
    {
      VkDescriptorPoolCreateInfo createInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
      createInfo.maxSets = _maxSetsPerPool;
      createInfo.poolSizeCount = _poolSizes.GetCount();
      createInfo.pPoolSizes = _poolSizes.GetData();

      VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
      VK_CHECK(vkCreateDescriptorPool(_device->vkDevice, &createInfo, nullptr, &descriptorPool));
      _pools.PushBack({descriptorPool});

      _pools[idx].freeSets.SetCount(_maxSetsPerPool);
      VkDescriptorSetAllocateInfo info{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
      info.pSetLayouts = _setLayouts.GetData();
      info.descriptorSetCount = _maxSetsPerPool;
      info.descriptorPool = descriptorPool;
      VK_CHECK(vkAllocateDescriptorSets(_device->vkDevice, &info, _pools[idx].freeSets.GetData()));
    }

    output = _pools[idx].freeSets.PeekBack();
    _pools[idx].freeSets.PopBack();
    _pools[idx].activeSets.Insert(output);
    return output;
  }

  void yield(VkDescriptorSet set, ezUInt32 backBufferIndex)
  {
    bool found = false;
    for (DescriptorSetPool& pool : _pools)
    {
      if (pool.activeSets.Contains(set))
      {
        found = true;
        break;
      }
    }
    EZ_ASSERT_DEV(found, "wrong descriptor set layout to yield?");
    _fleaMarkets[backBufferIndex].PushBack(set);
  }

private:
  CCVKGPUDevice* _device = nullptr;

  struct DescriptorSetPool
  {
    VkDescriptorPool vkPool = VK_NULL_HANDLE;
    ezSet<VkDescriptorSet> activeSets;
    ezDynamicArray<VkDescriptorSet> freeSets;
  };
  ezDynamicArray<DescriptorSetPool> _pools;

  ezDynamicArray<ezDynamicArray<VkDescriptorSet>> _fleaMarkets; // per back buffer

  ezDynamicArray<VkDescriptorPoolSize> _poolSizes;
  ezDynamicArray<VkDescriptorSetLayout> _setLayouts;
  ezUInt32 _maxSetsPerPool = 0U;
};

class CCVKGPUDescriptorSetLayout final
{
public:
  DescriptorSetLayoutBindingList bindings;
  ezDynamicArray<ezUInt32> dynamicBindings;

  ezDynamicArray<VkDescriptorSetLayoutBinding> vkBindings;
  VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
  VkDescriptorUpdateTemplate vkDescriptorUpdateTemplate = VK_NULL_HANDLE;
  VkDescriptorSet defaultDescriptorSet = VK_NULL_HANDLE;

  ezDynamicArray<ezUInt32> bindingIndices;
  ezDynamicArray<ezUInt32> descriptorIndices;
  ezUInt32 descriptorCount = 0U;

  ezUInt32 id = 0U;
  ezUInt32 maxSetsPerPool = 10U;
};

/**
 * Command buffer pool based on VkCommandPools, always try to reuse previous allocations first.
 */
class CCVKGPUCommandBufferPool final
{
public:
  explicit CCVKGPUCommandBufferPool(CCVKGPUDevice* device)
    : _device(device)
  {
  }

  ~CCVKGPUCommandBufferPool()
  {
    for (auto& it : _pools)
    {
      CommandBufferPool& pool = it.value;
      if (pool.vkCommandPool != VK_NULL_HANDLE)
      {
        vkDestroyCommandPool(_device->vkDevice, pool.vkCommandPool, nullptr);
        pool.vkCommandPool = VK_NULL_HANDLE;
      }
      pool.usedCommandBuffers->Clear();
      pool.commandBuffers->Clear();
    }
    _pools.Clear();
  }

  ezUInt32 getHash(ezUInt32 queueFamilyIndex)
  {
    return (queueFamilyIndex << 10) | _device->curBackBufferIndex;
  }
  static ezUInt32 getBackBufferIndex(ezUInt32 hash)
  {
    return hash & ((1 << 10) - 1);
  }

  void request(CCVKGPUCommandBuffer* gpuCommandBuffer)
  {
    ezUInt32 hash = getHash(gpuCommandBuffer->queueFamilyIndex);

    if (_device->curBackBufferIndex != _lastBackBufferIndex)
    {
      reset();
      _lastBackBufferIndex = _device->curBackBufferIndex;
    }

    if (!_pools.Contains(hash))
    {
      VkCommandPoolCreateInfo createInfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
      createInfo.queueFamilyIndex = gpuCommandBuffer->queueFamilyIndex;
      createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
      VK_CHECK(vkCreateCommandPool(_device->vkDevice, &createInfo, nullptr, &_pools[hash].vkCommandPool));
    }
    CommandBufferPool& pool = _pools[hash];

    ezDynamicArray<VkCommandBuffer>& availableList = pool.commandBuffers[gpuCommandBuffer->level];
    if (availableList.GetCount())
    {
      gpuCommandBuffer->vkCommandBuffer = availableList.PeekBack();
      availableList.PopBack();
    }
    else
    {
      VkCommandBufferAllocateInfo allocateInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
      allocateInfo.commandPool = pool.vkCommandPool;
      allocateInfo.commandBufferCount = 1;
      allocateInfo.level = gpuCommandBuffer->level;
      VK_CHECK(vkAllocateCommandBuffers(_device->vkDevice, &allocateInfo, &gpuCommandBuffer->vkCommandBuffer));
    }
  }

  void yield(CCVKGPUCommandBuffer* gpuCommandBuffer)
  {
    if (gpuCommandBuffer->vkCommandBuffer)
    {
      ezUInt32 hash = getHash(gpuCommandBuffer->queueFamilyIndex);
      EZ_ASSERT_DEV(_pools.Contains(hash), "wrong command pool to yield?");

      CommandBufferPool& pool = _pools[hash];
      pool.usedCommandBuffers[gpuCommandBuffer->level].PushBack(gpuCommandBuffer->vkCommandBuffer);
      gpuCommandBuffer->vkCommandBuffer = VK_NULL_HANDLE;
    }
  }

  void reset()
  {
    for (auto& it : _pools)
    {
      if (getBackBufferIndex(it.key) != _device->curBackBufferIndex)
      {
        continue;
      }
      CommandBufferPool& pool = it.value;

      bool needsReset = false;
      for (ezUInt32 i = 0U; i < 2U; ++i)
      {
        ezDynamicArray<VkCommandBuffer>& usedList = pool.usedCommandBuffers[i];
        if (usedList.GetCount())
        {
          //pool.commandBuffers[i].concat(usedList);
          pool.commandBuffers[i].PushBackRange(usedList);
          usedList.Clear();
          needsReset = true;
        }
      }
      if (needsReset)
      {
        VK_CHECK(vkResetCommandPool(_device->vkDevice, pool.vkCommandPool, 0));
      }
    }
  }

private:
  struct CommandBufferPool
  {
    VkCommandPool vkCommandPool = VK_NULL_HANDLE;
    ezDynamicArray<VkCommandBuffer> commandBuffers[2];
    ezDynamicArray<VkCommandBuffer> usedCommandBuffers[2];
  };

  CCVKGPUDevice* _device = nullptr;
  ezUInt32 _lastBackBufferIndex = 0U;

  ezArrayMap<ezUInt32, CommandBufferPool> _pools;
};

/**
 * Staging buffer pool, based on multiple fix-sized VkBuffer blocks.
 */
constexpr size_t CHUNK_SIZE = 32 * 1024 * 1024; // 32M per block by default
class CCVKGPUStagingBufferPool final
{
public:
  explicit CCVKGPUStagingBufferPool(CCVKGPUDevice* device)
    : _device(device)
  {
  }

  ~CCVKGPUStagingBufferPool()
  {
    for (Buffer& buffer : _pool)
    {
      vmaDestroyBuffer(_device->memoryAllocator, buffer.vkBuffer, buffer.vmaAllocation);
    }
    _pool.Clear();
  }

  void alloc(CCVKGPUBuffer* gpuBuffer) { alloc(gpuBuffer, 1U); }
  void alloc(CCVKGPUBuffer* gpuBuffer, ezUInt32 alignment)
  {
    ezUInt32 bufferCount = _pool.GetCount();
    Buffer* buffer = nullptr;
    VkDeviceSize offset = 0U;
    for (ezUInt32 idx = 0U; idx < bufferCount; idx++)
    {
      Buffer* cur = &_pool[idx];
      offset = roundUp(cur->curOffset, alignment);
      if (CHUNK_SIZE - offset >= gpuBuffer->size)
      {
        buffer = cur;
        break;
      }
    }
    if (!buffer)
    {
      _pool.SetCount(bufferCount + 1);
      buffer = &_pool.PeekBack();
      VkBufferCreateInfo bufferInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
      bufferInfo.size = CHUNK_SIZE;
      bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      VmaAllocationCreateInfo allocInfo{};
      allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
      allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
      VmaAllocationInfo res;
      VK_CHECK(vmaCreateBuffer(_device->memoryAllocator, &bufferInfo, &allocInfo, &buffer->vkBuffer, &buffer->vmaAllocation, &res));
      buffer->mappedData = reinterpret_cast<ezUInt8*>(res.pMappedData);
      offset = 0U;
    }
    gpuBuffer->vkBuffer = buffer->vkBuffer;
    gpuBuffer->startOffset = offset;
    gpuBuffer->mappedData = buffer->mappedData + offset;
    buffer->curOffset = offset + gpuBuffer->size;
  }

  void reset()
  {
    for (Buffer& buffer : _pool)
    {
      buffer.curOffset = 0U;
    }
  }

private:
  struct Buffer
  {
    VkBuffer vkBuffer = VK_NULL_HANDLE;
    ezUInt8* mappedData = nullptr;
    VmaAllocation vmaAllocation = VK_NULL_HANDLE;

    VkDeviceSize curOffset = 0U;
  };

  CCVKGPUDevice* _device = nullptr;
  ezDynamicArray<Buffer> _pool;
};

/**
 * Manages descriptor set update events, across all back buffer instances.
 */
class CCVKGPUDescriptorSetHub final
{
public:
  explicit CCVKGPUDescriptorSetHub(CCVKGPUDevice* device)
    : _device(device)
  {
    _setsToBeUpdated.SetCount(device->backBufferCount);
    if (device->minorVersion > 0)
    {
      _updateFn = vkUpdateDescriptorSetWithTemplate;
    }
    else
    {
      _updateFn = vkUpdateDescriptorSetWithTemplateKHR;
    }
  }

  void record(const CCVKGPUDescriptorSet* gpuDescriptorSet)
  {
    update(gpuDescriptorSet);
    for (ezUInt32 i = 0U; i < _device->backBufferCount; ++i)
    {
      if (i == _device->curBackBufferIndex)
      {
        _setsToBeUpdated[i].Remove(gpuDescriptorSet);
      }
      else
      {
        _setsToBeUpdated[i].Insert(gpuDescriptorSet);
      }
    }
  }

  void erase(CCVKGPUDescriptorSet* gpuDescriptorSet)
  {
    for (ezUInt32 i = 0U; i < _device->backBufferCount; ++i)
    {
      if (_setsToBeUpdated[i].Contains(gpuDescriptorSet))
      {
        _setsToBeUpdated[i].Remove(gpuDescriptorSet);
      }
    }
  }

  void flush()
  {
    DescriptorSetList& sets = _setsToBeUpdated[_device->curBackBufferIndex];
    for (const auto* set : sets)
    {
      update(set);
    }
    sets.Clear();
  }

private:
  void update(const CCVKGPUDescriptorSet* gpuDescriptorSet)
  {
    const CCVKGPUDescriptorSet::Instance& instance = gpuDescriptorSet->instances[_device->curBackBufferIndex];
    if (gpuDescriptorSet->gpuLayout->vkDescriptorUpdateTemplate)
    {
      _updateFn(_device->vkDevice, instance.vkDescriptorSet,
        gpuDescriptorSet->gpuLayout->vkDescriptorUpdateTemplate, instance.descriptorInfos.GetData());
    }
    else
    {
      const ezDynamicArray<VkWriteDescriptorSet>& entries = instance.descriptorUpdateEntries;
      vkUpdateDescriptorSets(_device->vkDevice, entries.GetCount(), entries.GetData(), 0, nullptr);
    }
  }

  using DescriptorSetList = ezSet<const CCVKGPUDescriptorSet*>;

  CCVKGPUDevice* _device = nullptr;
  ezDynamicArray<DescriptorSetList> _setsToBeUpdated;
  PFN_vkUpdateDescriptorSetWithTemplate _updateFn = nullptr;
};

/**
 * Descriptor data maintenance hub, events like buffer/texture resizing,
 * descriptor set binding change, etc. should all request an update operation here.
 */
class CCVKGPUDescriptorHub final
{
public:
  explicit CCVKGPUDescriptorHub(CCVKGPUDevice* /*device*/)
  {
  }

  void link(CCVKGPUDescriptorSetHub* descriptorSetHub)
  {
    _descriptorSetHub = descriptorSetHub;
  }

  void connect(const CCVKGPUDescriptorSet* set, const CCVKGPUBufferView* buffer, VkDescriptorBufferInfo* descriptor, ezUInt32 instanceIdx)
  {
    _buffers[buffer].sets.Insert(set);
    _buffers[buffer].descriptors.PushBack(descriptor);
    _bufferInstanceIndices[descriptor] = instanceIdx;
  }
  void connect(const CCVKGPUDescriptorSet* set, const CCVKGPUTextureView* texture, VkDescriptorImageInfo* descriptor)
  {
    _textures[texture].sets.Insert(set);
    _textures[texture].descriptors.PushBack(descriptor);
  }
  void connect(const CCVKGPUSampler* sampler, VkDescriptorImageInfo* descriptor)
  {
    _samplers[sampler].PushBack(descriptor);
  }

  void update(const CCVKGPUBufferView* buffer)
  {
    for (const auto& it : _buffers)
    {
      if (it.key->gpuBuffer != buffer->gpuBuffer)
        continue;
      const auto& info = it.value;
      for (ezUInt32 i = 0U; i < info.descriptors.GetCount(); i++)
      {
        doUpdate(buffer, info.descriptors[i]);
      }
      for (const auto* set : info.sets)
      {
        _descriptorSetHub->record(set);
      }
    }
  }
  void update(const CCVKGPUBufferView* buffer, VkDescriptorBufferInfo* descriptor)
  {
    auto it = _buffers.Find(buffer);
    if (it == ezInvalidIndex)
      return;
    auto& descriptors = _buffers[it].descriptors;
    for (ezUInt32 i = 0U; i < descriptors.GetCount(); i++)
    {
      if (descriptors[i] == descriptor)
      {
        doUpdate(buffer, descriptor);
        break;
      }
    }
  }
  void update(const CCVKGPUTextureView* texture)
  {
    for (const auto& it : _textures)
    {
      if (it.key->gpuTexture != texture->gpuTexture)
        continue;
      const auto& info = it.value;
      for (ezUInt32 i = 0U; i < info.descriptors.GetCount(); i++)
      {
        doUpdate(texture, info.descriptors[i]);
      }
      for (const auto* set : info.sets)
      {
        _descriptorSetHub->record(set);
      }
    }
  }
  void update(const CCVKGPUTextureView* texture, VkDescriptorImageInfo* descriptor)
  {
    auto it = _textures.Find(texture);
    if (it == ezInvalidIndex)
      return;
    auto& descriptors = _textures[it].descriptors;
    for (ezUInt32 i = 0U; i < descriptors.GetCount(); i++)
    {
      if (descriptors[i] == descriptor)
      {
        doUpdate(texture, descriptor);
        break;
      }
    }
  }
  void update(const CCVKGPUSampler* sampler, VkDescriptorImageInfo* descriptor)
  {
    auto index = _samplers.Find(sampler);
    if (index == ezInvalidIndex)
      return;
    auto& descriptors = _samplers.GetValue(index);
    for (ezUInt32 i = 0U; i < descriptors.GetCount(); ++i)
    {
      if (descriptors[i] == descriptor)
      {
        doUpdate(sampler, descriptor);
        break;
      }
    }
  }
  // for resize events
  void update(const CCVKGPUBuffer* buffer)
  {
    for (const auto& it : _buffers)
    {
      if (it.key->gpuBuffer != buffer)
        continue;
      const auto& info = it.value;
      for (ezUInt32 i = 0U; i < info.descriptors.GetCount(); i++)
      {
        doUpdate(it.key, info.descriptors[i]);
      }
      for (const auto* set : info.sets)
      {
        _descriptorSetHub->record(set);
      }
    }
  }

  void disengage(const CCVKGPUBufferView* buffer)
  {
    auto it = _buffers.Find(buffer);
    if (it == ezInvalidIndex)
      return;
    for (ezUInt32 i = 0; i < _buffers[it].descriptors.GetCount(); ++i)
    {
      _bufferInstanceIndices.RemoveAndCopy(_buffers[it].descriptors[i]);
    }
    _buffers.RemoveAtAndCopy(it);
  }
  void disengage(const CCVKGPUDescriptorSet* set, const CCVKGPUBufferView* buffer, VkDescriptorBufferInfo* descriptor)
  {
    auto it = _buffers.Find(buffer);
    if (it == ezInvalidIndex)
      return;
    _buffers[it].sets.Remove(set);
    auto& descriptors = _buffers[it].descriptors;
    descriptors.RemoveAtAndCopy(descriptors.IndexOf(descriptor));
    _bufferInstanceIndices.RemoveAndCopy(descriptor);
  }
  void disengage(const CCVKGPUTextureView* texture)
  {
    auto it = _textures.Find(texture);
    if (it == ezInvalidIndex)
      return;
    _textures.RemoveAtAndCopy(it);
  }
  void disengage(const CCVKGPUDescriptorSet* set, const CCVKGPUTextureView* texture, VkDescriptorImageInfo* descriptor)
  {
    auto it = _textures.Find(texture);
    if (it == ezInvalidIndex)
      return;
    _textures[it].sets.Remove(set);
    auto& descriptors = _textures[it].descriptors;
    descriptors.RemoveAtAndCopy(descriptors.IndexOf(descriptor));
  }
  void disengage(const CCVKGPUSampler* sampler)
  {
    auto index = _samplers.Find(sampler);
    if (index == ezInvalidIndex)
      return;
    _samplers.RemoveAtAndCopy(index);
  }
  void disengage(const CCVKGPUSampler* sampler, VkDescriptorImageInfo* descriptor)
  {
    auto index = _samplers.Find(sampler);
    if (index == ezInvalidIndex)
      return;
    auto& descriptors = _samplers.GetValue(index);
    descriptors.RemoveAtAndCopy(descriptors.IndexOf(descriptor));
  }

private:
  void doUpdate(const CCVKGPUBufferView* buffer, VkDescriptorBufferInfo* descriptor)
  {
    descriptor->buffer = buffer->gpuBuffer->vkBuffer;
    descriptor->offset = buffer->getStartOffset(_bufferInstanceIndices[descriptor]);
    descriptor->range = buffer->range;
  }

  static void doUpdate(const CCVKGPUTextureView* texture, VkDescriptorImageInfo* descriptor)
  {
    descriptor->imageView = texture->vkImageView;
    if (HasFlag(texture->gpuTexture->flags, TextureFlagBit::GENERAL_LAYOUT))
    {
      descriptor->imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    }
    else
    {
      descriptor->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }
  }

  static void doUpdate(const CCVKGPUSampler* sampler, VkDescriptorImageInfo* descriptor)
  {
    descriptor->sampler = sampler->vkSampler;
  }

  template <typename T>
  struct DescriptorInfo
  {
    ezSet<const CCVKGPUDescriptorSet*> sets;
    ezDynamicArray<T*> descriptors;
  };

  ezArrayMap<const VkDescriptorBufferInfo*, ezUInt32> _bufferInstanceIndices;
  ezArrayMap<const CCVKGPUBufferView*, DescriptorInfo<VkDescriptorBufferInfo>> _buffers;
  ezArrayMap<const CCVKGPUTextureView*, DescriptorInfo<VkDescriptorImageInfo>> _textures;
  ezArrayMap<const CCVKGPUSampler*, ezDynamicArray<VkDescriptorImageInfo*>> _samplers;

  CCVKGPUDescriptorSetHub* _descriptorSetHub = nullptr;
};

/**
 * Recycle bin for GPU resources, clears after vkDeviceWaitIdle every frame.
 * All the destroy events will be postponed to that time.
 */
class CCVKGPURecycleBin final
{
public:
  explicit CCVKGPURecycleBin(CCVKGPUDevice* device)
    : _device(device)
  {
    _resources.SetCount(16);
  }

  void collect(CCVKGPUTexture* gpuTexture)
  {
    if (_resources.GetCount() <= _count)
    {
      _resources.SetCount(_count * 2);
    }
    if (gpuTexture->swapchain)
    {
      if (GFX_FORMAT_INFOS[ToNumber(gpuTexture->format)].hasDepth)
      {
        for (ezUInt32 i = 0; i < gpuTexture->swapchainVkImages.GetCount(); ++i)
        {
          vmaDestroyImage(_device->memoryAllocator, gpuTexture->swapchainVkImages[i], gpuTexture->swapchainVmaAllocations[i]);
        }
        gpuTexture->swapchainVkImages.Clear();
        gpuTexture->swapchainVmaAllocations.Clear();
      }
    }
    else
    {
      Resource& res = _resources[_count++];
      res.type = RecycledType::TEXTURE;
      res.image = {gpuTexture->vkImage, gpuTexture->vmaAllocation};
    }
  }

  void collect(CCVKGPUTextureView* gpuTextureView)
  {
    if (_resources.GetCount() <= _count)
    {
      _resources.SetCount(_count * 2);
    }
    if (gpuTextureView->gpuTexture->swapchain)
    {
      for (VkImageView swapchainVkImageView : gpuTextureView->swapchainVkImageViews)
      {
        vkDestroyImageView(_device->vkDevice, swapchainVkImageView, nullptr);
      }
      gpuTextureView->swapchainVkImageViews.Clear();
    }
    else
    {
      Resource& res = _resources[_count++];
      res.type = RecycledType::TEXTURE_VIEW;
      res.vkImageView = gpuTextureView->vkImageView;
    }
  }

#define DEFINE_RECYCLE_BIN_COLLECT_FN(_type, typeValue, expr) \
  void collect(_type* gpuRes)                                 \
  { /* NOLINT(bugprone-macro-parentheses) N/A */              \
    if (_resources.GetCount() <= _count)                      \
    {                                                         \
      _resources.SetCount(_count * 2);                        \
    }                                                         \
    Resource& res = _resources[_count++];                     \
    res.type = typeValue;                                     \
    expr;                                                     \
  }

  DEFINE_RECYCLE_BIN_COLLECT_FN(CCVKGPUBuffer, RecycledType::BUFFER, (res.buffer = {gpuRes->vkBuffer, gpuRes->vmaAllocation}))
  DEFINE_RECYCLE_BIN_COLLECT_FN(CCVKGPURenderPass, RecycledType::RENDER_PASS, res.gpuRenderPass = gpuRes)
  DEFINE_RECYCLE_BIN_COLLECT_FN(CCVKGPUFramebuffer, RecycledType::FRAMEBUFFER, res.gpuFramebuffer = gpuRes)
  DEFINE_RECYCLE_BIN_COLLECT_FN(CCVKGPUSampler, RecycledType::SAMPLER, res.gpuSampler = gpuRes)
  DEFINE_RECYCLE_BIN_COLLECT_FN(CCVKGPUShader, RecycledType::SHADER, res.gpuShader = gpuRes)
  DEFINE_RECYCLE_BIN_COLLECT_FN(CCVKGPUDescriptorSetLayout, RecycledType::DESCRIPTOR_SET_LAYOUT, res.gpuDescriptorSetLayout = gpuRes)
  DEFINE_RECYCLE_BIN_COLLECT_FN(CCVKGPUPipelineLayout, RecycledType::PIPELINE_LAYOUT, res.gpuPipelineLayout = gpuRes)
  DEFINE_RECYCLE_BIN_COLLECT_FN(CCVKGPUPipelineState, RecycledType::PIPELINE_STATE, res.gpuPipelineState = gpuRes)

  void clear();

private:
  enum class RecycledType
  {
    UNKNOWN,
    BUFFER,
    TEXTURE,
    TEXTURE_VIEW,
    RENDER_PASS,
    FRAMEBUFFER,
    SAMPLER,
    SHADER,
    DESCRIPTOR_SET_LAYOUT,
    PIPELINE_LAYOUT,
    PIPELINE_STATE,
  };
  struct Buffer
  {
    VkBuffer vkBuffer;
    VmaAllocation vmaAllocation;
  };
  struct Image
  {
    VkImage vkImage;
    VmaAllocation vmaAllocation;
  };
  struct Resource
  {
    RecycledType type = RecycledType::UNKNOWN;
    union
    {
      // resizable resources, cannot take over directly
      // or descriptor sets won't work
      Buffer buffer;
      Image image;
      VkImageView vkImageView;

      CCVKGPURenderPass* gpuRenderPass;
      CCVKGPUFramebuffer* gpuFramebuffer;
      CCVKGPUSampler* gpuSampler;
      CCVKGPUShader* gpuShader;
      CCVKGPUDescriptorSetLayout* gpuDescriptorSetLayout;
      CCVKGPUPipelineLayout* gpuPipelineLayout;
      CCVKGPUPipelineState* gpuPipelineState;
    };
  };
  CCVKGPUDevice* _device = nullptr;
  ezDynamicArray<Resource> _resources;
  ezUInt32 _count = 0U;
};

/**
 * Transport hub for data traveling between host and devices.
 * Record all transfer commands until batched submission.
 */
//#define ASYNC_BUFFER_UPDATE
class CCVKGPUTransportHub final
{
public:
  CCVKGPUTransportHub(CCVKGPUDevice* device, CCVKGPUQueue* queue)
    : _device(device)
    , _queue(queue)
  {
    _earlyCmdBuff.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    _earlyCmdBuff.queueFamilyIndex = _queue->queueFamilyIndex;

    _lateCmdBuff.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    _lateCmdBuff.queueFamilyIndex = _queue->queueFamilyIndex;

    VkFenceCreateInfo createInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    VK_CHECK(vkCreateFence(_device->vkDevice, &createInfo, nullptr, &_fence));
  }

  ~CCVKGPUTransportHub()
  {
    if (_fence)
    {
      vkDestroyFence(_device->vkDevice, _fence, nullptr);
      _fence = VK_NULL_HANDLE;
    }
  }

  bool empty(bool late) const
  {
    const CCVKGPUCommandBuffer* cmdBuff = late ? &_lateCmdBuff : &_earlyCmdBuff;

    return !cmdBuff->vkCommandBuffer;
  }

  template <typename TFunc>
  void checkIn(const TFunc& record, bool immediateSubmission = false, bool late = false)
  {
    CCVKGPUCommandBufferPool* commandBufferPool = _device->getCommandBufferPool();
    CCVKGPUCommandBuffer* cmdBuff = late ? &_lateCmdBuff : &_earlyCmdBuff;

    if (!cmdBuff->vkCommandBuffer)
    {
      commandBufferPool->request(cmdBuff);
      VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
      beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
      VK_CHECK(vkBeginCommandBuffer(cmdBuff->vkCommandBuffer, &beginInfo));
    }

    record(cmdBuff);

    if (immediateSubmission)
    {
      VK_CHECK(vkEndCommandBuffer(cmdBuff->vkCommandBuffer));
      VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = &cmdBuff->vkCommandBuffer;
      VK_CHECK(vkQueueSubmit(_queue->vkQueue, 1, &submitInfo, _fence));
      VK_CHECK(vkWaitForFences(_device->vkDevice, 1, &_fence, VK_TRUE, DEFAULT_TIMEOUT));
      vkResetFences(_device->vkDevice, 1, &_fence);
      commandBufferPool->yield(cmdBuff);
      cmdBuff->vkCommandBuffer = VK_NULL_HANDLE;
    }
  }

  VkCommandBuffer packageForFlight(bool late)
  {
    CCVKGPUCommandBuffer* cmdBuff = late ? &_lateCmdBuff : &_earlyCmdBuff;

    VkCommandBuffer vkCommandBuffer = cmdBuff->vkCommandBuffer;
    if (vkCommandBuffer)
    {
      VK_CHECK(vkEndCommandBuffer(vkCommandBuffer));
      _device->getCommandBufferPool()->yield(cmdBuff);
    }
    return vkCommandBuffer;
  }

private:
  CCVKGPUDevice* _device = nullptr;

  CCVKGPUQueue* _queue = nullptr;
  CCVKGPUCommandBuffer _earlyCmdBuff;
  CCVKGPUCommandBuffer _lateCmdBuff;
  VkFence _fence = VK_NULL_HANDLE;
};

class CCVKGPUBarrierManager final
{
public:
  explicit CCVKGPUBarrierManager(CCVKGPUDevice* device)
    : _device(device)
  {
  }

  void checkIn(CCVKGPUBuffer* gpuBuffer)
  {
    _buffersToBeChecked.Insert(gpuBuffer);
  }

  void checkIn(CCVKGPUTexture* gpuTexture, const ThsvsAccessType* newTypes = nullptr, ezUInt32 newTypeCount = 0)
  {
    ezDynamicArray<ThsvsAccessType>& target = gpuTexture->renderAccessTypes;
    for (ezUInt32 i = 0U; i < newTypeCount; ++i)
    {
      if (!target.Contains(newTypes[i]))
        target.PushBack(newTypes[i]);

      //if (std::find(target.begin(), target.end(), newTypes[i]) == target.end())
      //{
      //  target.push_back(newTypes[i]);
      //}
    }
    _texturesToBeChecked.Insert(gpuTexture);
  }

  void update(CCVKGPUTransportHub* transportHub);

  inline void cancel(CCVKGPUBuffer* gpuBuffer) { _buffersToBeChecked.Remove(gpuBuffer); }
  inline void cancel(CCVKGPUTexture* gpuTexture) { _texturesToBeChecked.Remove(gpuTexture); }

private:
  ezSet<CCVKGPUBuffer*> _buffersToBeChecked;
  ezSet<CCVKGPUTexture*> _texturesToBeChecked;
  CCVKGPUDevice* _device = nullptr;
};

/**
 * Manages buffer update events, across all back buffer instances.
 */
class CCVKGPUBufferHub final
{
public:
  explicit CCVKGPUBufferHub(CCVKGPUDevice* device)
    : _device(device)
  {
    _buffersToBeUpdated.SetCount(device->backBufferCount);
  }

  void record(CCVKGPUBuffer* gpuBuffer, ezUInt32 backBufferIndex, size_t size, bool canMemcpy)
  {
    for (ezUInt32 i = 0U; i < _device->backBufferCount; ++i)
    {
      if (i == backBufferIndex)
      {
        _buffersToBeUpdated[i].RemoveAndCopy(gpuBuffer);
      }
      else
      {
        _buffersToBeUpdated[i][gpuBuffer] = {backBufferIndex, size, canMemcpy};
      }
    }
  }

  void erase(CCVKGPUBuffer* gpuBuffer)
  {
    for (ezUInt32 i = 0U; i < _device->backBufferCount; ++i)
    {
      if (_buffersToBeUpdated[i].Contains(gpuBuffer))
      {
        _buffersToBeUpdated[i].RemoveAndCopy(gpuBuffer);
      }
    }
  }

  void flush(CCVKGPUTransportHub* transportHub);

private:
  struct BufferUpdate
  {
    ezUInt32 srcIndex = 0U;
    size_t size = 0U;
    bool canMemcpy = false;
  };

  ezDynamicArray<ezArrayMap<CCVKGPUBuffer*, BufferUpdate>> _buffersToBeUpdated;

  CCVKGPUDevice* _device = nullptr;
};
