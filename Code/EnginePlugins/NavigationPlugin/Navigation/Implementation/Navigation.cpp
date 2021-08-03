#include <NavigationPluginPCH.h>

#include <NavigationPlugin/Navigation/Navigation.h>
#include <NavigationPlugin/Navigation/NavigationMesh.h>
#include <NavigationPlugin/Navigation/NavigationBuilder.h>

ezNavigationBuilder* ezNavigation::CreateBuilder()
{
  return EZ_DEFAULT_NEW(ezNavigationBuilder);
}

void ezNavigation::DestroyBuilder(ezNavigationBuilder* nav)
{
  EZ_DEFAULT_DELETE(nav);
}

void ezNavigation::SetSettings(ezNavigationBuilder* nav, BuildSettings* buildSettings)
{
  nav->SetSettings(*buildSettings);
}

ezNavigation::GeneratedData* ezNavigation::BuildNavmesh(ezNavigationBuilder* nav, ezVec3* vertices, int numVertices, int* indices, int numIndices)
{
  return nav->BuildNavmesh(vertices, numVertices, indices, numIndices);
}

// Navmesh Query

void* ezNavigation::CreateNavmesh(float cellTileSize)
{
  ezNavigationMesh* navmesh = EZ_DEFAULT_NEW(ezNavigationMesh);
  if (!navmesh->Init(cellTileSize))
  {
    EZ_DEFAULT_DELETE(navmesh);
    navmesh = nullptr;
  }
  return navmesh;
}

void ezNavigation::DestroyNavmesh(ezNavigationMesh* navmesh)
{
  EZ_DEFAULT_DELETE(navmesh);
}

bool ezNavigation::AddTile(ezNavigationMesh* navmesh, uint8_t* data, int dataLength)
{
  return navmesh->LoadTile(data, dataLength);
}

bool ezNavigation::RemoveTile(ezNavigationMesh* navmesh, Point tileCoordinate)
{
  return navmesh->RemoveTile(tileCoordinate);
}

void ezNavigation::DoPathFindQuery(ezNavigationMesh* navmesh, const ezNavigation::PathFindQuery& query, ezNavigation::PathFindResult* result)
{
  navmesh->FindPath(query, result);
}

void ezNavigation::DoRaycastQuery(ezNavigationMesh* navmesh, const ezNavigation::RaycastQuery& query, ezNavigation::RaycastResult* result)
{
  navmesh->Raycast(query, result);
}

int ezNavigation::DtAlign4(int size)
{
  return (size + 3) & ~3;
}
