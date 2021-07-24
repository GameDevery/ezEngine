#include <RHIHighLevelRenderer/HighLevelRenderer/BufferLayout.h>

ViewProvider::ViewProvider(RenderDevice& device, const ezUInt8* src_data, BufferLayout& layout)
  : m_device(device)
  , m_src_data(src_data)
  , m_layout(layout)
{
  m_dst_data.SetCountUninitialized(layout.dst_buffer_size);
}

std::shared_ptr<ResourceLazyViewDesc> ViewProvider::GetView(RenderCommandList& command_list)
{
  if (SyncData() || !m_last_view)
  {
    ezSharedPtr<Resource> resource;
    if (!m_free_resources.empty())
    {
      resource = m_free_resources.back();
      m_free_resources.pop_back();
    }
    else
    {
      resource = m_device.CreateBuffer(BindFlag::kConstantBuffer, static_cast<ezUInt32>(m_layout.dst_buffer_size), MemoryType::kUpload);
    }
    resource->UpdateUploadBuffer(0, m_dst_data.GetData(), m_dst_data.GetCount());
    m_last_view = std::make_shared<ResourceLazyViewDesc>(*this, resource);
  }
  return m_last_view;
}

void ViewProvider::OnDestroy(ResourceLazyViewDesc& view_desc)
{
  m_free_resources.emplace_back(view_desc.resource);
}

bool ViewProvider::SyncData()
{
  bool dirty = false;
  for (size_t i = 0; i < m_layout.data_size.size(); ++i)
  {
    const ezUInt8* ptr_src = m_src_data + m_layout.src_offset[i];
    ezUInt8* ptr_dst = m_dst_data.GetData() + m_layout.dst_offset[i];

    if (ezMemoryUtils::Compare(ptr_dst, ptr_src, m_layout.data_size[i]) != 0)
    {
      ezMemoryUtils::Copy(ptr_dst, ptr_src, m_layout.data_size[i]);
      dirty = true;
    }
  }
  return dirty;
}
