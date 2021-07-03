#include <RHIDX12/Adapter/DXAdapter.h>
#include <RHIDX12/CommandQueue/DXCommandQueue.h>
#include <RHIDX12/Device/DXDevice.h>
#include <RHIDX12/Instance/DXInstance.h>
#include <RHIDX12/Resource/DXResource.h>
#include <RHIDX12/Swapchain/DXSwapchain.h>
#include <RHIDX12/Utilities/DXUtility.h>

DXSwapchain::DXSwapchain(DXCommandQueue& command_queue, Window window, uint32_t width, uint32_t height, uint32_t frame_count, bool vsync)
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
    ezSharedPtr<DXResource> res = EZ_DEFAULT_NEW(DXResource, command_queue.GetDevice());
    ComPtr<ID3D12Resource> back_buffer;
    EZ_ASSERT_ALWAYS(m_swap_chain->GetBuffer(i, IID_PPV_ARGS(&back_buffer)) == S_OK, "");
    res->format = GetFormat();
    res->SetInitialState(ResourceState::kPresent);
    res->resource = back_buffer;
    res->desc = back_buffer->GetDesc();
    res->is_back_buffer = true;
    m_back_buffers.emplace_back(res);
  }
}

ezRHIResourceFormat::Enum DXSwapchain::GetFormat() const
{
  return ezRHIResourceFormat::R8G8B8A8_UNORM;
}

ezSharedPtr<Resource> DXSwapchain::GetBackBuffer(uint32_t buffer)
{
  return m_back_buffers[buffer];
}

uint32_t DXSwapchain::NextImage(const ezSharedPtr<Fence>& fence, uint64_t signal_value)
{
  uint32_t frame_index = m_swap_chain->GetCurrentBackBufferIndex();
  m_command_queue.Signal(fence, signal_value);
  return frame_index;
}

void DXSwapchain::Present(const ezSharedPtr<Fence>& fence, uint64_t wait_value)
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
