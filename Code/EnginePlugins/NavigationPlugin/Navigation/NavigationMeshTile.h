#pragma once

#include <NavigationPlugin/NavigationPluginDLL.h>
#include <Foundation/IO/Stream.h>

class EZ_NAVIGATIONPLUGIN_DLL ezNavigationMeshTile
{
public:
  // \ brief Binary data of the built navigation mesh tile
  ezDynamicArray<ezUInt8> Data;

  // \ brief Extracts the navigation mesh geometry from the data for this tile
  bool GetTileVertices(ezDynamicArray<ezVec3>& vertices, ezDynamicArray<ezInt32>& indices);

  ezResult Serialize(ezStreamWriter& Writer) const;
  ezResult Deserialize(ezStreamReader& Reader);
};
