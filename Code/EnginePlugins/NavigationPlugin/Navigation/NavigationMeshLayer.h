#pragma once

#include <Foundation/Containers/Map.h>
#include <NavigationPlugin/Navigation/Navigation.h>
#include <NavigationPlugin/Navigation/NavigationMeshTile.h>
#include <NavigationPlugin/NavigationPluginDLL.h>

//\brief Information about what tiles changes after building a navigation mesh
class EZ_NAVIGATIONPLUGIN_DLL ezNavigationMeshLayer
{
public:
  //\brief Contains all the built tiles mapped to their tile coordinates
  const ezMap<ezNavigation::Point, ezNavigationMeshTile*>& GetTiles() const;

  //\brief Tries to find a built tile inside this layer
  const ezNavigationMeshTile* FindTile(const ezNavigation::Point& tileCoordinate) const;

  ezResult Serialize(ezStreamWriter& Writer) const;
  ezResult Deserialize(ezStreamReader& Reader);

private:
  ezMap<ezNavigation::Point, ezNavigationMeshTile*> m_Tiles;
};
