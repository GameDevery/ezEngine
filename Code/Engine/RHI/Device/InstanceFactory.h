#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Device/Instance.h>

struct EZ_RHI_DLL ezRHIInstanceFactory
{
  using CreatorFunc = ezDelegate<ezSharedPtr<ezRHIInstance>()>;

  static ezSharedPtr<ezRHIInstance> CreateInstance(ezRHIRenderAPI type);

  static void RegisterCreatorFunc(ezRHIRenderAPI type, const CreatorFunc& func);
  static void UnregisterCreatorFunc(ezRHIRenderAPI type);
};

