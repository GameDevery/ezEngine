#pragma once

#include <RHI/Base/CommandBuffer.h>

#include <RHI/Vulkan/VKCommands.h>

class EZ_VULKAN_API CCVKCommandBuffer final : public CommandBuffer
{
public:
  CCVKCommandBuffer();
  ~CCVKCommandBuffer() override;

  void begin(RenderPass* renderPass, ezUInt32 subpass, Framebuffer* frameBuffer) override;
  void end() override;
  void beginRenderPass(RenderPass* renderPass, Framebuffer* fbo, const Rect& renderArea, const Color* colors, float depth, ezUInt32 stencil, CommandBuffer* const* secondaryCBs, ezUInt32 secondaryCBCount) override;
  void endRenderPass() override;
  void bindPipelineState(PipelineState* pso) override;
  void bindDescriptorSet(ezUInt32 set, DescriptorSet* descriptorSet, ezUInt32 dynamicOffsetCount, const ezUInt32* dynamicOffsets) override;
  void bindInputAssembler(InputAssembler* ia) override;
  void setViewport(const Viewport& vp) override;
  void setScissor(const Rect& rect) override;
  void setLineWidth(float width) override;
  void setDepthBias(float constant, float clamp, float slope) override;
  void setBlendConstants(const Color& constants) override;
  void setDepthBound(float minBounds, float maxBounds) override;
  void setStencilWriteMask(StencilFace face, ezUInt32 mask) override;
  void setStencilCompareMask(StencilFace face, ezUInt32 reference, ezUInt32 mask) override;
  void nextSubpass() override;
  void draw(const DrawInfo& info) override;
  void updateBuffer(Buffer* buffer, const void* data, ezUInt32 size) override;
  void copyBuffersToTexture(const uint8_t* const* buffers, Texture* texture, const BufferTextureCopy* regions, ezUInt32 count) override;
  void blitTexture(Texture* srcTexture, Texture* dstTexture, const TextureBlit* regions, ezUInt32 count, Filter filter) override;
  void execute(CommandBuffer* const* cmdBuffs, ezUInt32 count) override;
  void dispatch(const DispatchInfo& info) override;
  void pipelineBarrier(const GlobalBarrier* barrier, const TextureBarrier* const* textureBarriers, const Texture* const* textures, ezUInt32 textureBarrierCount) override;

  CCVKGPUCommandBuffer* gpuCommandBuffer() const { return _gpuCommandBuffer; }

protected:
  friend class CCVKQueue;

  void doInit(const CommandBufferInfo& info) override;
  void doDestroy() override;

  void bindDescriptorSets(VkPipelineBindPoint bindPoint);

  CCVKGPUCommandBuffer* _gpuCommandBuffer = nullptr;

  CCVKGPUPipelineState* _curGPUPipelineState = nullptr;
  ezDynamicArray<CCVKGPUDescriptorSet*> _curGPUDescriptorSets;
  ezDynamicArray<VkDescriptorSet> _curVkDescriptorSets;
  ezDynamicArray<ezUInt32> _curDynamicOffsets;
  ezDynamicArray<ezDynamicArray<ezUInt32>> _curDynamicOffsetsArray;
  ezUInt32 _firstDirtyDescriptorSet = UINT_MAX;

  CCVKGPUInputAssembler* _curGPUInputAssember = nullptr;
  CCVKGPUFramebuffer* _curGPUFBO = nullptr;

  bool _secondaryRP = false;

  DynamicStates _curDynamicStates;

  // temp storage
  ezDynamicArray<VkImageBlit> _blitRegions;
  ezDynamicArray<VkImageMemoryBarrier> _imageMemoryBarriers;
  ezDynamicArray<VkCommandBuffer> _vkCommandBuffers;

  ezDeque<VkCommandBuffer> _pendingQueue;
};
