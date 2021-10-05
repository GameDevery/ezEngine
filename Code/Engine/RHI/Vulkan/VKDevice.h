#pragma once

#include <RHI/Base/Device.h>
#include <RHI/Vulkan/VKStd.h>

class CCVKTexture;

class CCVKGPUDevice;
class CCVKGPUContext;

class CCVKGPUBufferHub;
class CCVKGPUTransportHub;
class CCVKGPUDescriptorHub;
class CCVKGPUSemaphorePool;
class CCVKGPUBarrierManager;
class CCVKGPUDescriptorSetHub;

class CCVKGPUFencePool;
class CCVKGPURecycleBin;
class CCVKGPUStagingBufferPool;

class EZ_VULKAN_API CCVKDevice final : public Device
{
public:
  static CCVKDevice* getInstance();

  ~CCVKDevice() override;

  friend class CCVKContext;
  using Device::copyBuffersToTexture;
  using Device::createBuffer;
  using Device::createCommandBuffer;
  using Device::createDescriptorSet;
  using Device::createDescriptorSetLayout;
  using Device::createFramebuffer;
  using Device::createGlobalBarrier;
  using Device::createInputAssembler;
  using Device::createPipelineLayout;
  using Device::createPipelineState;
  using Device::createQueue;
  using Device::createRenderPass;
  using Device::createSampler;
  using Device::createShader;
  using Device::createTexture;
  using Device::createTextureBarrier;

  void acquire(Swapchain* const* swapchains, ezUInt32 count) override;
  void present() override;

  inline bool checkExtension(const ezString& extension) const
  {
    for (auto& currentExtension : _extensions)
    {
      if (ezStringUtils::IsEqual(currentExtension, extension))
        return true;
    }
    return false;

    //return std::any_of(_extensions.begin(), _extensions.end(), [&extension](auto& ext)
    //  { return std::strcmp(ext, extension.c_str()) == 0; });
  }

  inline CCVKGPUDevice* gpuDevice() const { return _gpuDevice; }
  inline CCVKGPUContext* gpuContext() { return _gpuContext; }

  inline CCVKGPUBufferHub* gpuBufferHub() { return _gpuBufferHub; }
  inline CCVKGPUTransportHub* gpuTransportHub() { return _gpuTransportHub; }
  inline CCVKGPUDescriptorHub* gpuDescriptorHub() { return _gpuDescriptorHub; }
  inline CCVKGPUSemaphorePool* gpuSemaphorePool() { return _gpuSemaphorePool; }
  inline CCVKGPUBarrierManager* gpuBarrierManager() { return _gpuBarrierManager; }
  inline CCVKGPUDescriptorSetHub* gpuDescriptorSetHub() { return _gpuDescriptorSetHub; }

  CCVKGPUFencePool* gpuFencePool();
  CCVKGPURecycleBin* gpuRecycleBin();
  CCVKGPUStagingBufferPool* gpuStagingBufferPool();
  void waitAllFences();

protected:
  static CCVKDevice* instance;

  friend class DeviceManager;

  CCVKDevice();

  bool doInit(const DeviceInfo& info) override;
  void doDestroy() override;
  CommandBuffer* createCommandBuffer(const CommandBufferInfo& info, bool hasAgent) override;
  Queue* createQueue() override;
  Swapchain* createSwapchain() override;
  Buffer* createBuffer() override;
  Texture* createTexture() override;
  Shader* createShader() override;
  InputAssembler* createInputAssembler() override;
  RenderPass* createRenderPass() override;
  Framebuffer* createFramebuffer() override;
  DescriptorSet* createDescriptorSet() override;
  DescriptorSetLayout* createDescriptorSetLayout() override;
  PipelineLayout* createPipelineLayout() override;
  PipelineState* createPipelineState() override;

  Sampler* createSampler(const SamplerInfo& info, ezUInt32 hash) override;
  GlobalBarrier* createGlobalBarrier(const GlobalBarrierInfo& info, ezUInt32 hash) override;
  TextureBarrier* createTextureBarrier(const TextureBarrierInfo& info, ezUInt32 hash) override;

  void copyBuffersToTexture(const uint8_t* const* buffers, Texture* dst, const BufferTextureCopy* regions, ezUInt32 count) override;
  void copyTextureToBuffers(Texture* src, uint8_t* const* buffers, const BufferTextureCopy* region, ezUInt32 count) override;

  void destroySwapchain();
  bool checkSwapchainStatus();

  CCVKGPUDevice* _gpuDevice = nullptr;
  CCVKGPUContext* _gpuContext = nullptr;
  ezDynamicArray<CCVKTexture*> _depthStencilTextures;

  ezDynamicArray<CCVKGPUFencePool*> _gpuFencePools;
  ezDynamicArray<CCVKGPURecycleBin*> _gpuRecycleBins;
  ezDynamicArray<CCVKGPUStagingBufferPool*> _gpuStagingBufferPools;

  CCVKGPUBufferHub* _gpuBufferHub = nullptr;
  CCVKGPUTransportHub* _gpuTransportHub = nullptr;
  CCVKGPUDescriptorHub* _gpuDescriptorHub = nullptr;
  CCVKGPUSemaphorePool* _gpuSemaphorePool = nullptr;
  CCVKGPUDescriptorSetHub* _gpuDescriptorSetHub = nullptr;
  CCVKGPUBarrierManager* _gpuBarrierManager = nullptr;

  ezDynamicArray<const char*> _layers;
  ezDynamicArray<const char*> _extensions;
};
