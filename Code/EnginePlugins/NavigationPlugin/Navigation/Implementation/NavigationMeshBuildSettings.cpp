#include <NavigationPluginPCH.h>

#include <NavigationPlugin/Navigation/NavigationMeshBuildSettings.h>

ezNavigationMeshBuildSettings ezNavigationMeshBuildSettings::Default()
{
  ezNavigationMeshBuildSettings settings;

  settings.CellHeight = 0.2f;
  settings.CellSize = 0.3f;
  settings.TileSize = 32;
  settings.MinRegionArea = 2;
  settings.RegionMergeArea = 20;
  settings.MaxEdgeLen = 12.0f;
  settings.MaxEdgeError = 1.3f;
  settings.DetailSamplingDistance = 6.0f;
  settings.MaxDetailSamplingError = 1.0f;

  return settings;
}
