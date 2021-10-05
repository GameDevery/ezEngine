#pragma once

#include <RHI/Base/DefCommon.h>

struct SwapchainTextureInfo
{
  Swapchain* swapchain{nullptr};
  Format format{Format::UNKNOWN};
  ezUInt32 width{0};
  ezUInt32 height{0};
};

constexpr TextureUsage TEXTURE_USAGE_TRANSIENT = static_cast<TextureUsage>(
  static_cast<ezUInt32>(TextureUsageBit::COLOR_ATTACHMENT) |
  static_cast<ezUInt32>(TextureUsageBit::DEPTH_STENCIL_ATTACHMENT) |
  static_cast<ezUInt32>(TextureUsageBit::INPUT_ATTACHMENT));

constexpr DescriptorType DESCRIPTOR_BUFFER_TYPE = static_cast<DescriptorType>(
  static_cast<ezUInt32>(DescriptorType::STORAGE_BUFFER) |
  static_cast<ezUInt32>(DescriptorType::DYNAMIC_STORAGE_BUFFER) |
  static_cast<ezUInt32>(DescriptorType::UNIFORM_BUFFER) |
  static_cast<ezUInt32>(DescriptorType::DYNAMIC_UNIFORM_BUFFER));

constexpr DescriptorType DESCRIPTOR_TEXTURE_TYPE = static_cast<DescriptorType>(
  static_cast<ezUInt32>(DescriptorType::SAMPLER_TEXTURE) |
  static_cast<ezUInt32>(DescriptorType::SAMPLER) |
  static_cast<ezUInt32>(DescriptorType::TEXTURE) |
  static_cast<ezUInt32>(DescriptorType::STORAGE_IMAGE) |
  static_cast<ezUInt32>(DescriptorType::INPUT_ATTACHMENT));

constexpr DescriptorType DESCRIPTOR_DYNAMIC_TYPE = static_cast<DescriptorType>(
  static_cast<ezUInt32>(DescriptorType::DYNAMIC_STORAGE_BUFFER) |
  static_cast<ezUInt32>(DescriptorType::DYNAMIC_UNIFORM_BUFFER));

constexpr ezUInt32 DRAW_INFO_SIZE = 28U;

extern const FormatInfo GFX_FORMAT_INFOS[];
extern const ezUInt32 GFX_TYPE_SIZES[];

extern ezUInt32 FormatSize(Format format, ezUInt32 width, ezUInt32 height, ezUInt32 depth);

extern ezUInt32 FormatSurfaceSize(Format format, ezUInt32 width, ezUInt32 height, ezUInt32 depth, ezUInt32 mips);
