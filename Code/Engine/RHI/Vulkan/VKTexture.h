#pragma once

#include<RHI/Vulkan/VKStd.h>
#include<RHI/Base/Texture.h>


class CCVKGPUTexture;
class CCVKGPUTextureView;

class EZ_VULKAN_API CCVKTexture final : public Texture {
public:
    CCVKTexture();
    ~CCVKTexture() override;

    inline CCVKGPUTexture *    gpuTexture() const { return _gpuTexture; }
    inline CCVKGPUTextureView *gpuTextureView() const { return _gpuTextureView; }

protected:
    friend class CCVKSwapchain;

    void doInit(const TextureInfo &info) override;
    void doInit(const TextureViewInfo &info) override;
    void doInit(const SwapchainTextureInfo &info) override;
    void doDestroy() override;
    void doResize(ezUInt32 width, ezUInt32 height, ezUInt32 size) override;

    void createTextureView();

    CCVKGPUTexture *    _gpuTexture     = nullptr;
    CCVKGPUTextureView *_gpuTextureView = nullptr;
};
