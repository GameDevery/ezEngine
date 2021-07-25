#include <RHIHighLevelRenderer/HighLevelRenderer/RenderCommandListImpl.h>
#include <RHIHighLevelRenderer/HighLevelRenderer/RenderDeviceImpl.h>
#include <RHI/Resource/ResourceBase.h>

#include <Core/System/Window.h>

RenderDeviceImpl::RenderDeviceImpl(const RenderDeviceDesc& settings, ezWindowBase* window)
  : m_window(window)
  , m_vsync(settings.vsync)
  , m_FrameCount(settings.frame_count)
{
  m_instance = InstanceFactory::CreateInstance(settings.api_type);
  m_adapter = std::move(m_instance->EnumerateAdapters()[settings.required_gpu_index]);
  m_device = m_adapter->CreateDevice();
  m_command_queue = m_device->GetCommandQueue(CommandListType::kGraphics);
  m_object_cache = std::make_unique<ObjectCache>(*m_device);

  m_width = m_window->GetClientAreaSize().width;
  m_height = m_window->GetClientAreaSize().height;

  m_Swapchain = m_device->CreateSwapchain(m_window->GetNativeWindowHandle(), m_width, m_height, m_FrameCount, settings.vsync);
  m_pFence = m_device->CreateFence(m_FenceValue);
  for (ezUInt32 i = 0; i < m_FrameCount; ++i)
  {
    m_BarrierCommandLists.emplace_back(m_device->CreateCommandList(CommandListType::kGraphics));
    m_frame_fence_values.emplace_back(0);
  }
}

RenderDeviceImpl::~RenderDeviceImpl()
{
  m_CommandListPool.clear();
  WaitForIdle();
}

ezRHIResourceFormat::Enum RenderDeviceImpl::GetFormat() const
{
  return m_Swapchain->GetFormat();
}

ezSharedPtr<Resource> RenderDeviceImpl::GetBackBuffer(ezUInt32 buffer)
{
  return m_Swapchain->GetBackBuffer(buffer);
}

ezSharedPtr<RenderCommandList> RenderDeviceImpl::CreateRenderCommandList(CommandListType type)
{
  return EZ_DEFAULT_NEW(RenderCommandListImpl, *m_device, *m_object_cache, CommandListType::kGraphics);
}

ezSharedPtr<Resource> RenderDeviceImpl::CreateTexture(ezUInt32 bind_flag, ezRHIResourceFormat::Enum format, ezUInt32 sample_count, int width, int height, int depth, int mip_levels)
{
  auto res = m_device->CreateTexture(TextureType::k2D, bind_flag, format, sample_count, width, height, depth, mip_levels);
  res->CommitMemory(MemoryType::kDefault);
  return res;
}

ezSharedPtr<Resource> RenderDeviceImpl::CreateBuffer(ezUInt32 bind_flag, ezUInt32 buffer_size, MemoryType memory_type)
{
  auto res = m_device->CreateBuffer(bind_flag, buffer_size);
  if (res)
    res->CommitMemory(memory_type);
  return res;
}

ezSharedPtr<Resource> RenderDeviceImpl::CreateSampler(const SamplerDesc& desc)
{
  return m_device->CreateSampler(desc);
}

ezSharedPtr<Resource> RenderDeviceImpl::CreateBottomLevelAS(const std::vector<RaytracingGeometryDesc>& descs, BuildAccelerationStructureFlags flags)
{
  auto prebuild_info = m_device->GetBLASPrebuildInfo(descs, flags);
  ezSharedPtr<Resource> memory = m_device->CreateBuffer(BindFlag::kAccelerationStructure, (ezUInt32)prebuild_info.acceleration_structure_size);
  memory->CommitMemory(MemoryType::kDefault);
  return m_device->CreateAccelerationStructure(AccelerationStructureType::kBottomLevel, memory, 0);
}

ezSharedPtr<Resource> RenderDeviceImpl::CreateTopLevelAS(ezUInt32 instance_count, BuildAccelerationStructureFlags flags)
{
  auto prebuild_info = m_device->GetTLASPrebuildInfo(instance_count, flags);
  ezSharedPtr<Resource> memory = m_device->CreateBuffer(BindFlag::kAccelerationStructure, (ezUInt32)prebuild_info.acceleration_structure_size);
  memory->CommitMemory(MemoryType::kDefault);
  return m_device->CreateAccelerationStructure(AccelerationStructureType::kTopLevel, memory, 0);
}

ezSharedPtr<View> RenderDeviceImpl::CreateView(const ezSharedPtr<Resource>& resource, const ViewDesc& viewDesc)
{
  return m_device->CreateView(resource, viewDesc);
}

ezSharedPtr<Shader> RenderDeviceImpl::CreateShader(const ShaderDesc& desc, ezDynamicArray<ezUInt8> byteCode, ezSharedPtr<ShaderReflection> reflection)
{
  return m_device->CreateShader(desc, byteCode, reflection);
}

ezSharedPtr<Program> RenderDeviceImpl::CreateProgram(const ezDynamicArray<ezSharedPtr<Shader>>& shaders)
{
  return m_device->CreateProgram(shaders);
}

bool RenderDeviceImpl::IsDxrSupported() const
{
  return m_device->IsDxrSupported();
}

bool RenderDeviceImpl::IsRayQuerySupported() const
{
  return m_device->IsRayQuerySupported();
}

bool RenderDeviceImpl::IsVariableRateShadingSupported() const
{
  return m_device->IsVariableRateShadingSupported();
}

bool RenderDeviceImpl::IsMeshShadingSupported() const
{
  return m_device->IsMeshShadingSupported();
}

ezUInt32 RenderDeviceImpl::GetShadingRateImageTileSize() const
{
  return m_device->GetShadingRateImageTileSize();
}

void RenderDeviceImpl::ExecuteCommandLists(const std::vector<ezSharedPtr<RenderCommandList>>& command_lists)
{
  for (auto& command_list : command_lists)
  {
    command_list->GetFenceValue() = m_FenceValue + 1;
  }
  ExecuteCommandListsImpl(command_lists);
  m_command_queue->Signal(m_pFence, ++m_FenceValue);
}

const ezString& RenderDeviceImpl::GetGpuName() const
{
  return m_adapter->GetName();
}

void RenderDeviceImpl::ExecuteCommandListsImpl(const std::vector<ezSharedPtr<RenderCommandList>>& command_lists)
{
  std::vector<ezSharedPtr<CommandList>> rawCommandLists;
  size_t patch_cmds = 0;
  for (size_t c = 0; c < command_lists.size(); ++c)
  {
    std::vector<ResourceBarrierDesc> new_barriers;
    decltype(auto) command_list_impl = command_lists[c].Downcast<RenderCommandListImpl>();
    auto barriers = command_list_impl->GetLazyBarriers();
    for (auto& barrier : barriers)
    {
      if (c == 0 && barrier.resource->AllowCommonStatePromotion(barrier.state_after))
        continue;
      decltype(auto) resource_base = barrier.resource.Downcast<ResourceBase>();
      auto& global_state_tracker = resource_base->GetGlobalResourceStateTracker();
      if (global_state_tracker.HasResourceState() && barrier.baseMipLevel == 0 && barrier.level_count == barrier.resource->GetLevelCount() &&
          barrier.base_array_layer == 0 && barrier.layer_count == barrier.resource->GetLayerCount())
      {
        barrier.state_before = global_state_tracker.GetResourceState();
        if (barrier.state_before != barrier.state_after)
          new_barriers.emplace_back(barrier);
      }
      else
      {
        for (ezUInt32 i = 0; i < barrier.level_count; ++i)
        {
          for (ezUInt32 j = 0; j < barrier.layer_count; ++j)
          {
            barrier.state_before = global_state_tracker.GetSubresourceState(barrier.baseMipLevel + i, barrier.base_array_layer + j);
            if (barrier.state_before != barrier.state_after)
            {
              auto& new_barrier = new_barriers.emplace_back(barrier);
              new_barrier.baseMipLevel = barrier.baseMipLevel + i;
              new_barrier.level_count = 1;
              new_barrier.base_array_layer = barrier.base_array_layer + j;
              new_barrier.layer_count = 1;
            }
          }
        }
      }
    }

    if (!new_barriers.empty())
    {
      ezSharedPtr<CommandList> tmp_cmd;
      if (c != 0 && kUseFakeClose)
      {
        tmp_cmd = command_lists[c - 1].Downcast<RenderCommandListImpl>()->GetCommandList();
      }
      else
      {
        if (!m_fence_value_by_cmd.empty())
        {
          auto& desc = m_fence_value_by_cmd.front();
          if (m_pFence->GetCompletedValue() >= desc.first)
          {
            m_pFence->Wait(desc.first);
            tmp_cmd = m_CommandListPool[desc.second];
            tmp_cmd->Reset();
            m_fence_value_by_cmd.pop_front();
            m_fence_value_by_cmd.emplace_back(m_FenceValue + 1, desc.second);
          }
        }
        if (!tmp_cmd)
        {
          tmp_cmd = m_CommandListPool.emplace_back(m_device->CreateCommandList(CommandListType::kGraphics));
          m_fence_value_by_cmd.emplace_back(m_FenceValue + 1, m_CommandListPool.size() - 1);
        }
        rawCommandLists.emplace_back(tmp_cmd);
      }

      tmp_cmd->ResourceBarrier(new_barriers);
      if (!kUseFakeClose)
        tmp_cmd->Close();
      ++patch_cmds;
    }

    auto& state_trackers = command_list_impl->GetResourceStateTrackers();
    for (const auto& state_tracker_pair : state_trackers)
    {
      auto& resource = state_tracker_pair.first;
      auto& state_tracker = state_tracker_pair.second;
      decltype(auto) resource_base = resource.Downcast<ResourceBase>();
      auto& global_state_tracker = resource_base->GetGlobalResourceStateTracker();
      global_state_tracker.Merge(state_tracker);
    }

    rawCommandLists.emplace_back(command_list_impl->GetCommandList());
  }
  if (kUseFakeClose)
  {
    for (auto& cmd : rawCommandLists)
      cmd->Close();
  }
  m_command_queue->ExecuteCommandLists(rawCommandLists);
  if (patch_cmds)
  {
    m_command_queue->Signal(m_pFence, ++m_FenceValue);
  }
}

void RenderDeviceImpl::WaitForIdle()
{
  m_command_queue->Signal(m_pFence, ++m_FenceValue);
  m_pFence->Wait(m_FenceValue);
}

void RenderDeviceImpl::Resize(ezUInt32 width, ezUInt32 height)
{
  m_width = width;
  m_height = height;
  m_Swapchain.Clear();
  m_Swapchain = m_device->CreateSwapchain(m_window->GetNativeWindowHandle(), m_width, m_height, m_FrameCount, m_vsync);
  m_frame_index = 0;
}

void RenderDeviceImpl::InsertPresentBarrier()
{
  auto back_buffer = GetBackBuffer(GetFrameIndex());
  decltype(auto) resource_base = back_buffer.Downcast<ResourceBase>();
  decltype(auto) global_state_tracker = resource_base->GetGlobalResourceStateTracker();

  ResourceBarrierDesc barrier = {};
  barrier.resource = back_buffer;
  barrier.state_before = global_state_tracker.GetSubresourceState(0, 0);
  barrier.state_after = ResourceState::kPresent;
  decltype(auto) command_list = m_BarrierCommandLists[m_frame_index];
  command_list->Reset();
  command_list->ResourceBarrier({barrier});
  command_list->Close();
  m_command_queue->ExecuteCommandLists({command_list});

  global_state_tracker.SetSubresourceState(0, 0, barrier.state_after);
}

void RenderDeviceImpl::Present()
{
  InsertPresentBarrier();
  m_Swapchain->NextImage(m_pFence, ++m_FenceValue);
  m_command_queue->Wait(m_pFence, m_FenceValue);
  m_command_queue->Signal(m_pFence, m_frame_fence_values[m_frame_index] = ++m_FenceValue);
  m_Swapchain->Present(m_pFence, m_FenceValue);
  m_frame_index = (m_frame_index + 1) % m_FrameCount;
  m_pFence->Wait(m_frame_fence_values[m_frame_index]);
}

void RenderDeviceImpl::Wait(ezUInt64 fence_value)
{
  m_pFence->Wait(fence_value);
}

ezUInt32 RenderDeviceImpl::GetFrameIndex() const
{
  return m_frame_index;
}

ezSharedPtr<RenderDevice> CreateRenderDevice(const RenderDeviceDesc& settings, ezWindowBase* window)
{
  return EZ_DEFAULT_NEW(RenderDeviceImpl, settings, window);
}
