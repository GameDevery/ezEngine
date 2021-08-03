#pragma once

#include <NavigationPlugin/Navigation/Navigation.h>
#include <NavigationPlugin/NavigationPluginDLL.h>
#include <Recast/Recast.h>

class EZ_NAVIGATIONPLUGIN_DLL ezNavigationBuilder
{
public:
  ezNavigationBuilder();
  ~ezNavigationBuilder();
  void Cleanup();
  ezNavigation::GeneratedData* BuildNavmesh(ezVec3* vertices, int numVertices, int* indices, int numIndices);
  void SetSettings(const ezNavigation::BuildSettings& buildSettings);

private:
  bool CreateDetourMesh();

private:
  rcHeightfield* m_solid = nullptr;
  ezUInt8* m_triareas = nullptr;
  rcCompactHeightfield* m_chf = nullptr;
  rcContourSet* m_cset = nullptr;
  rcPolyMesh* m_pmesh = nullptr;
  rcPolyMeshDetail* m_dmesh = nullptr;
  ezNavigation::BuildSettings m_buildSettings;
  rcContext* m_context;

  // Detour returned navigation mesh data
  // free with dtFree()
  ezUInt8* m_navmeshData = nullptr;
  int m_navmeshDataLength = 0;

  ezNavigation::GeneratedData m_result;
};
