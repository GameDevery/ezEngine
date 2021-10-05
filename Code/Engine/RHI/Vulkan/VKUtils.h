#pragma once

#define WIN32_LEAN_AND_MEAN

//#include "base/Macros.h"
//#include "base/StringUtil.h"
//#include "base/Utils.h"
#include <RHI/Base/Def.h>

#include <RHI/Vulkan/ThirdParty/vk_mem_alloc.h>
#include <RHI/Vulkan/ThirdParty/volk.h>

#include <RHI/Vulkan/ThirdParty/thsvs_simpler_vulkan_synchronization.h>

#define DEFAULT_TIMEOUT 10000000000 // 10 second

#define BARRIER_DEDUCTION_LEVEL_NONE 0
#define BARRIER_DEDUCTION_LEVEL_BASIC 1
#define BARRIER_DEDUCTION_LEVEL_FULL 2

#ifndef BARRIER_DEDUCTION_LEVEL
#  define BARRIER_DEDUCTION_LEVEL BARRIER_DEDUCTION_LEVEL_BASIC
#endif

class CCVKGPUDevice;

extern VkFormat mapVkFormat(Format format, const CCVKGPUDevice* gpuDevice);
extern VkAttachmentLoadOp mapVkLoadOp(LoadOp loadOp);
extern VkAttachmentStoreOp mapVkStoreOp(StoreOp storeOp);
extern VkBufferUsageFlagBits mapVkBufferUsageFlagBits(BufferUsage usage);
extern VkImageType mapVkImageType(TextureType type);
extern VkFormatFeatureFlags mapVkFormatFeatureFlags(TextureUsage usage);
extern VkImageUsageFlagBits mapVkImageUsageFlagBits(TextureUsage usage);
extern VkImageAspectFlags mapVkImageAspectFlags(Format format);
extern VkImageCreateFlags mapVkImageCreateFlags(TextureType type);
extern VkImageViewType mapVkImageViewType(TextureType viewType);
extern VkCommandBufferLevel mapVkCommandBufferLevel(CommandBufferType type);
extern VkDescriptorType mapVkDescriptorType(DescriptorType type);
extern VkColorComponentFlags mapVkColorComponentFlags(ColorMask colorMask);
extern VkShaderStageFlagBits mapVkShaderStageFlagBits(ShaderStageFlagBit stage);
extern VkShaderStageFlags mapVkShaderStageFlags(ShaderStageFlagBit stages);
extern SurfaceTransform mapSurfaceTransform(VkSurfaceTransformFlagBitsKHR transform);
extern ezString mapVendorName(ezUInt32 vendorID);

extern void fullPipelineBarrier(VkCommandBuffer cmdBuff);
extern VkDeviceSize roundUp(VkDeviceSize numToRound, ezUInt32 multiple);
extern bool isLayerSupported(const char* required, const ezDynamicArray<VkLayerProperties>& available);
extern bool isExtensionSupported(const char* required, const ezDynamicArray<VkExtensionProperties>& available);

extern const VkSurfaceTransformFlagsKHR TRANSFORMS_THAT_REQUIRE_FLIPPING;
extern const VkPrimitiveTopology VK_PRIMITIVE_MODES[];
extern const VkCullModeFlags VK_CULL_MODES[];
extern const VkPolygonMode VK_POLYGON_MODES[];
extern const VkCompareOp VK_CMP_FUNCS[];
extern const VkStencilOp VK_STENCIL_OPS[];
extern const VkBlendOp VK_BLEND_OPS[];
extern const VkBlendFactor VK_BLEND_FACTORS[];
extern const VkFilter VK_FILTERS[];
extern const VkSamplerMipmapMode VK_SAMPLER_MIPMAP_MODES[];
extern const VkSamplerAddressMode VK_SAMPLER_ADDRESS_MODES[];
extern const VkPipelineBindPoint VK_PIPELINE_BIND_POINTS[];
extern const ThsvsAccessType THSVS_ACCESS_TYPES[];
extern const VkResolveModeFlagBits VK_RESOLVE_MODES[];
extern const VkImageLayout VK_IMAGE_LAYOUTS[];
extern const VkStencilFaceFlags VK_STENCIL_FACE_FLAGS[];
extern const VkSampleCountFlags VK_SAMPLE_COUNT_FLAGS[];
extern const VkAccessFlags FULL_ACCESS_FLAGS;
