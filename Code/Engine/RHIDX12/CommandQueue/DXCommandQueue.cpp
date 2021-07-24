#include <RHIDX12/CommandList/DXCommandList.h>
#include <RHIDX12/Device/DXDevice.h>
#include <RHIDX12/Fence/DXFence.h>
#include <RHIDX12/CommandQueue/DXCommandQueue.h>
#include <RHIDX12/Resource/DXResource.h>
#include <RHIDX12/Utilities/DXUtility.h>

DXCommandQueue::DXCommandQueue(DXDevice& device, CommandListType type)
  : m_device(device)
{
  D3D12_COMMAND_LIST_TYPE dx_type{};
  switch (type)
  {
    case CommandListType::kGraphics:
      dx_type = D3D12_COMMAND_LIST_TYPE_DIRECT;
      break;
    case CommandListType::kCompute:
      dx_type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
      break;
    case CommandListType::kCopy:
      dx_type = D3D12_COMMAND_LIST_TYPE_COPY;
      break;
    default:
      assert(false);
      break;
  }
  D3D12_COMMAND_QUEUE_DESC queue_desc = {};
  queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
  queue_desc.Type = dx_type;
  EZ_ASSERT_ALWAYS(m_device.GetDevice()->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&m_command_queue)) == S_OK, "");
}

void DXCommandQueue::Wait(const ezSharedPtr<Fence>& fence, ezUInt64 value)
{
  ezSharedPtr<DXFence> dxFence = fence.Downcast<DXFence>();
  EZ_ASSERT_ALWAYS(m_command_queue->Wait(dxFence->GetFence().Get(), value) == S_OK, "");
}

void DXCommandQueue::Signal(const ezSharedPtr<Fence>& fence, ezUInt64 value)
{
  ezSharedPtr<DXFence> dxFence = fence.Downcast<DXFence>();
  EZ_ASSERT_ALWAYS(m_command_queue->Signal(dxFence->GetFence().Get(), value) == S_OK, "");
}

void DXCommandQueue::ExecuteCommandLists(const std::vector<ezSharedPtr<CommandList>>& commandLists)
{
  ezDynamicArray<ID3D12CommandList*> dxCommandLists;
  for (auto& commandList : commandLists)
  {
    if (!commandList)
      continue;
    ezSharedPtr<DXCommandList> dxCommandList = commandList.Downcast<DXCommandList>();
    dxCommandLists.PushBack(dxCommandList->GetCommandList().Get());
  }
  if (!dxCommandLists.IsEmpty())
    m_command_queue->ExecuteCommandLists(dxCommandLists.GetCount(), dxCommandLists.GetData());
}

DXDevice& DXCommandQueue::GetDevice()
{
  return m_device;
}

ComPtr<ID3D12CommandQueue> DXCommandQueue::GetQueue()
{
  return m_command_queue;
}
