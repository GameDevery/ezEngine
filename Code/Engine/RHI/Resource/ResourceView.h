#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Descriptors/Descriptors.h>

class EZ_RHI_DLL ezRHIResourceView : public ezRHIObject<ezRHIResourceViewCreationDescription>
{
public:
  virtual ~ezRHIResourceView() = default;

  virtual ezRHIResourceHandle GetResource() = 0;
  virtual ezUInt32 GetDescriptorId() const = 0;
  virtual ezUInt32 GetBaseMipLevel() const = 0;
  virtual ezUInt32 GetLevelCount() const = 0;
  virtual ezUInt32 GetBaseArrayLayer() const = 0;
  virtual ezUInt32 GetLayerCount() const = 0;
};
