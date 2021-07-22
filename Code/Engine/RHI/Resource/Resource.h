#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Resource/ResourceStateTracker.h>

struct EZ_RHI_DLL ezRHIMemoryRequirements
{
  ezUInt64 m_Size;
  ezUInt64 m_Alignment;
  ezUInt32 m_MemoryTypeBits;
};

class EZ_RHI_DLL ezRHIResource
{
public:
  ezRHIResource();
  virtual ~ezRHIResource() = default;
  virtual void CommitMemory(ezRHIMemoryType memoryType) = 0;
  virtual void BindMemory(ezRHIMemoryHandle hMemory, ezUInt64 offset) = 0;

  ezRHIResourceType GetResourceType() const;
  ezRHIResourceFormat::Enum GetFormat() const;
  ezRHIMemoryType GetMemoryType() const;

  virtual ezUInt64 GetWidth() const = 0;
  virtual ezUInt32 GetHeight() const = 0;
  virtual ezUInt16 GetLayerCount() const = 0;
  virtual ezUInt16 GetLevelCount() const = 0;
  virtual ezUInt32 GetSampleCount() const = 0;
  virtual ezUInt64 GetAccelerationStructureHandle() const = 0;
  virtual void SetName(const ezString& name) = 0;
  virtual ezUInt8* Map() = 0;
  virtual void Unmap() = 0;

  void UpdateUploadBuffer(ezUInt64 bufferOffset, const void* pData, ezUInt64 numBytes);
  void UpdateUploadBufferWithTextureData(ezUInt64 bufferOffset, ezUInt32 bufferRowPitch, ezUInt32 bufferDepthPitch,
    const void* pSrcData, ezUInt32 srcRowPitch, ezUInt32 srcDepthPitch, ezUInt32 numRows, ezUInt32 numSlices);

  virtual bool AllowCommonStatePromotion(ezBitflags<ezRHIResourceState> stateAfter) = 0;

  ezBitflags<ezRHIResourceState> GetInitialState() const;
  virtual ezRHIMemoryRequirements GetMemoryRequirements() const = 0;
  bool IsBackBuffer() const;


  void SetInitialState(ezBitflags<ezRHIResourceState> state);
  ezRHIResourceStateTracker& GetGlobalResourceStateTracker();
  const ezRHIResourceStateTracker& GetGlobalResourceStateTracker() const;

  protected:
  ezRHIMemoryHandle m_hMemory;
  ezRHIMemoryType m_MemoryType = ezRHIMemoryType::Default;

  ezRHIResourceFormat::Enum m_Format = ezRHIResourceFormat::Unknown;
  ezRHIResourceType m_ResourceType = ezRHIResourceType::Unknown;
  ezRHIResourceHandle m_hAccelerationStructuresMemory;
  bool m_IsBackBuffer = false;

private:
  ezRHIResourceStateTracker m_ResourceStateTracker;
  ezBitflags<ezRHIResourceState> m_InitialState = ezRHIResourceState::Unknown;
};
