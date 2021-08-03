#pragma once

#include <Foundation/Containers/Map.h>
#include <Foundation/Containers/Set.h>
#include <Foundation/Types/Uuid.h>
#include <NavigationPlugin/Navigation/Navigation.h>
#include <NavigationPlugin/Navigation/NavigationMeshLayer.h>
#include <NavigationPlugin/NavigationPluginDLL.h>
#include <Recast/DetourNavMesh.h>
#include <Recast/DetourNavMeshQuery.h>
#include <Recast/Recast.h>

struct ezNavigationMeshCache;


class EZ_NAVIGATIONPLUGIN_DLL ezNavigationMesh
{
public:
  ezNavigationMesh();
  ~ezNavigationMesh();
  bool Init(float cellTileSize);
  bool LoadTile(ezUInt8* navData, int navDataLength);
  bool RemoveTile(ezNavigation::Point tileCoordinate);
  void FindPath(const ezNavigation::PathFindQuery& query, ezNavigation::PathFindResult* result);
  void Raycast(const ezNavigation::RaycastQuery& query, ezNavigation::RaycastResult* result);

  EZ_ALWAYS_INLINE void SetCache(ezNavigationMeshCache* pCache) { m_pCache = pCache; }
  EZ_ALWAYS_INLINE ezNavigationMeshCache* GetCache() const { return m_pCache; }
  EZ_ALWAYS_INLINE const ezMap<ezUuid, ezNavigationMeshLayer*>& GetLayers() const { return m_Layers; }

  
  EZ_ALWAYS_INLINE void SetTileSize(float tileSize) { m_TileSize = tileSize; }
  EZ_ALWAYS_INLINE void SetCellSize(float cellSize) { m_CellSize = cellSize; }
  EZ_ALWAYS_INLINE float GetTileSize() const { return m_TileSize; }
  EZ_ALWAYS_INLINE float GetCellSize() const { return m_CellSize; }

  
  ezResult Serialize(ezStreamWriter& Writer) const;
  ezResult Deserialize(ezStreamReader& Reader);

private:
  dtNavMesh* m_navMesh = nullptr;
  dtNavMeshQuery* m_navQuery = nullptr;
  ezSet<dtTileRef> m_tileRefs;

  ezNavigationMeshCache* m_pCache;
  ezMap<ezUuid, ezNavigationMeshLayer*> m_Layers;

  float m_TileSize;
  float m_CellSize;
};
