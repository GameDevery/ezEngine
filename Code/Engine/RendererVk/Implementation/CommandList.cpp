#include <RendererVkDLL.h>

#include <RendererVk/Implementation/CommandList.h>
#include <RendererVk/Device/DeviceVk.h>

namespace ezInternal::Vk
{
  CommandList::CommandList(ezGALDeviceVk& device, CommandListType type)
    : m_Device{device}
  {
    vk::CommandBufferAllocateInfo cmdBufAllocInfo = {};
    cmdBufAllocInfo.commandPool = device.GetCmdPool(type);
    cmdBufAllocInfo.commandBufferCount = 1;
    cmdBufAllocInfo.level = vk::CommandBufferLevel::ePrimary;
    std::vector<vk::UniqueCommandBuffer> cmdBufs = device.GetVkDevice().allocateCommandBuffersUnique(cmdBufAllocInfo);
    m_CommandBuffer = std::move(cmdBufs.front());
    vk::CommandBufferBeginInfo beginInfo = {};
    m_CommandBuffer->begin(beginInfo);
  }

  void CommandList::Reset()
  {
    Close();
    vk::CommandBufferBeginInfo beginInfo = {};
    m_CommandBuffer->begin(beginInfo);
    m_Closed = false;
    //m_State = nullptr;
    //m_BindingSet = nullptr;
  }

  void CommandList::Close()
  {
    if (!m_Closed)
    {
      m_CommandBuffer->end();
      m_Closed = true;
    }
  }
} // namespace ezInternal::Vk
