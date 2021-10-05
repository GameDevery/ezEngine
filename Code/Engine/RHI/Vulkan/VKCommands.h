#pragma once

#include <RHI/Vulkan/VKGPUObjects.h>

class CCVKDevice;

EZ_VULKAN_API void cmdFuncCCVKGetDeviceQueue(CCVKDevice* device, CCVKGPUQueue* gpuQueue);

EZ_VULKAN_API void cmdFuncCCVKCreateTexture(CCVKDevice* device, CCVKGPUTexture* gpuTexture);
EZ_VULKAN_API void cmdFuncCCVKCreateTextureView(CCVKDevice* device, CCVKGPUTextureView* gpuTextureView);
EZ_VULKAN_API void cmdFuncCCVKCreateSampler(CCVKDevice* device, CCVKGPUSampler* gpuSampler);
EZ_VULKAN_API void cmdFuncCCVKCreateBuffer(CCVKDevice* device, CCVKGPUBuffer* gpuBuffer);
EZ_VULKAN_API void cmdFuncCCVKCreateRenderPass(CCVKDevice* device, CCVKGPURenderPass* gpuRenderPass);
EZ_VULKAN_API void cmdFuncCCVKCreateFramebuffer(CCVKDevice* device, CCVKGPUFramebuffer* gpuFramebuffer);
EZ_VULKAN_API void cmdFuncCCVKCreateShader(CCVKDevice* device, CCVKGPUShader* gpuShader);
EZ_VULKAN_API void cmdFuncCCVKCreateDescriptorSetLayout(CCVKDevice* device, CCVKGPUDescriptorSetLayout* gpuDescriptorSetLayout);
EZ_VULKAN_API void cmdFuncCCVKCreatePipelineLayout(CCVKDevice* device, CCVKGPUPipelineLayout* gpuPipelineLayout);
EZ_VULKAN_API void cmdFuncCCVKCreateGraphicsPipelineState(CCVKDevice* device, CCVKGPUPipelineState* gpuPipelineState);
EZ_VULKAN_API void cmdFuncCCVKCreateComputePipelineState(CCVKDevice* device, CCVKGPUPipelineState* gpuPipelineState);

EZ_VULKAN_API void cmdFuncCCVKUpdateBuffer(CCVKDevice* device, CCVKGPUBuffer* gpuBuffer, const void* buffer, ezUInt32 size, const CCVKGPUCommandBuffer* cmdBuffer = nullptr);
EZ_VULKAN_API void cmdFuncCCVKCopyBuffersToTexture(CCVKDevice* device, const uint8_t* const* buffers, CCVKGPUTexture* gpuTexture, const BufferTextureCopy* regions, ezUInt32 count, const CCVKGPUCommandBuffer* gpuCommandBuffer);
EZ_VULKAN_API void cmdFuncCCVKCopyTextureToBuffers(CCVKDevice* device, CCVKGPUTexture* srcTexture, CCVKGPUBuffer* destBuffer, const BufferTextureCopy* regions, ezUInt32 count, const CCVKGPUCommandBuffer* gpuCommandBuffer);

EZ_VULKAN_API void cmdFuncCCVKDestroyRenderPass(CCVKGPUDevice* device, CCVKGPURenderPass* gpuRenderPass);
EZ_VULKAN_API void cmdFuncCCVKDestroySampler(CCVKGPUDevice* device, CCVKGPUSampler* gpuSampler);
EZ_VULKAN_API void cmdFuncCCVKDestroyShader(CCVKGPUDevice* device, CCVKGPUShader* gpuShader);
EZ_VULKAN_API void cmdFuncCCVKDestroyFramebuffer(CCVKGPUDevice* device, CCVKGPUFramebuffer* gpuFramebuffer);
EZ_VULKAN_API void cmdFuncCCVKDestroyDescriptorSetLayout(CCVKGPUDevice* device, CCVKGPUDescriptorSetLayout* gpuDescriptorSetLayout);
EZ_VULKAN_API void cmdFuncCCVKDestroyPipelineLayout(CCVKGPUDevice* device, CCVKGPUPipelineLayout* gpuPipelineLayout);
EZ_VULKAN_API void cmdFuncCCVKDestroyPipelineState(CCVKGPUDevice* device, CCVKGPUPipelineState* gpuPipelineState);

EZ_VULKAN_API void cmdFuncCCVKImageMemoryBarrier(const CCVKGPUCommandBuffer* gpuCommandBuffer, const ThsvsImageBarrier& imageBarrier);
EZ_VULKAN_API void cmdFuncCCVKGlobalMemoryBarrier(const CCVKGPUCommandBuffer* gpuCommandBuffer, const ThsvsGlobalBarrier& globalBarrier);
