#pragma once

#include <RHI/RHIDLL.h>
#include <Foundation/Logging/Log.h>

#define EZ_VULKAN_API EZ_RHI_DLL

#if EZ_ENABLED(EZ_COMPILE_FOR_DEBUG)
#define VK_CHECK(x)                                                      \
    do {                                                                 \
        VkResult err = x;                                                \
        if (err) {                                                       \
            ezLog::Error("{0} returned Vulkan error: {1}", #x, err);     \
            EZ_ASSERT_DEV(false, "Vulkan Error");                        \
        }                                                                \
    } while (0)

#else
#define VK_CHECK(x) x
#endif
