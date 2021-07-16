#include <RHIPCH.h>

#include <RHI/Device/Device.h>

Device::~Device()
{
  m_FreeCommandLists.Clear();
  m_CommandLists.Clear();
}

std::shared_ptr<CommandList> Device::CreateCommandList(CommandListType type)
{
  std::shared_ptr<CommandList> commandList = nullptr;

  ezLock lock(m_CommandListsMutex);
  if (m_FreeCommandLists.IsEmpty())
  {
    commandList = CreateCommandListPlatform(type);
    m_CommandLists.PushBack(commandList);
  }
  else
  {
    commandList = m_FreeCommandLists.PeekBack();
    m_CommandLists.PopBack();
  }


  return commandList;
}
