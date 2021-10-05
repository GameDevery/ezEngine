#include <RHI/Base/RenderPass.h>

RenderPass::RenderPass()
  : RHIObject(ObjectType::RENDER_PASS)
{
}

RenderPass::~RenderPass() = default;

// Based on render pass compatibility
ezUInt32 RenderPass::computeHash()
{
  // https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
  ezUInt32 seed = 0;
  if (!_subpasses.IsEmpty())
  {
    for (const SubpassInfo& subPass : _subpasses)
    {
      for (const ezUInt32 iaIndex : subPass.inputs)
      {
        if (iaIndex >= _colorAttachments.GetCount())
          break;
        seed += 2;
      }
      for (const ezUInt32 caIndex : subPass.colors)
      {
        if (caIndex >= _colorAttachments.GetCount())
          break;
        seed += 2;
      }
      for (const ezUInt32 raIndex : subPass.resolves)
      {
        if (raIndex >= _colorAttachments.GetCount())
          break;
        seed += 2;
      }
      for (const ezUInt32 paIndex : subPass.preserves)
      {
        if (paIndex >= _colorAttachments.GetCount())
          break;
        seed += 2;
      }
      if (subPass.depthStencil != INVALID_BINDING)
      {
        seed += 2;
      }
    }
    for (const SubpassInfo& subpass : _subpasses)
    {
      for (const ezUInt32 iaIndex : subpass.inputs)
      {
        if (iaIndex >= _colorAttachments.GetCount())
          break;
        const ColorAttachment& ia = _colorAttachments[iaIndex];
        seed ^= static_cast<ezUInt32>(ia.format) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= static_cast<ezUInt32>(ia.sampleCount) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      }
      for (const ezUInt32 caIndex : subpass.colors)
      {
        if (caIndex >= _colorAttachments.GetCount())
          break;
        const ColorAttachment& ca = _colorAttachments[caIndex];
        seed ^= static_cast<ezUInt32>(ca.format) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= static_cast<ezUInt32>(ca.sampleCount) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      }
      for (const ezUInt32 raIndex : subpass.resolves)
      {
        if (raIndex >= _colorAttachments.GetCount())
          break;
        const ColorAttachment& ca = _colorAttachments[raIndex];
        seed ^= static_cast<ezUInt32>(ca.format) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= static_cast<ezUInt32>(ca.sampleCount) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      }
      for (const ezUInt32 paIndex : subpass.preserves)
      {
        if (paIndex >= _colorAttachments.GetCount())
          break;
        const ColorAttachment& ca = _colorAttachments[paIndex];
        seed ^= static_cast<ezUInt32>(ca.format) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= static_cast<ezUInt32>(ca.sampleCount) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      }
      if (subpass.depthStencil != INVALID_BINDING)
      {
        if (subpass.depthStencil < _colorAttachments.GetCount())
        {
          const auto& ds = _colorAttachments[subpass.depthStencil];
          seed ^= static_cast<ezUInt32>(ds.format) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
          seed ^= static_cast<ezUInt32>(ds.sampleCount) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        else
        {
          const auto& ds = _depthStencilAttachment;
          seed ^= static_cast<ezUInt32>(ds.format) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
          seed ^= static_cast<ezUInt32>(ds.sampleCount) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
      }
    }
  }
  else
  {
    seed = static_cast<ezUInt32>(_colorAttachments.GetCount() * 2 + 2);
    for (const ColorAttachment& colorAttachment : _colorAttachments)
    {
      seed ^= static_cast<ezUInt32>(colorAttachment.format) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      seed ^= static_cast<ezUInt32>(colorAttachment.sampleCount) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    seed ^= static_cast<ezUInt32>(_depthStencilAttachment.format) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= static_cast<ezUInt32>(_depthStencilAttachment.sampleCount) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  return seed;
}

ezUInt32 RenderPass::computeHash(const RenderPassInfo& info)
{
  static auto computeAttachmentHash = [](const ColorAttachment& attachment, ezUInt32& seed)
  {
    seed ^= static_cast<ezUInt32>(attachment.format) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= static_cast<ezUInt32>(attachment.sampleCount) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= static_cast<ezUInt32>(attachment.loadOp) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= static_cast<ezUInt32>(attachment.storeOp) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    for (AccessType type : attachment.beginAccesses)
    {
      seed ^= static_cast<ezUInt32>(type) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    for (AccessType type : attachment.endAccesses)
    {
      seed ^= static_cast<ezUInt32>(type) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
  };
  static auto computeDSAttachmentHash = [](const DepthStencilAttachment& attachment, ezUInt32& seed)
  {
    seed ^= static_cast<ezUInt32>(attachment.format) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= static_cast<ezUInt32>(attachment.sampleCount) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= static_cast<ezUInt32>(attachment.depthLoadOp) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= static_cast<ezUInt32>(attachment.depthStoreOp) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= static_cast<ezUInt32>(attachment.stencilLoadOp) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= static_cast<ezUInt32>(attachment.stencilStoreOp) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    for (AccessType type : attachment.beginAccesses)
    {
      seed ^= static_cast<ezUInt32>(type) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    for (AccessType type : attachment.endAccesses)
    {
      seed ^= static_cast<ezUInt32>(type) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
  };
  ezUInt32 seed = 0;
  if (!info.subpasses.IsEmpty())
  {
    for (const SubpassInfo& subpass : info.subpasses)
    {
      for (const ezUInt32 iaIndex : subpass.inputs)
      {
        if (iaIndex >= info.colorAttachments.GetCount())
          break;
        const ColorAttachment& ia = info.colorAttachments[iaIndex];
        seed += static_cast<ezUInt32>(4 + ia.beginAccesses.GetCount() + ia.endAccesses.GetCount());
      }
      for (const ezUInt32 caIndex : subpass.colors)
      {
        if (caIndex >= info.colorAttachments.GetCount())
          break;
        const ColorAttachment& ca = info.colorAttachments[caIndex];
        seed += static_cast<ezUInt32>(4 + ca.beginAccesses.GetCount() + ca.endAccesses.GetCount());
      }
      for (const ezUInt32 raIndex : subpass.resolves)
      {
        if (raIndex >= info.colorAttachments.GetCount())
          break;
        const ColorAttachment& ra = info.colorAttachments[raIndex];
        seed += static_cast<ezUInt32>(4 + ra.beginAccesses.GetCount() + ra.endAccesses.GetCount());
      }
      for (const ezUInt32 paIndex : subpass.preserves)
      {
        if (paIndex >= info.colorAttachments.GetCount())
          break;
        const ColorAttachment& pa = info.colorAttachments[paIndex];
        seed += static_cast<ezUInt32>(4 + pa.beginAccesses.GetCount() + pa.endAccesses.GetCount());
      }
      if (subpass.depthStencil != INVALID_BINDING)
      {
        if (subpass.depthStencil < info.colorAttachments.GetCount())
        {
          const auto& ds = info.colorAttachments[subpass.depthStencil];
          seed += static_cast<ezUInt32>(4 + ds.beginAccesses.GetCount() + ds.endAccesses.GetCount());
        }
        else
        {
          const auto& ds = info.depthStencilAttachment;
          seed += static_cast<ezUInt32>(6 + ds.beginAccesses.GetCount() + ds.endAccesses.GetCount());
        }
      }
    }
    for (const SubpassInfo& subpass : info.subpasses)
    {
      for (const ezUInt32 iaIndex : subpass.inputs)
      {
        if (iaIndex >= info.colorAttachments.GetCount())
          break;
        computeAttachmentHash(info.colorAttachments[iaIndex], seed);
      }
      for (const ezUInt32 caIndex : subpass.colors)
      {
        if (caIndex >= info.colorAttachments.GetCount())
          break;
        computeAttachmentHash(info.colorAttachments[caIndex], seed);
      }
      for (const ezUInt32 raIndex : subpass.resolves)
      {
        if (raIndex >= info.colorAttachments.GetCount())
          break;
        computeAttachmentHash(info.colorAttachments[raIndex], seed);
      }
      for (const ezUInt32 paIndex : subpass.preserves)
      {
        if (paIndex >= info.colorAttachments.GetCount())
          break;
        computeAttachmentHash(info.colorAttachments[paIndex], seed);
      }
      if (subpass.depthStencil != INVALID_BINDING)
      {
        if (subpass.depthStencil < info.colorAttachments.GetCount())
        {
          computeAttachmentHash(info.colorAttachments[subpass.depthStencil], seed);
        }
        else
        {
          computeDSAttachmentHash(info.depthStencilAttachment, seed);
        }
      }
    }
  }
  else
  {
    for (const ColorAttachment& ca : info.colorAttachments)
    {
      seed += static_cast<ezUInt32>(4 + ca.beginAccesses.GetCount() + ca.endAccesses.GetCount());
    }
    seed += static_cast<ezUInt32>(6 + info.depthStencilAttachment.beginAccesses.GetCount() + info.depthStencilAttachment.endAccesses.GetCount());

    for (const ColorAttachment& ca : info.colorAttachments)
    {
      computeAttachmentHash(ca, seed);
    }
    computeDSAttachmentHash(info.depthStencilAttachment, seed);
  }

  return seed;
}

void RenderPass::initialize(const RenderPassInfo& info)
{
  _colorAttachments = info.colorAttachments;
  _depthStencilAttachment = info.depthStencilAttachment;
  _subpasses = info.subpasses;
  _dependencies = info.dependencies;
  _hash = computeHash();

  doInit(info);
}

void RenderPass::destroy()
{
  doDestroy();

  _colorAttachments.Clear();
  _subpasses.Clear();
  _hash = 0U;
}
