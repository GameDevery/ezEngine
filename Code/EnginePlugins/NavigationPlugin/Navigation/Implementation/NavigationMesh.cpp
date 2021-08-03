#include <NavigationPluginPCH.h>

#include <NavigationPlugin/Navigation/NavigationMesh.h>

#include <Recast/DetourCommon.h>

ezNavigationMesh::ezNavigationMesh()
{
}

ezNavigationMesh::~ezNavigationMesh()
{
  // Cleanup allocated tiles
  for (auto tile : m_tileRefs)
  {
    ezUInt8* deletedData;
    int deletedDataLength = 0;
    dtStatus status = m_navMesh->removeTile(tile, &deletedData, &deletedDataLength);
    if (dtStatusSucceed(status))
    {
      if (deletedData)
        delete[] deletedData;
    }
  }

  if (m_navQuery)
    dtFreeNavMeshQuery(m_navQuery);
  if (m_navMesh)
    dtFreeNavMesh(m_navMesh);
}

bool ezNavigationMesh::Init(float cellTileSize)
{ // Allocate objects
  m_navMesh = dtAllocNavMesh();
  m_navQuery = dtAllocNavMeshQuery();

  if (!m_navMesh || !m_navQuery)
    return false;

  dtNavMeshParams params = {0};
  params.orig[0] = 0.0f;
  params.orig[1] = 0.0f;
  params.orig[2] = 0.0f;
  params.tileWidth = cellTileSize;
  params.tileHeight = cellTileSize;

  // TODO: Link these parameters to the builder
  int tileBits = 14;
  if (tileBits > 14)
    tileBits = 14;
  int polyBits = 22 - tileBits;
  params.maxTiles = 1 << tileBits;
  params.maxPolys = 1 << polyBits;

  dtStatus status = m_navMesh->init(&params);
  if (dtStatusFailed(status))
    return false;

  // Initialize the query object
  status = m_navQuery->init(m_navMesh, 2048);
  if (dtStatusFailed(status))
    return false;
  return true;
}

bool ezNavigationMesh::LoadTile(ezUInt8* navData, int navDataLength)
{
  if (!m_navMesh || !m_navQuery)
    return false;
  if (!navData)
    return false;

  // Copy data
  uint8_t* dataCopy = new uint8_t[navDataLength];
  memcpy(dataCopy, navData, navDataLength);

  dtTileRef tileRef = 0;
  if (dtStatusSucceed(m_navMesh->addTile(dataCopy, navDataLength, 0, 0, &tileRef)))
  {
    m_tileRefs.Insert(tileRef);
    return true;
  }

  delete[] dataCopy;
  return false;
}

bool ezNavigationMesh::RemoveTile(ezNavigation::Point tileCoordinate)
{
  dtTileRef tileRef = m_navMesh->getTileRefAt(tileCoordinate.x, tileCoordinate.y, 0);

  uint8_t* deletedData;
  int deletedDataLength = 0;
  dtStatus status = m_navMesh->removeTile(tileRef, &deletedData, &deletedDataLength);
  if (dtStatusSucceed(status))
  {
    if (deletedData)
      delete[] deletedData;
    m_tileRefs.Remove(tileRef);
    return true;
  }

  return false;
}

void ezNavigationMesh::FindPath(const ezNavigation::PathFindQuery& query, ezNavigation::PathFindResult* result)
{ // Reset result
  result->PathFound = false;
  dtPolyRef startPoly, endPoly;
  ezVec3 startPoint, endPoint;

  // Find the starting polygons and point on it to start from
  dtQueryFilter filter;
  dtStatus status;
  status = m_navQuery->findNearestPoly(&query.Source.x, &query.FindNearestPolyExtent.x, &filter, &startPoly, &startPoint.x);
  if (dtStatusFailed(status))
    return;
  status = m_navQuery->findNearestPoly(&query.Target.x, &query.FindNearestPolyExtent.x, &filter, &endPoly, &endPoint.x);
  if (dtStatusFailed(status))
    return;

  ezDynamicArray<dtPolyRef> polys;
  polys.SetCountUninitialized(query.MaxPathPoints);
  int pathPointCount = 0;
  status = m_navQuery->findPath(startPoly, endPoly, &startPoint.x, &endPoint.x,
    &filter, polys.GetData(), &pathPointCount, polys.GetCount());
  if (dtStatusFailed(status) || (status & DT_PARTIAL_RESULT) != 0)
    return;

  ezDynamicArray<ezVec3> straightPath;
  ezDynamicArray<ezUInt8> straightPathFlags;
  ezDynamicArray<dtPolyRef> straightpathPolys;
  straightPath.SetCountUninitialized(query.MaxPathPoints);
  straightPathFlags.SetCountUninitialized(query.MaxPathPoints);
  straightpathPolys.SetCountUninitialized(query.MaxPathPoints);
  status = m_navQuery->findStraightPath(&startPoint.x, &endPoint.x,
    polys.GetData(), pathPointCount,
    (float*)result->PathPoints, straightPathFlags.GetData(), straightpathPolys.GetData(),
    &result->NumPathPoints, query.MaxPathPoints);
  if (dtStatusFailed(status))
    return;
  result->PathFound = true;
}

void ezNavigationMesh::Raycast(const ezNavigation::RaycastQuery& query, ezNavigation::RaycastResult* result)
{
  // Reset result
  result->Hit = false;
  dtQueryFilter filter;

  dtPolyRef startPoly;
  dtStatus status = m_navQuery->findNearestPoly(&query.Source.x, &query.FindNearestPolyExtent.x, &filter, &startPoly, 0);
  if (dtStatusFailed(status))
    return;

  float t;
  ezDynamicArray<dtPolyRef> polys;
  polys.SetCountUninitialized(query.MaxPathPoints);
  int raycastPolyCount = 0;
  status = m_navQuery->raycast(startPoly, &query.Source.x, &query.Target.x, &filter, &t, &result->Normal.x, polys.GetData(), &raycastPolyCount, polys.GetCount());
  if (dtStatusFailed(status))
    return;

  result->Hit = true;
  dtVlerp(&result->Position.x, &query.Source.x, &query.Target.x, t);
}

ezResult ezNavigationMesh::Serialize(ezStreamWriter& Writer) const
{
  return EZ_SUCCESS;
}

ezResult ezNavigationMesh::Deserialize(ezStreamReader& Reader)
{
  return EZ_SUCCESS;
}
