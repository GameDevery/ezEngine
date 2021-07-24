#pragma once
#include <RHIDX12/RHIDX12DLL.h>
#include <RHI/CommandQueue/CommandQueue.h>
#include <directx/d3d12.h>
#include <wrl.h>
using namespace Microsoft::WRL;

class DXDevice;

class DXCommandQueue : public CommandQueue
{
public:
    DXCommandQueue(DXDevice& device, CommandListType type);
    void Wait(const std::shared_ptr<Fence>& fence, ezUInt64 value) override;
    void Signal(const std::shared_ptr<Fence>& fence, ezUInt64 value) override;
    void ExecuteCommandLists(const std::vector<ezSharedPtr<CommandList>>& command_lists) override;

    DXDevice& GetDevice();
    ComPtr<ID3D12CommandQueue> GetQueue();

private:
    DXDevice& m_device;
    ComPtr<ID3D12CommandQueue> m_command_queue;
};
