#pragma once

#include <Foundation/Types/Uuid.h>
#include <NavigationPlugin/Navigation/NavigationMeshCachedObject.h>
#include <NavigationPlugin/Navigation/NavigationMeshInputBuilder.h>
#include <NavigationPlugin/Navigation/StaticColliderData.h>
#include <NavigationPlugin/NavigationPluginDLL.h>

//\brief Utility function for navigation mesh building
class EZ_NAVIGATIONPLUGIN_DLL ezNavigationMeshBuildUtils
{
public:

  // Check which tiles overlap a given bounding box
  static ezDynamicArray<ezNavigation::Point> GetOverlappingTiles(const ezNavigationMeshBuildSettings& settings, const ezBoundingBox& boundingBox);

  static void SnapBoundingBoxToCellHeight(const ezNavigationMeshBuildSettings& settings, const ezBoundingBox& boundingBox);

  static ezBoundingBox CalculateTileBoundingBox(const ezNavigationMeshBuildSettings& settings, const ezNavigation::Point& tileCoord);

  static void GenerateTangentBinormal(ezVec3 normal, ezVec3& out_tangent, ezVec3& out_binormal);

  static void BuildPlanePoints(const ezPlane& plane, float size, ezDynamicArray<ezVec3>& points, ezDynamicArray<ezInt32>& inds);
};
