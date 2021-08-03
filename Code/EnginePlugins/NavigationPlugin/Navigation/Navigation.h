#pragma once

#include <Foundation/Algorithm/HashableStruct.h>
#include <Foundation/Math/BoundingBox.h>
#include <NavigationPlugin/NavigationPluginDLL.h>

class ezNavigationBuilder;
class ezNavigationMesh;

class EZ_NAVIGATIONPLUGIN_DLL ezNavigation
{
public:
  struct Point : ezHashableStruct<Point>
  {
    ezInt32 x;
    ezInt32 y;

    EZ_FORCE_INLINE Point() = default;

    EZ_FORCE_INLINE Point(ezInt32 x, ezInt32 y)
      : x{x}
      , y{y}
    {
    }
  };

  struct BuildSettings
  {
    ezBoundingBox m_BoundingBox;
    float m_CellHeight;
    float m_CellSize;
    ezInt32 m_TileSize;
    struct Point m_TilePosition;
    ezInt32 m_RegionMinArea;
    ezInt32 m_RegionMergeArea;
    float m_EdgeMaxLen;
    float m_EdgeMaxError;
    float m_DetailSampleDistInput;
    float m_DetailSampleMaxErrorInput;
    float m_AgentHeight;
    float m_AgentRadius;
    float m_AgentMaxClimb;
    float m_AgentMaxSlope;
  };

  struct GeneratedData
  {
    bool m_Success;
    ezVec3* m_pNavmeshVertices = nullptr;
    ezInt32 m_NumNavmeshVertices = 0;
    ezUInt8* m_pNavmeshData = nullptr;
    ezInt32 m_NavmeshDataLength = 0;
  };

  struct TileHeader
  {
    int Magic;
    int Version;
    int X;
    int Y;
    int Layer;
    ezUInt32 UserId;
    int PolyCount;
    int VertCount;
    int MaxLinkCount;
    int DetailMeshCount;
    int DetailVertCount;
    int DetailTriCount;
    int BvNodeCount;
    int OffMeshConCount;
    int OffMeshBase;
    float WalkableHeight;
    float WalkableRadius;
    float WalkableClimb;
    float Bmin[3];
    float Bmax[3];
    float BvQuantFactor;
  };

  struct Poly
  {
    ezUInt8 FirstLink;
    unsigned short Vertices[6];
    unsigned short Neighbours[6];
    unsigned short Flags;
    ezUInt8 VertexCount;
    ezUInt8 AreaAndType;
  };

  struct PathFindQuery
  {
    ezVec3 Source;
    ezVec3 Target;
    ezVec3 FindNearestPolyExtent;
    ezInt32 MaxPathPoints;
  };

  struct PathFindResult
  {
    bool PathFound;
    ezVec3* PathPoints;
    ezInt32 NumPathPoints;
  };

  struct RaycastQuery
  {
    ezVec3 Source;
    ezVec3 Target;
    ezVec3 FindNearestPolyExtent;
    ezInt32 MaxPathPoints;
  };

  struct RaycastResult
  {
    bool Hit;
    ezVec3 Position;
    ezVec3 Normal;
  };

  //\brief Provides advanced settings to be passed to navigation mesh queries
  struct QuerySettings
  {
    /// Used as the extend for the find nearest poly bounding box used when scanning for a polygon corresponding to the given starting/ending position.
    /// Making this bigger will allow you to find paths that allow the entity to start further away or higher from the navigation mesh bounds for example
    ezVec3 FindNearestPolyExtent;

    /// The maximum number of path points used internally and also the maximum number of output points
    ezInt32 MaxPathPoints;

    /// The default settings that are used when querying navigation meshes
    EZ_FORCE_INLINE static QuerySettings Default()
    {
      return {ezVec3(2.0f, 4.0f, 2.0f), 1024};
    }
  };

public:
  static ezNavigationBuilder* CreateBuilder();

  static void DestroyBuilder(ezNavigationBuilder* nav);

  static void SetSettings(ezNavigationBuilder* nav, BuildSettings* buildSettings);

  static GeneratedData* BuildNavmesh(ezNavigationBuilder* nav, ezVec3* vertices, int numVertices, int* indices, int numIndices);

  // Navmesh Query
  static void* CreateNavmesh(float cellTileSize);

  static void DestroyNavmesh(ezNavigationMesh* navmesh);

  static bool AddTile(ezNavigationMesh* navmesh, uint8_t* data, int dataLength);

  static bool RemoveTile(ezNavigationMesh* navmesh, Point tileCoordinate);

  static void DoPathFindQuery(ezNavigationMesh* navmesh, const ezNavigation::PathFindQuery& query, PathFindResult* result);

  static void DoRaycastQuery(ezNavigationMesh* navmesh, const ezNavigation::RaycastQuery& query, RaycastResult* result);

  static int DtAlign4(int size);
};

template <>
struct ezCompareHelper<ezNavigation::Point>
{
  EZ_ALWAYS_INLINE bool Less(const ezNavigation::Point& a, const ezNavigation::Point& b) const { return a.CalculateHash() < b.CalculateHash(); }
  EZ_ALWAYS_INLINE bool Equal(const ezNavigation::Point& a, const ezNavigation::Point& b) const { return a.CalculateHash() == b.CalculateHash(); }
};
