#include <NavigationPluginPCH.h>

#include <NavigationPlugin/Navigation/NavigationBuilder.h>

#include <Recast/DetourNavMeshBuilder.h>
#include <Foundation/Time/Stopwatch.h>

ezNavigationBuilder::ezNavigationBuilder()
{
  m_context = new rcContext(false);
}

ezNavigationBuilder::~ezNavigationBuilder()
{
  delete m_context;
  Cleanup();
}

void ezNavigationBuilder::Cleanup()
{
  if (m_navmeshData)
  {
    dtFree(m_navmeshData);
    m_navmeshData = nullptr;
    m_navmeshDataLength = 0;
  }
  if (m_solid)
  {
    rcFreeHeightField(m_solid);
    m_solid = nullptr;
  }
  if (m_triareas)
  {
    delete[] m_triareas;
    m_triareas = nullptr;
  }
  if (m_chf)
  {
    rcFreeCompactHeightfield(m_chf);
    m_chf = nullptr;
  }
  if (m_pmesh)
  {
    rcFreePolyMesh(m_pmesh);
    m_pmesh = nullptr;
  }
  if (m_dmesh)
  {
    rcFreePolyMeshDetail(m_dmesh);
    m_dmesh = nullptr;
  }
}

ezNavigation::GeneratedData* ezNavigationBuilder::BuildNavmesh(ezVec3* vertices, int numVertices, int* indices, int numIndices)
{
  ezNavigation::GeneratedData* ret = &m_result;
  ret->m_Success = false;

  float bmin[3];
  memcpy(bmin, &m_buildSettings.m_BoundingBox.m_vMin.x, sizeof(float) * 3);
  float bmax[3];
  memcpy(bmax, &m_buildSettings.m_BoundingBox.m_vMax.x, sizeof(float) * 3);

  float bbSize[3];
  rcVsub(bbSize, bmax, bmin);
  if (bbSize[0] <= 0.0f || bbSize[1] <= 0.0f || bbSize[2] <= 0.0f)
    return ret; // Negative or empty bounding box

  // Check input parameters
  if (m_buildSettings.m_DetailSampleDistInput < 1.0f)
    return ret;
  if (m_buildSettings.m_DetailSampleMaxErrorInput <= 0.0f)
    return ret;
  if (m_buildSettings.m_EdgeMaxError < 0.1f)
    return ret;
  if (m_buildSettings.m_EdgeMaxLen < 0.0f)
    return ret;
  if (m_buildSettings.m_RegionMinArea < 0.0f)
    return ret;
  if (m_buildSettings.m_RegionMergeArea < 0.0f)
    return ret;
  if (m_buildSettings.m_TileSize <= 0)
    return ret;

  // Limit cell size to not freeze the process with calculating a huge amount of cells
  if (m_buildSettings.m_CellSize < 0.01f)
    m_buildSettings.m_CellSize = 0.01f;
  if (m_buildSettings.m_CellHeight < 0.01f)
    m_buildSettings.m_CellHeight = 0.01f;

  int maxEdgeLen = (int)(m_buildSettings.m_EdgeMaxLen / m_buildSettings.m_CellSize);
  float maxSimplificationError = m_buildSettings.m_EdgeMaxError;
  int maxVertsPerPoly = 6;
  float detailSampleDist = m_buildSettings.m_CellSize * m_buildSettings.m_DetailSampleDistInput;
  float detailSampleMaxError = m_buildSettings.m_CellHeight * m_buildSettings.m_DetailSampleMaxErrorInput;

  int walkableHeight = (int)ceilf(m_buildSettings.m_AgentHeight / m_buildSettings.m_CellHeight);
  int walkableClimb = (int)floorf(m_buildSettings.m_AgentMaxClimb / m_buildSettings.m_CellHeight);
  int walkableRadius = (int)ceilf(m_buildSettings.m_AgentRadius / m_buildSettings.m_CellSize);

  // Size of the tile border
  int borderSize = walkableRadius + 3;
  int tileSize = m_buildSettings.m_TileSize;

  // Expand bounding box by border size so that all required geometry is included
  bmin[0] -= borderSize * m_buildSettings.m_CellSize;
  bmin[2] -= borderSize * m_buildSettings.m_CellSize;
  bmax[0] += borderSize * m_buildSettings.m_CellSize;
  bmax[2] += borderSize * m_buildSettings.m_CellSize;

  int width = tileSize + borderSize * 2;
  int height = tileSize + borderSize * 2;

  ezStopwatch stopwatch;
  //double totalTime = npSeconds();

  // Make sure state is clean
  Cleanup();

  if (numIndices == 0 || numVertices == 0)
    return ret;

  if (walkableClimb < 0)
    return ret;

  m_solid = rcAllocHeightfield();
  if (!rcCreateHeightfield(m_context, *m_solid, width, height, bmin, bmax, m_buildSettings.m_CellSize, m_buildSettings.m_CellHeight))
  {
    return ret;
  }

  int numTriangles = numIndices / 3;
  m_triareas = new uint8_t[numTriangles];
  if (!m_triareas)
  {
    return ret;
  }

  // Find walkable triangles and rasterize into heightfield
  memset(m_triareas, 0, numTriangles * sizeof(unsigned char));
  rcMarkWalkableTriangles(m_context, m_buildSettings.m_AgentMaxSlope, (float*)vertices, numVertices, indices, numTriangles, m_triareas);
  if (!rcRasterizeTriangles(m_context, (float*)vertices, numVertices, indices, m_triareas, numTriangles, *m_solid, walkableClimb))
  {
    return ret;
  }

  // Filter walkables surfaces.
  rcFilterLowHangingWalkableObstacles(m_context, walkableClimb, *m_solid);
  rcFilterLedgeSpans(m_context, walkableHeight, walkableClimb, *m_solid);
  rcFilterWalkableLowHeightSpans(m_context, walkableHeight, *m_solid);

  // Compact the heightfield so that it is faster to handle from now on.
  // This will result more cache coherent data as well as the neighbours
  // between walkable cells will be calculated.
  m_chf = rcAllocCompactHeightfield();
  if (!m_chf)
  {
    return ret;
  }
  if (!rcBuildCompactHeightfield(m_context, walkableHeight, walkableClimb, *m_solid, *m_chf))
  {
    return ret;
  }

  // No longer need solid heightfield after compacting it
  rcFreeHeightField(m_solid);
  m_solid = 0;

  // Erode the walkable area by agent radius.
  if (!rcErodeWalkableArea(m_context, walkableRadius, *m_chf))
  {
    return ret;
  }

  // Prepare for region partitioning, by calculating distance field along the walkable surface.
  if (!rcBuildDistanceField(m_context, *m_chf))
  {
    return ret;
  }
  // Partition the walkable surface into simple regions without holes.
  if (!rcBuildRegions(m_context, *m_chf, borderSize, m_buildSettings.m_RegionMinArea, m_buildSettings.m_RegionMergeArea))
  {
    return ret;
  }

  // Create contours.
  m_cset = rcAllocContourSet();
  if (!m_cset)
  {
    return ret;
  }
  if (!rcBuildContours(m_context, *m_chf, maxSimplificationError, maxEdgeLen, *m_cset))
  {
    return ret;
  }

  // Build polygon navmesh from the contours.
  m_pmesh = rcAllocPolyMesh();
  if (!m_pmesh)
  {
    return ret;
  }
  if (!rcBuildPolyMesh(m_context, *m_cset, maxVertsPerPoly, *m_pmesh))
  {
    return ret;
  }

  // Free intermediate results
  rcFreeContourSet(m_cset);
  m_cset = nullptr;

  m_dmesh = rcAllocPolyMeshDetail();
  if (!m_dmesh)
  {
    return ret;
  }

  if (!rcBuildPolyMeshDetail(m_context, *m_pmesh, *m_chf, detailSampleDist, detailSampleMaxError, *m_dmesh))
  {
    return ret;
  }

  // Free intermediate results
  rcFreeCompactHeightfield(m_chf);
  m_chf = nullptr;

  // Update poly flags from areas.
  for (int i = 0; i < m_pmesh->npolys; ++i)
  {
    if (m_pmesh->areas[i] == RC_WALKABLE_AREA)
      m_pmesh->areas[i] = 0;

    if (m_pmesh->areas[i] == 0)
    {
      m_pmesh->flags[i] = 1;
    }
  }

  // Generate native navmesh format and store the data pointers in the return structure
  if (!CreateDetourMesh())
    return ret;
  ret->m_pNavmeshData = m_navmeshData;
  ret->m_NavmeshDataLength = m_navmeshDataLength;
  ret->m_Success = true;

  stopwatch.Pause();
  //totalTime = npSeconds() - totalTime;
  return ret;
}

void ezNavigationBuilder::SetSettings(const ezNavigation::BuildSettings& buildSettings)
{
  m_buildSettings = buildSettings;
}

bool ezNavigationBuilder::CreateDetourMesh()
{
  dtNavMeshCreateParams params = {0};
  params.verts = m_pmesh->verts;
  params.vertCount = m_pmesh->nverts;
  params.polys = m_pmesh->polys;
  params.polyAreas = m_pmesh->areas;
  params.polyFlags = m_pmesh->flags;
  params.polyCount = m_pmesh->npolys;
  params.nvp = m_pmesh->nvp;
  params.detailMeshes = m_dmesh->meshes;
  params.detailVerts = m_dmesh->verts;
  params.detailVertsCount = m_dmesh->nverts;
  params.detailTris = m_dmesh->tris;
  params.detailTriCount = m_dmesh->ntris;
  // TODO: Support off-mesh connections
  params.offMeshConVerts = nullptr;
  params.offMeshConRad = nullptr;
  params.offMeshConDir = nullptr;
  params.offMeshConAreas = nullptr;
  params.offMeshConFlags = nullptr;
  params.offMeshConUserID = nullptr;
  params.offMeshConCount = 0;
  params.walkableHeight = m_buildSettings.m_AgentHeight;
  params.walkableClimb = m_buildSettings.m_AgentMaxClimb;
  params.walkableRadius = m_buildSettings.m_AgentRadius;
  rcVcopy(params.bmin, m_pmesh->bmin);
  rcVcopy(params.bmax, m_pmesh->bmax);
  params.cs = m_buildSettings.m_CellSize;
  params.ch = m_buildSettings.m_CellHeight;
  params.buildBvTree = true;
  params.tileX = m_buildSettings.m_TilePosition.x;
  params.tileY = m_buildSettings.m_TilePosition.y;

  if (!dtCreateNavMeshData(&params, &m_navmeshData, &m_navmeshDataLength))
  {
    dtFree(m_navmeshData);
    return false;
  }
  if (m_navmeshDataLength == 0 || !m_navmeshData)
    return false;
  return true;
}
