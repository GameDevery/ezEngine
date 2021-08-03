#pragma once

#include <Foundation/Types/Uuid.h>
#include <NavigationPlugin/Navigation/NavigationMesh.h>
#include <NavigationPlugin/NavigationPluginDLL.h>
#include <Foundation/Algorithm/HashableStruct.h>

//\brief Provides settings for the navigation mesh builder to control granularity and error margins 
struct EZ_NAVIGATIONPLUGIN_DLL ezNavigationMeshBuildSettings : ezHashableStruct<ezNavigationMeshBuildSettings>
{
  /// The Height of a grid cell in the navigation mesh building steps using heightfields.
  /// A lower number means higher precision on the vertical axis but longer build times
  // 0.01, 4
  float CellHeight;

          /// The Width/Height of a grid cell in the navigation mesh building steps using heightfields.
  /// A lower number means higher precision on the horizontal axes but longer build times
  //[DataMemberRange(0.01, 4)]
  float CellSize;

  /// Tile size used for Navigation mesh tiles, the final size of a tile is CellSize*TileSize
  //[DataMemberRange(8, 4096, 1, 8, 0)]
  int TileSize;

  /// The minimum number of cells allowed to form isolated island areas
  //[DataMemberRange(0, 0)]
  int MinRegionArea;

  /// Any regions with a span count smaller than this value will, if possible,
  /// be merged with larger regions.
  //[DataMemberRange(0, 0)]
  int RegionMergeArea;

  /// The maximum allowed length for contour edges along the border of the mesh.
  //[DataMemberRange(0, 0)]
  float MaxEdgeLen;

  /// The maximum distance a simplfied contour's border edges should deviate from the original raw contour.
  //[DataMemberRange(0.1, 4)]
  float MaxEdgeError;

  /// Sets the sampling distance to use when generating the detail mesh. (For height detail only.)
  //[DataMemberRange(1.0, 3)]
  float DetailSamplingDistance;

  /// The maximum distance the detail mesh surface should deviate from heightfield data. (For height detail only.)
  //[DataMemberRange(0.0, 3)]
  float MaxDetailSamplingError;

  static ezNavigationMeshBuildSettings Default();
};

