#pragma once

#include <RHI/Base/Object.h>

class EZ_RHI_DLL RenderPass : public RHIObject
{
public:
  RenderPass();
  ~RenderPass() override;

  static ezUInt32 computeHash(const RenderPassInfo& info);

  void initialize(const RenderPassInfo& info);
  void destroy();

  inline const ColorAttachmentList& getColorAttachments() const { return _colorAttachments; }
  inline const DepthStencilAttachment& getDepthStencilAttachment() const { return _depthStencilAttachment; }
  inline const SubpassInfoList& getSubpasses() const { return _subpasses; }
  inline const SubpassDependencyList& getDependencies() const { return _dependencies; }
  inline ezUInt32 getHash() const { return _hash; }

protected:
  ezUInt32 computeHash();

  virtual void doInit(const RenderPassInfo& info) = 0;
  virtual void doDestroy() = 0;

  ColorAttachmentList _colorAttachments;
  DepthStencilAttachment _depthStencilAttachment;
  SubpassInfoList _subpasses;
  SubpassDependencyList _dependencies;
  ezUInt32 _hash = 0;
};
