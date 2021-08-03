#pragma once

#include <NavigationPlugin/NavigationPluginDLL.h>
#include <NavigationPlugin/Navigation/NavigationAgentSettings.h>
#include <Foundation/Types/Uuid.h>
#include <Foundation/Strings/String.h>

struct EZ_NAVIGATIONPLUGIN_DLL ezNavigationMeshGroup
{
  ezUuid Id;
  ezString Name;
  ezNavigationAgentSettings AgentSettings;
};
