#include <RHIPCH.h>

#include <RHI/Device/InstanceFactory.h>

struct CreatorFuncInfo
{
  ezRHIInstanceFactory::CreatorFunc m_Func;
};

static ezMap<ezRHIRenderAPI, CreatorFuncInfo> s_CreatorFuncs;

CreatorFuncInfo* GetCreatorFuncInfo(ezRHIRenderAPI type)
{
  auto pFuncInfo = s_CreatorFuncs.GetValue(type);
  if (pFuncInfo == nullptr)
  {
    ezStringBuilder sPluginName = "ezRHI";
    if (type == ezRHIRenderAPI::DX12)
      sPluginName.Append("DX12");
    else
      sPluginName.Append("Vulkan");

    EZ_VERIFY(ezPlugin::LoadPlugin(sPluginName).Succeeded(), "Renderer plugin '{}' not found", sPluginName);

    pFuncInfo = s_CreatorFuncs.GetValue(type);
    EZ_ASSERT_DEV(pFuncInfo != nullptr, "Renderer '{}' is not registered", sPluginName.GetData());
  }

  return pFuncInfo;
}

ezSharedPtr<ezRHIInstance> ezRHIInstanceFactory::CreateInstance(ezRHIRenderAPI type)
{
  if (auto pFuncInfo = GetCreatorFuncInfo(type))
  {
    return pFuncInfo->m_Func();
  }

  return nullptr;
}

void ezRHIInstanceFactory::RegisterCreatorFunc(ezRHIRenderAPI type, const CreatorFunc& func)
{
  CreatorFuncInfo funcInfo;
  funcInfo.m_Func = func;

  if (s_CreatorFuncs.Contains(type))
  {
    EZ_REPORT_FAILURE("Creator func already registered");
    return;
  }

  s_CreatorFuncs.Insert(type, funcInfo);
}

void ezRHIInstanceFactory::UnregisterCreatorFunc(ezRHIRenderAPI type)
{
  EZ_VERIFY(s_CreatorFuncs.Remove(type), "Creator func not registered");
}
