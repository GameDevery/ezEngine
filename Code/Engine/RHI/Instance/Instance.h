#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Instance/BaseTypes.h>
#include <RHI/Adapter/Adapter.h>
#include <memory>
#include <string>
#include <vector>

class EZ_RHI_DLL Instance : public ezRefCounted
{
public:
    virtual ~Instance() = default;
  virtual ezDynamicArray<ezSharedPtr<Adapter>> EnumerateAdapters() = 0;
};

struct EZ_RHI_DLL InstanceFactory
{
  using CreatorFunc = ezDelegate<ezSharedPtr<Instance>()>;

  static ezSharedPtr<Instance> CreateInstance(ApiType type);

  static void RegisterCreatorFunc(ApiType type, const CreatorFunc& func);
  static void UnregisterCreatorFunc(ApiType type);
};

