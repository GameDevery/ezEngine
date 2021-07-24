#pragma once

#include <RendererVk/RendererVkDLL.h>
#include <vulkan/vulkan.hpp>

namespace ezInternal::Vk
{
  class CommandList
  {
  public:
    CommandList(ezGALDeviceVk& device, CommandListType type);
    void Reset();
    void Close();

    EZ_ALWAYS_INLINE vk::CommandBuffer GetCommandBuffer() const
    {
      return m_CommandBuffer.get();
    }

  private:
    ezGALDeviceVk& m_Device;
    vk::UniqueCommandBuffer m_CommandBuffer;
    bool m_Closed = false;
  };
} // namespace ezInternal::Vk
