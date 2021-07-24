#pragma once

#include <Foundation/Types/SharedPtr.h>
#include <RendererVk/RendererVkDLL.h>

#include <vulkan/vulkan.hpp>

namespace ezInternal::Vk
{
  constexpr const char* win32SurfaceExtension = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;

} // namespace ezInternal::Vk
