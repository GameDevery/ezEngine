#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Descriptors/Descriptors.h>

class EZ_RHI_DLL ezRHIShader : public ezRefCounted
{
public:
  virtual ~ezRHIShader() = default;

  ezRHIShader(const ezRHIShaderCreationDescription& desc);
  ezRHIShaderType GetType() const;
  const ezDynamicArray<ezUInt8>& GetBlob() const;
  ezUInt64 GetId(const ezString& entryPoint) const;
  const ezRHIBindKeyDescription& GetBindKey(const ezString& name) const;
  const ezDynamicArray<ezRHIResourceBindingDescription>& GetResourceBindings() const;
  const ezRHIResourceBindingDescription& GetResourceBinding(const ezRHIBindKeyDescription& bindKey) const;
  const ezDynamicArray<ezRHIInputElementDescription>& GetInputLayouts() const;
  ezUInt32 GetInputLayoutLocation(const ezString& semanticName) const;
  const ezDynamicArray<ezRHIBindKeyDescription>& GetBindings() const;
  const ezUniquePtr<ezRHIShaderReflection>& GetReflection() const;

protected:
  ezRHIShaderType m_ShaderType;
  ezRHIShaderBlobType m_BlobType;
  ezDynamicArray<ezUInt8> m_Blob;
  ezMap<ezString, ezUInt64> m_Ids;
  ezDynamicArray<ezRHIResourceBindingDescription> m_Bindings;
  ezDynamicArray<ezRHIBindKeyDescription> m_BindingKeys;
  ezMap<ezRHIBindKeyDescription, ezUInt32> m_Mapping;
  ezMap<ezString, ezRHIBindKeyDescription> m_BindKeys;
  ezDynamicArray<ezRHIInputElementDescription> m_InputLayoutDescs;
  ezMap<ezString, ezUInt32> m_Locations;
  ezUniquePtr<ezRHIShaderReflection> m_Reflection;
};
