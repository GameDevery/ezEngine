#include <RHIDX12/Swapchain/DXSwapchain.h>
#include <RHIDX12/Adapter/DXAdapter.h>
#include <RHIDX12/Device/DXDevice.h>
#include <RHIDX12/CommandQueue/DXCommandQueue.h>
#include <RHIDX12/Instance/DXInstance.h>
#include <RHIDX12/Resource/DXResource.h>
#include <RHIDX12/Utilities/DXUtility.h>

DXSwapchain::DXSwapchain(DXCommandQueue& command_queue, Window window, ezUInt32 width, ezUInt32 height, ezUInt32 frame_count, bool vsync)
    : m_command_queue(command_queue)
    , m_vsync(vsync)
{
    DXInstance& instance = command_queue.GetDevice().GetAdapter().GetInstance();
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
    swap_chain_desc.Width = width;
    swap_chain_desc.Height = height;
    swap_chain_desc.Format = DXUtils::ToDXGIFormat(GetFormat()); // static_cast<DXGI_FORMAT>(gli::dx().translate(GetFormat()).DXGIFormat.DDS);
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount = frame_count;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    ComPtr<IDXGISwapChain1> tmp_swap_chain;
    EZ_ASSERT_ALWAYS(instance.GetFactory()->CreateSwapChainForHwnd(command_queue.GetQueue().Get(), reinterpret_cast<HWND>(window), &swap_chain_desc, nullptr, nullptr, &tmp_swap_chain) == S_OK, "");
    EZ_ASSERT_ALWAYS(instance.GetFactory()->MakeWindowAssociation(reinterpret_cast<HWND>(window), DXGI_MWA_NO_WINDOW_CHANGES) == S_OK, "");
    tmp_swap_chain.As(&m_swap_chain);

    for (ezUInt32 i = 0; i < frame_count; ++i)
    {
        std::shared_ptr<DXResource> res = std::make_shared<DXResource>(command_queue.GetDevice());
        ComPtr<ID3D12Resource> back_buffer;
        EZ_ASSERT_ALWAYS(m_swap_chain->GetBuffer(i, IID_PPV_ARGS(&back_buffer)) == S_OK, "");
        res->format = GetFormat();
        res->SetInitialState(ResourceState::kPresent);
        res->resource = back_buffer;
        res->desc = back_buffer->GetDesc();
        res->is_back_buffer = true;
        m_BackBuffers.PushBack(res);
    }
}

ezRHIResourceFormat::Enum DXSwapchain::GetFormat() const
{
    return ezRHIResourceFormat::R8G8B8A8_UNORM;
}

std::shared_ptr<Resource> DXSwapchain::GetBackBuffer(ezUInt32 buffer)
{
  return m_BackBuffers[buffer];
}

ezUInt32 DXSwapchain::NextImage(const std::shared_ptr<Fence>& fence, ezUInt64 signal_value)
{
    ezUInt32 frame_index = m_swap_chain->GetCurrentBackBufferIndex();
    m_command_queue.Signal(fence, signal_value);
    return frame_index;
}

void DXSwapchain::Present(const std::shared_ptr<Fence>& fence, ezUInt64 wait_value)
{
    m_command_queue.Wait(fence, wait_value);
    if (m_vsync)
    {
      EZ_ASSERT_ALWAYS(m_swap_chain->Present(1, 0) == S_OK, "");
    }
    else
    {
      EZ_ASSERT_ALWAYS(m_swap_chain->Present(0, DXGI_PRESENT_ALLOW_TEARING) == S_OK, "");
    }
}
