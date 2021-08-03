#include <NavigationPluginPCH.h>

#include <NavigationPlugin/Navigation/NavigationMeshBuildUtils.h>

ezDynamicArray<ezNavigation::Point> ezNavigationMeshBuildUtils::GetOverlappingTiles(const ezNavigationMeshBuildSettings& settings, const ezBoundingBox& boundingBox)
{
  return ezDynamicArray<ezNavigation::Point>();
}

void ezNavigationMeshBuildUtils::SnapBoundingBoxToCellHeight(const ezNavigationMeshBuildSettings& settings, const ezBoundingBox& boundingBox)
{
}

ezBoundingBox ezNavigationMeshBuildUtils::CalculateTileBoundingBox(const ezNavigationMeshBuildSettings& settings, const ezNavigation::Point& tileCoord)
{
  return ezBoundingBox();
}

void ezNavigationMeshBuildUtils::GenerateTangentBinormal(ezVec3 normal, ezVec3& out_tangent, ezVec3& out_binormal)
{
}

void ezNavigationMeshBuildUtils::BuildPlanePoints(const ezPlane& plane, float size, ezDynamicArray<ezVec3>& points, ezDynamicArray<ezInt32>& inds)
{
}
