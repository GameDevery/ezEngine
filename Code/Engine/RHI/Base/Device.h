#pragma once

#include <Foundation/Containers/ArrayMap.h>
#include <RHI/RHIDLL.h>

#include <RHI/Base/Queue.h>
#include <RHI/Base/Swapchain.h>
#include <RHI/Base/PipelineLayout.h>
#include <RHI/Base/PipelineState.h>
#include <RHI/Base/Shader.h>
#include <RHI/Base/Framebuffer.h>
#include <RHI/Base/RenderPass.h>
#include <RHI/Base/States/GlobalBarrier.h>
#include <RHI/Base/States/Sampler.h>
#include <RHI/Base/States/TextureBarrier.h>
#include <RHI/Base/DescriptorSet.h>
#include <RHI/Base/DescriptorSetLayout.h>
#include <RHI/Base/CommandBuffer.h>
#include <RHI/Base/Buffer.h>

class EZ_RHI_DLL Device
{
public:
  static Device* getInstance();

  virtual ~Device();

  bool initialize(const DeviceInfo& info);
  void destroy();

  virtual void acquire(Swapchain* const* swapchains, ezUInt32 count) = 0;
  virtual void present() = 0;

  virtual void flushCommands(CommandBuffer* const* cmdBuffs, ezUInt32 count) {}

  virtual MemoryStatus& getMemoryStatus() { return _memoryStatus; }
  virtual ezUInt32 getNumDrawCalls() const { return _numDrawCalls; }
  virtual ezUInt32 getNumInstances() const { return _numInstances; }
  virtual ezUInt32 getNumTris() const { return _numTriangles; }

  inline CommandBuffer* createCommandBuffer(const CommandBufferInfo& info);
  inline Queue* createQueue(const QueueInfo& info);
  inline Swapchain* createSwapchain(const SwapchainInfo& info);
  inline Buffer* createBuffer(const BufferInfo& info);
  inline Buffer* createBuffer(const BufferViewInfo& info);
  inline Texture* createTexture(const TextureInfo& info);
  inline Texture* createTexture(const TextureViewInfo& info);
  inline Shader* createShader(const ShaderInfo& info);
  inline InputAssembler* createInputAssembler(const InputAssemblerInfo& info);
  inline RenderPass* createRenderPass(const RenderPassInfo& info);
  inline Framebuffer* createFramebuffer(const FramebufferInfo& info);
  inline DescriptorSet* createDescriptorSet(const DescriptorSetInfo& info);
  inline DescriptorSetLayout* createDescriptorSetLayout(const DescriptorSetLayoutInfo& info);
  inline PipelineLayout* createPipelineLayout(const PipelineLayoutInfo& info);
  inline PipelineState* createPipelineState(const PipelineStateInfo& info);

  inline Sampler* getSampler(const SamplerInfo& info);
  inline GlobalBarrier* getGlobalBarrier(const GlobalBarrierInfo& info);
  inline TextureBarrier* getTextureBarrier(const TextureBarrierInfo& info);

  virtual void copyBuffersToTexture(const uint8_t* const* buffers, Texture* dst, const BufferTextureCopy* regions, ezUInt32 count) = 0;
  virtual void copyTextureToBuffers(Texture* src, uint8_t* const* buffers, const BufferTextureCopy* region, ezUInt32 count) = 0;

  inline void copyBuffersToTexture(const BufferDataList& buffers, Texture* dst, const BufferTextureCopyList& regions);
  inline void flushCommands(const ezDynamicArray<CommandBuffer*>& cmdBuffs);
  inline void acquire(const ezDynamicArray<Swapchain*>& swapchains);

  inline Queue* getQueue() const { return _queue; }
  inline CommandBuffer* getCommandBuffer() const { return _cmdBuff; }
  inline const DeviceCaps& getCapabilities() const { return _caps; }
  inline API getGfxAPI() const { return _api; }
  inline const ezString& getDeviceName() const { return _deviceName; }
  inline const ezString& getRenderer() const { return _renderer; }
  inline const ezString& getVendor() const { return _vendor; }
  inline bool hasFeature(Feature feature) const { return _features[static_cast<ezUInt32>(feature)]; }

  inline const BindingMappingInfo& bindingMappingInfo() const { return _bindingMappingInfo; }

protected:
  static Device* instance;

  friend class DeviceAgent;
  friend class DeviceValidator;
  friend class DeviceManager;

  Device();

  void destroySurface(void* windowHandle);
  void createSurface(void* windowHandle);

  virtual bool doInit(const DeviceInfo& info) = 0;
  virtual void doDestroy() = 0;

  virtual CommandBuffer* createCommandBuffer(const CommandBufferInfo& info, bool hasAgent) = 0;
  virtual Queue* createQueue() = 0;
  virtual Swapchain* createSwapchain() = 0;
  virtual Buffer* createBuffer() = 0;
  virtual Texture* createTexture() = 0;
  virtual Shader* createShader() = 0;
  virtual InputAssembler* createInputAssembler() = 0;
  virtual RenderPass* createRenderPass() = 0;
  virtual Framebuffer* createFramebuffer() = 0;
  virtual DescriptorSet* createDescriptorSet() = 0;
  virtual DescriptorSetLayout* createDescriptorSetLayout() = 0;
  virtual PipelineLayout* createPipelineLayout() = 0;
  virtual PipelineState* createPipelineState() = 0;

  virtual Sampler* createSampler(const SamplerInfo& info, ezUInt32 hash) = 0;
  virtual GlobalBarrier* createGlobalBarrier(const GlobalBarrierInfo& info, ezUInt32 hash) = 0;
  virtual TextureBarrier* createTextureBarrier(const TextureBarrierInfo& info, ezUInt32 hash) = 0;

  // For context switching between threads
  virtual void bindContext(bool bound) {}

  ezString _deviceName;
  ezString _renderer;
  ezString _vendor;
  ezString _version;
  API _api{API::UNKNOWN};
  bool _multithreadedSubmission{true};
  DeviceCaps _caps;
  BindingMappingInfo _bindingMappingInfo;

  ezStaticArray<bool, static_cast<ezUInt32>(Feature::COUNT)> _features;

  Queue* _queue{nullptr};
  CommandBuffer* _cmdBuff{nullptr};

  ezUInt32 _numDrawCalls{0U};
  ezUInt32 _numInstances{0U};
  ezUInt32 _numTriangles{0U};
  MemoryStatus _memoryStatus;

  ezArrayMap<ezUInt32, Sampler*> _samplers;
  ezArrayMap<ezUInt32, GlobalBarrier*> _globalBarriers;
  ezArrayMap<ezUInt32, TextureBarrier*> _textureBarriers;

private:
  ezDynamicArray<Swapchain*> _swapchains;
};

//////////////////////////////////////////////////////////////////////////

CommandBuffer* Device::createCommandBuffer(const CommandBufferInfo& info)
{
  CommandBuffer* res = createCommandBuffer(info, false);
  res->initialize(info);
  return res;
}

Queue* Device::createQueue(const QueueInfo& info)
{
  Queue* res = createQueue();
  res->initialize(info);
  return res;
}

Swapchain* Device::createSwapchain(const SwapchainInfo& info)
{
  Swapchain* res = createSwapchain();
  res->initialize(info);
  _swapchains.PushBack(res);
  return res;
}

Buffer* Device::createBuffer(const BufferInfo& info)
{
  Buffer* res = createBuffer();
  res->initialize(info);
  return res;
}

Buffer* Device::createBuffer(const BufferViewInfo& info)
{
  Buffer* res = createBuffer();
  res->initialize(info);
  return res;
}

Texture* Device::createTexture(const TextureInfo& info)
{
  Texture* res = createTexture();
  res->initialize(info);
  return res;
}

Texture* Device::createTexture(const TextureViewInfo& info)
{
  Texture* res = createTexture();
  res->initialize(info);
  return res;
}

Shader* Device::createShader(const ShaderInfo& info)
{
  Shader* res = createShader();
  res->initialize(info);
  return res;
}

InputAssembler* Device::createInputAssembler(const InputAssemblerInfo& info)
{
  InputAssembler* res = createInputAssembler();
  res->initialize(info);
  return res;
}

RenderPass* Device::createRenderPass(const RenderPassInfo& info)
{
  RenderPass* res = createRenderPass();
  res->initialize(info);
  return res;
}

Framebuffer* Device::createFramebuffer(const FramebufferInfo& info)
{
  Framebuffer* res = createFramebuffer();
  res->initialize(info);
  return res;
}

DescriptorSet* Device::createDescriptorSet(const DescriptorSetInfo& info)
{
  DescriptorSet* res = createDescriptorSet();
  res->initialize(info);
  return res;
}

DescriptorSetLayout* Device::createDescriptorSetLayout(const DescriptorSetLayoutInfo& info)
{
  DescriptorSetLayout* res = createDescriptorSetLayout();
  res->initialize(info);
  return res;
}

PipelineLayout* Device::createPipelineLayout(const PipelineLayoutInfo& info)
{
  PipelineLayout* res = createPipelineLayout();
  res->initialize(info);
  return res;
}

PipelineState* Device::createPipelineState(const PipelineStateInfo& info)
{
  PipelineState* res = createPipelineState();
  res->initialize(info);
  return res;
}

Sampler* Device::getSampler(const SamplerInfo& info)
{
  ezUInt32 hash = Sampler::computeHash(info);
  if (!_samplers.Contains(hash))
  {
    _samplers[hash] = createSampler(info, hash);
  }
  return _samplers[hash];
}

GlobalBarrier* Device::getGlobalBarrier(const GlobalBarrierInfo& info)
{
  ezUInt32 hash = GlobalBarrier::computeHash(info);
  if (!_globalBarriers.Contains(hash))
  {
    _globalBarriers[hash] = createGlobalBarrier(info, hash);
  }
  return _globalBarriers[hash];
}

TextureBarrier* Device::getTextureBarrier(const TextureBarrierInfo& info)
{
  ezUInt32 hash = TextureBarrier::computeHash(info);
  if (!_textureBarriers.Contains(hash))
  {
    _textureBarriers[hash] = createTextureBarrier(info, hash);
  }
  return _textureBarriers[hash];
}

void Device::copyBuffersToTexture(const BufferDataList& buffers, Texture* dst, const BufferTextureCopyList& regions)
{
  copyBuffersToTexture(buffers.GetData(), dst, regions.GetData(), regions.GetCount());
}

void Device::flushCommands(const ezDynamicArray<CommandBuffer*>& cmdBuffs)
{
  flushCommands(cmdBuffs.GetData(), cmdBuffs.GetCount());
}

void Device::acquire(const ezDynamicArray<Swapchain*>& swapchains)
{
  acquire(swapchains.GetData(), swapchains.GetCount());
}
