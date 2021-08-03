#pragma once

#include <NavigationPlugin/NavigationPluginDLL.h>

#include <Core/ResourceManager/ResourceHandle.h>
#include <Core/World/WorldModule.h>

struct ezResourceEvent;

using ezNavigationMeshResourceHandle = ezTypedResourceHandle<class ezNavigationMeshResource>;

class EZ_NAVIGATIONPLUGIN_DLL ezNavigationWorldModule : public ezWorldModule
{
  EZ_DECLARE_WORLD_MODULE();
  EZ_ADD_DYNAMIC_REFLECTION(ezNavigationWorldModule, ezWorldModule);

public:
  ezNavigationWorldModule(ezWorld* pWorld);
  ~ezNavigationWorldModule();

  virtual void Initialize() override;
  virtual void Deinitialize() override;
};
