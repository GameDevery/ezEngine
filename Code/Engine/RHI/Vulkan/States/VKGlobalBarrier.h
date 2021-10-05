#pragma once

#include <RHI/Base/States/GlobalBarrier.h>
#include <RHI/Vulkan/VKStd.h>

class CCVKGPUGlobalBarrier;

class EZ_VULKAN_API CCVKGlobalBarrier : public GlobalBarrier {
public:
    explicit CCVKGlobalBarrier(const GlobalBarrierInfo &info, ezUInt32 hash);
    ~CCVKGlobalBarrier() override;

    inline const CCVKGPUGlobalBarrier *gpuBarrier() const { return _gpuBarrier; }

protected:
    CCVKGPUGlobalBarrier *_gpuBarrier = nullptr;
};
