#include <NavigationPluginPCH.h>

#include <Core/World/World.h>
#include <NavigationPlugin/WorldModule/NavigationWorldModule.h>

// clang-format off
EZ_IMPLEMENT_WORLD_MODULE(ezNavigationWorldModule);
EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezNavigationWorldModule, 1, ezRTTINoAllocator)
EZ_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

ezNavigationWorldModule::ezNavigationWorldModule(ezWorld* pWorld)
  : ezWorldModule(pWorld)
{
}

ezNavigationWorldModule::~ezNavigationWorldModule() = default;

void ezNavigationWorldModule::Initialize()
{
  SUPER::Initialize();

  {
  }
}

void ezNavigationWorldModule::Deinitialize()
{

  SUPER::Deinitialize();
}

