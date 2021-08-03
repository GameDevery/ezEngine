#pragma once

#include <Core/ResourceManager/Resource.h>
#include <NavigationPlugin/NavigationPluginDLL.h>


using ezNavigationMeshResourceHandle = ezTypedResourceHandle<class ezNavigationMeshResource>;

struct EZ_NAVIGATIONPLUGIN_DLL ezNavigationMeshResourceDescriptor
{
  ezNavigationMeshResourceDescriptor();
  ezNavigationMeshResourceDescriptor(const ezNavigationMeshResourceDescriptor& rhs) = delete;
  ezNavigationMeshResourceDescriptor(ezNavigationMeshResourceDescriptor&& rhs);
  ~ezNavigationMeshResourceDescriptor();
  void operator=(ezNavigationMeshResourceDescriptor&& rhs);
  void operator=(const ezNavigationMeshResourceDescriptor& rhs) = delete;

  ezResult Serialize(ezStreamWriter& stream) const;
  ezResult Deserialize(ezStreamReader& stream);
};

class EZ_NAVIGATIONPLUGIN_DLL ezNavigationMeshResource : public ezResource
{
  EZ_ADD_DYNAMIC_REFLECTION(ezNavigationMeshResource, ezResource);
  EZ_RESOURCE_DECLARE_COMMON_CODE(ezNavigationMeshResource);
  EZ_RESOURCE_DECLARE_CREATEABLE(ezNavigationMeshResource, ezNavigationMeshResourceDescriptor);

public:
  ezNavigationMeshResource();
  ~ezNavigationMeshResource();


private:
  virtual ezResourceLoadDesc UnloadData(Unload WhatToUnload) override;
  virtual ezResourceLoadDesc UpdateContent(ezStreamReader* Stream) override;
  virtual void UpdateMemoryUsage(MemoryUsage& out_NewMemoryUsage) override;
};
