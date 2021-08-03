#include <NavigationPluginPCH.h>

#include <Foundation/Configuration/Startup.h>
#include <NavigationPlugin/NavigationInterface.h>
#include <NavigationPlugin/Resources/NavigationMeshResource.h>

// clang-format off
EZ_BEGIN_SUBSYSTEM_DECLARATION(Navigation, NavigationPlugin)

  BEGIN_SUBSYSTEM_DEPENDENCIES
    "Foundation",
    "Core"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORESYSTEMS_STARTUP
  {
  }

  ON_CORESYSTEMS_SHUTDOWN
  {
  }

  ON_HIGHLEVELSYSTEMS_STARTUP
  {
  }

  ON_HIGHLEVELSYSTEMS_SHUTDOWN
  {
  }

EZ_END_SUBSYSTEM_DECLARATION;
// clang-format on

//////////////////////////////////////////////////////////////////////////

ezPlugin g_Plugin(false);
