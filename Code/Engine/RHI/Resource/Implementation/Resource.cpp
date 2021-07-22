#include <RHIPCH.h>

#include <RHI/Resource/Resource.h>

ezRHIResource::ezRHIResource()
  : m_ResourceStateTracker(*this)
{
}

ezRHIResourceType ezRHIResource::GetResourceType() const
{
  return m_ResourceType;
}

ezRHIResourceFormat::Enum ezRHIResource::GetFormat() const
{
  return m_Format;
}

ezRHIMemoryType ezRHIResource::GetMemoryType() const
{
  return m_MemoryType;
}

void ezRHIResource::UpdateUploadBuffer(ezUInt64 bufferOffset, const void* pData, ezUInt64 numBytes)
{
  void* pDstData = Map() + bufferOffset;
  memcpy(pDstData, pData, numBytes);
  Unmap();
}

void ezRHIResource::UpdateUploadBufferWithTextureData(ezUInt64 bufferOffset, ezUInt32 bufferRowPitch, ezUInt32 bufferDepthPitch, const void* pSrcData, ezUInt32 srcRowPitch, ezUInt32 srcDepthPitch, ezUInt32 numRows, ezUInt32 numSlices)
{
  void* pDstData = Map() + bufferOffset;
  for (ezUInt32 z = 0; z < numSlices; ++z)
  {
    ezUInt8* pDestSlice = reinterpret_cast<ezUInt8*>(pDstData) + bufferDepthPitch * z;
    const ezUInt8* pSrcSlice = reinterpret_cast<const ezUInt8*>(pSrcData) + srcDepthPitch * z;
    for (ezUInt32 y = 0; y < numRows; ++y)
    {
      memcpy(pDestSlice + bufferRowPitch * y, pSrcSlice + srcRowPitch * y, srcRowPitch);
    }
  }
  Unmap();
}

ezBitflags<ezRHIResourceState> ezRHIResource::GetInitialState() const
{
  return m_InitialState;
}

bool ezRHIResource::IsBackBuffer() const
{
  return m_IsBackBuffer;
}

void ezRHIResource::SetInitialState(ezBitflags<ezRHIResourceState> state)
{
  m_InitialState = state;
  m_ResourceStateTracker.SetResourceState(m_InitialState);
}

ezRHIResourceStateTracker& ezRHIResource::GetGlobalResourceStateTracker()
{
  return m_ResourceStateTracker;
}

const ezRHIResourceStateTracker& ezRHIResource::GetGlobalResourceStateTracker() const
{
  return m_ResourceStateTracker;
}
