#pragma once

#include <RHI/Base/Object.h>
#include <RHI/Base/InputAssembler.h>
#include <RHI/Base/Buffer.h>

class EZ_RHI_DLL CommandBuffer : public RHIObject
{
public:
  CommandBuffer();
  ~CommandBuffer() override;

  void initialize(const CommandBufferInfo& info);
  void destroy();

  virtual void begin(RenderPass* renderPass, ezUInt32 subpass, Framebuffer* frameBuffer) = 0;
  virtual void end() = 0;
  virtual void beginRenderPass(RenderPass* renderPass, Framebuffer* fbo, const Rect& renderArea, const Color* colors, float depth, ezUInt32 stencil, CommandBuffer* const* secondaryCBs, ezUInt32 secondaryCBCount) = 0;
  virtual void endRenderPass() = 0;
  virtual void bindPipelineState(PipelineState* pso) = 0;
  virtual void bindDescriptorSet(ezUInt32 set, DescriptorSet* descriptorSet, ezUInt32 dynamicOffsetCount, const ezUInt32* dynamicOffsets) = 0;
  virtual void bindInputAssembler(InputAssembler* ia) = 0;
  virtual void setViewport(const Viewport& vp) = 0;
  virtual void setScissor(const Rect& rect) = 0;
  virtual void setLineWidth(float width) = 0;
  virtual void setDepthBias(float constant, float clamp, float slope) = 0;
  virtual void setBlendConstants(const Color& constants) = 0;
  virtual void setDepthBound(float minBounds, float maxBounds) = 0;
  virtual void setStencilWriteMask(StencilFace face, ezUInt32 mask) = 0;
  virtual void setStencilCompareMask(StencilFace face, ezUInt32 ref, ezUInt32 mask) = 0;
  virtual void nextSubpass() = 0;
  virtual void draw(const DrawInfo& info) = 0;
  virtual void updateBuffer(Buffer* buff, const void* data, ezUInt32 size) = 0;
  virtual void copyBuffersToTexture(const uint8_t* const* buffers, Texture* texture, const BufferTextureCopy* regions, ezUInt32 count) = 0;
  virtual void blitTexture(Texture* srcTexture, Texture* dstTexture, const TextureBlit* regions, ezUInt32 count, Filter filter) = 0;
  virtual void execute(CommandBuffer* const* cmdBuffs, ezUInt32 count) = 0;
  virtual void dispatch(const DispatchInfo& info) = 0;
  virtual void pipelineBarrier(const GlobalBarrier* barrier, const TextureBarrier* const* textureBarriers, const Texture* const* textures, ezUInt32 textureBarrierCount) = 0;

  inline void begin();
  inline void begin(RenderPass* renderPass);
  inline void begin(RenderPass* renderPass, ezUInt32 subpass);

  inline void updateBuffer(Buffer* buff, const void* data);

  inline void execute(const CommandBufferList& cmdBuffs, ezUInt32 count);

  inline void bindDescriptorSet(ezUInt32 set, DescriptorSet* descriptorSet);
  inline void bindDescriptorSet(ezUInt32 set, DescriptorSet* descriptorSet, const ezDynamicArray<ezUInt32>& dynamicOffsets);

  inline void beginRenderPass(RenderPass* renderPass, Framebuffer* fbo, const Rect& renderArea, const ColorList& colors, float depth, ezUInt32 stencil, const CommandBufferList& secondaryCBs);
  inline void beginRenderPass(RenderPass* renderPass, Framebuffer* fbo, const Rect& renderArea, const ColorList& colors, float depth, ezUInt32 stencil);
  inline void beginRenderPass(RenderPass* renderPass, Framebuffer* fbo, const Rect& renderArea, const Color* colors, float depth, ezUInt32 stencil);

  inline void draw(InputAssembler* ia);
  inline void copyBuffersToTexture(const BufferDataList& buffers, Texture* texture, const BufferTextureCopyList& regions);

  inline void blitTexture(Texture* srcTexture, Texture* dstTexture, const TextureBlitList& regions, Filter filter);

  inline void pipelineBarrier(const GlobalBarrier* barrier);
  inline void pipelineBarrier(const GlobalBarrier* barrier, const TextureBarrierList& textureBarriers, const TextureList& textures);

  inline Queue* getQueue() const { return _queue; }
  inline CommandBufferType getType() const { return _type; }

  virtual ezUInt32 getNumDrawCalls() const { return _numDrawCalls; }
  virtual ezUInt32 getNumInstances() const { return _numInstances; }
  virtual ezUInt32 getNumTris() const { return _numTriangles; }

protected:
  virtual void doInit(const CommandBufferInfo& info) = 0;
  virtual void doDestroy() = 0;

  Queue* _queue = nullptr;
  CommandBufferType _type = CommandBufferType::PRIMARY;

  ezUInt32 _numDrawCalls = 0;
  ezUInt32 _numInstances = 0;
  ezUInt32 _numTriangles = 0;
};

//////////////////////////////////////////////////////////////////////////

void CommandBuffer::begin()
{
  begin(nullptr, 0, nullptr);
}

void CommandBuffer::begin(RenderPass* renderPass)
{
  begin(renderPass, 0, nullptr);
}

void CommandBuffer::begin(RenderPass* renderPass, ezUInt32 subpass)
{
  begin(renderPass, subpass, nullptr);
}

void CommandBuffer::updateBuffer(Buffer* buff, const void* data)
{
  updateBuffer(buff, data, buff->getSize());
}

void CommandBuffer::execute(const CommandBufferList& cmdBuffs, ezUInt32 count)
{
  execute(cmdBuffs.GetData(), count);
}

void CommandBuffer::bindDescriptorSet(ezUInt32 set, DescriptorSet* descriptorSet)
{
  bindDescriptorSet(set, descriptorSet, 0, nullptr);
}

void CommandBuffer::bindDescriptorSet(ezUInt32 set, DescriptorSet* descriptorSet, const ezDynamicArray<ezUInt32>& dynamicOffsets)
{
  bindDescriptorSet(set, descriptorSet, dynamicOffsets.GetCount(), dynamicOffsets.GetData());
}

void CommandBuffer::beginRenderPass(RenderPass* renderPass, Framebuffer* fbo, const Rect& renderArea, const ColorList& colors, float depth, ezUInt32 stencil, const CommandBufferList& secondaryCBs)
{
  beginRenderPass(renderPass, fbo, renderArea, colors.GetData(), depth, stencil, secondaryCBs.GetData(), secondaryCBs.GetCount());
}

void CommandBuffer::beginRenderPass(RenderPass* renderPass, Framebuffer* fbo, const Rect& renderArea, const ColorList& colors, float depth, ezUInt32 stencil)
{
  beginRenderPass(renderPass, fbo, renderArea, colors.GetData(), depth, stencil, nullptr, 0);
}

void CommandBuffer::beginRenderPass(RenderPass* renderPass, Framebuffer* fbo, const Rect& renderArea, const Color* colors, float depth, ezUInt32 stencil)
{
  beginRenderPass(renderPass, fbo, renderArea, colors, depth, stencil, nullptr, 0);
}

void CommandBuffer::draw(InputAssembler* ia)
{
  draw(ia->getDrawInfo());
}

void CommandBuffer::copyBuffersToTexture(const BufferDataList& buffers, Texture* texture, const BufferTextureCopyList& regions)
{
  copyBuffersToTexture(buffers.GetData(), texture, regions.GetData(), regions.GetCount());
}

void CommandBuffer::blitTexture(Texture* srcTexture, Texture* dstTexture, const TextureBlitList& regions, Filter filter)
{
  blitTexture(srcTexture, dstTexture, regions.GetData(), regions.GetCount(), filter);
}

void CommandBuffer::pipelineBarrier(const GlobalBarrier* barrier)
{
  pipelineBarrier(barrier, nullptr, nullptr, 0U);
}

void CommandBuffer::pipelineBarrier(const GlobalBarrier* barrier, const TextureBarrierList& textureBarriers, const TextureList& textures)
{
  pipelineBarrier(barrier, textureBarriers.GetData(), textures.GetData(), textureBarriers.GetCount());
}
