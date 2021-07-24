#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Instance/BaseTypes.h>
#include <RHI/Instance/QueryInterface.h>
#include <memory>

class Resource;

class EZ_RHI_DLL View : public QueryInterface
{
public:
  virtual ~View() = default;
  virtual std::shared_ptr<Resource> GetResource() = 0;
  virtual ezUInt32 GetDescriptorId() const = 0;
  virtual ezUInt32 GetBaseMipLevel() const = 0;
  virtual ezUInt32 GetLevelCount() const = 0;
  virtual ezUInt32 GetBaseArrayLayer() const = 0;
  virtual ezUInt32 GetLayerCount() const = 0;
};
