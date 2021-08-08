#include <NavigationPluginPCH.h>

#include <NavigationPlugin/Navigation/NavigationMeshBuilder.h>
#include <NavigationPlugin/Navigation/NavigationMeshCache.h>
#include <NavigationPlugin/Navigation/NavigationMeshBuildUtils.h>
#include <Foundation/Time/Timestamp.h>
#include <Foundation/Time/Clock.h>

ezNavigationMeshBuilder::ezNavigationMeshBuilder(ezNavigationMesh* pOldNavigationMesh)
  : m_pOldNavigationMesh{pOldNavigationMesh}
{
}

void ezNavigationMeshBuilder::Add(const ezStaticColliderData& colliderData)
{
  ezLock lock(m_CollidersMutex);
  if (m_RegisteredIds.Contains(colliderData.Component.GetId()))
    EZ_REPORT_FAILURE("Duplicate collider added");

  m_Colliders.PushBack(colliderData);

  m_RegisteredIds.Insert(colliderData.Component.GetId());
}

void ezNavigationMeshBuilder::Remove(const ezStaticColliderData& colliderData)
{
  ezLock lock(m_CollidersMutex);

  if (!m_RegisteredIds.Contains(colliderData.Component.GetId()))
    EZ_REPORT_FAILURE("Trying to remove unregistered collider");

  m_Colliders.RemoveAndCopy(colliderData);

  m_RegisteredIds.Remove(colliderData.Component.GetId());
}

ezNavigationMeshBuildResult ezNavigationMeshBuilder::Build(const ezNavigationMeshBuildSettings& buildSettings,
  const ezDynamicArray<ezNavigationMeshGroup>& groups,
  ezUInt32 includedCollisionGroups,
  const ezDynamicArray<ezBoundingBox>& boundingBoxes)
{
  const ezNavigationMeshCache* lastCache = nullptr; 
  if (m_pOldNavigationMesh != nullptr)
  {
   lastCache = m_pOldNavigationMesh->GetCache();
  }

  ezNavigationMeshBuildResult result;

  if (groups.GetCount() == 0)
  {
    ezLog::Warning("No group settings found");

    result.Success = true;
    result.pNavigationMesh = ezNavigationMesh();
    return result;
  }

  if (boundingBoxes.GetCount() == 0)
  {
    ezLog::Warning("No bounding boxes found");
  }

  ezHashHelper<ezUInt32> helper;

  ezUInt32 groupsHash = 0;
  for (auto& group : groups)
  {
    ezUInt32 groupHash = group.CalculateHash();
    groupsHash = ezHashingUtils::xxHash32(&groupHash, sizeof(ezUInt32), groupsHash);
  }

  ezUInt32 settingsHash = ezHashingUtils::xxHash32(&groupsHash, sizeof(ezUInt32), buildSettings.CalculateHash());

  if (lastCache != nullptr && lastCache->SettingsHash != settingsHash)
  {
    // Start from scratch if settings changed
    m_pOldNavigationMesh = nullptr;
    ezLog::Info("Build settings changed, doing a full rebuild");
  }

  // Copy colliders so the collection doesn't get modified
  ezDynamicArray<ezStaticColliderData> collidersLocal(m_Colliders);

  BuildInput(collidersLocal, includedCollisionGroups);

  // Check if cache was cleared while building the input
  if (m_pOldNavigationMesh != nullptr)
  {
    lastCache = m_pOldNavigationMesh->GetCache();
  }

  // The new navigation mesh that will be created
  result.pNavigationMesh = ezNavigationMesh();
  result.pNavigationMesh.SetCellSize(buildSettings.CellSize);
  result.pNavigationMesh.SetTileSize(buildSettings.TileSize);

  // Tile cache for this new navigation mesh
  result.pNavigationMesh.SetCache(EZ_DEFAULT_NEW(ezNavigationMeshCache));
  ezNavigationMeshCache* newCache = result.pNavigationMesh.GetCache();
  newCache->SettingsHash = settingsHash;

  // Generate global bounding box for planes
  ezBoundingBox globalBoundingBox;

  for (auto& boundingBox : boundingBoxes)
  {
    globalBoundingBox.ExpandToInclude(boundingBox);
  }

  // Combine input and collect tiles to build
  ezNavigationMeshInputBuilder sceneNavigationMeshInputBuilder;
  for (auto& colliderData : collidersLocal)
  {
    if (colliderData.InputBuilder == nullptr)
    {
      continue;
    }

    sceneNavigationMeshInputBuilder.AppendOther(*colliderData.InputBuilder);
    newCache->Add(colliderData.Component, *colliderData.InputBuilder, colliderData.Planes, colliderData.ParameterHash);

    // Generate geometry for planes
    for (auto& plane : colliderData.Planes)
    {
      sceneNavigationMeshInputBuilder.AppendOther(BuildPlaneGeometry(plane, globalBoundingBox));
    }
  }

  // TODO: Generate tile local mesh input data
  auto inputVertices = sceneNavigationMeshInputBuilder.GetPoints();
  auto inputIndices = sceneNavigationMeshInputBuilder.GetIndices();

  // Enumerate over every layer, and build tiles for each of those layers using the provided agent settings
  for (auto& group : groups)
  {
    for (ezUInt32 layerIndex = 0; layerIndex < groups.GetCount(); layerIndex++)
    {
      auto& currentAgentSettings = group.AgentSettings;

      if (result.pNavigationMesh.GetLayers().Contains(group.Id))
      {
        ezLog::Error("The same group can't be selected twice: {}", group.Name);

        return result;
      }

      ezHashSet<ezNavigation::Point> tilesToBuild;

      for (auto& colliderData : collidersLocal)
      {
        if (colliderData.InputBuilder == nullptr)
          continue;

        if (colliderData.Processed)
        {
          MarkTiles(*colliderData.InputBuilder, buildSettings, currentAgentSettings, tilesToBuild);
          if (colliderData.Previous != nullptr)
            MarkTiles(colliderData.Previous->InputBuilder, buildSettings, currentAgentSettings, tilesToBuild);
        }
      }

      // Check for removed colliders
      if (lastCache != nullptr)
      {
        for (auto& object : lastCache->Objects)
        {
          if (!newCache->Objects.Contains(object))
          {
            MarkTiles(object.InputBuilder, buildSettings, currentAgentSettings, tilesToBuild);
          }
        }
      }

      // Calculate updated/added bounding boxes
      for (auto& boundingBox : boundingBoxes)
      {
        if (lastCache != nullptr && lastCache->BoundingBoxes.Contains(boundingBox))
        {
          auto tiles = ezNavigationMeshBuildUtils::GetOverlappingTiles(buildSettings, boundingBox);
          for (auto& tile : tiles)
          {
            tilesToBuild.Insert(tile);
          }
        }
      }

      // Check for removed bounding boxes
      if (lastCache != nullptr)
      {
        for (auto& boundingBox : lastCache->BoundingBoxes)
        {
          if (!boundingBoxes.Contains(boundingBox))
          {
            auto tiles = ezNavigationMeshBuildUtils::GetOverlappingTiles(buildSettings, boundingBox);
            for (auto& tile : tiles)
            {
              tilesToBuild.Insert(tile);
            }
          }
        }
      }

      ezTime buildTimeStamp = ezTime::Now();

      // use task system here to dispatch tile building

    }

  }


  return ezNavigationMeshBuildResult();
}

ezNavigationMeshTile* ezNavigationMeshBuilder::BuildTile(const ezNavigation::Point& tileCoordinate, const ezNavigationMeshBuildSettings& buildSettings, const ezNavigationAgentSettings& agentSettings, const ezDynamicArray<ezBoundingBox>& boundingBoxes, const ezDynamicArray<ezVec3>& inputVertices, const ezDynamicArray<ezInt32>& inputIndices, long buildTimeStamp)
{
  ezNavigationMeshTile* meshTile = nullptr;

  // Include bounding boxes in tile height range
  ezBoundingBox tileBoundingBox = ezNavigationMeshBuildUtils::CalculateTileBoundingBox(buildSettings, tileCoordinate);

  float minimumHeight = ezMath::MaxValue<float>();
  float maximumHeight = ezMath::MinValue<float>();
  bool shouldBuildTile = false;
  for (const auto& boundingBox : boundingBoxes)
  {
    if (boundingBox.Overlaps(tileBoundingBox))
    {
      maximumHeight = ezMath::Max(maximumHeight, boundingBox.m_vMax.y);
      minimumHeight = ezMath::Min(minimumHeight, boundingBox.m_vMin.y);
      shouldBuildTile = true;
    }
  }

  ezNavigationMeshBuildUtils::SnapBoundingBoxToCellHeight(buildSettings, tileBoundingBox);

  // Skip tiles that do not overlap with any bounding box
  if (shouldBuildTile)
  {
    // Set tile's minimum and maximum height
    tileBoundingBox.m_vMin.y = minimumHeight;
    tileBoundingBox.m_vMax.y = maximumHeight;

    ezNavigationBuilder* builder = ezNavigation::CreateBuilder();

    // Turn build settings into native structure format
    ezNavigation::BuildSettings internalBuildSettings;

    // Tile settings
    internalBuildSettings.m_BoundingBox = tileBoundingBox;
    internalBuildSettings.m_TilePosition = tileCoordinate;
    internalBuildSettings.m_TileSize = buildSettings.TileSize;

    // General build settings
    internalBuildSettings.m_CellHeight = buildSettings.CellHeight;
    internalBuildSettings.m_CellSize = buildSettings.CellSize;
    internalBuildSettings.m_RegionMinArea = buildSettings.MinRegionArea;
    internalBuildSettings.m_RegionMergeArea = buildSettings.RegionMergeArea;
    internalBuildSettings.m_EdgeMaxLen = buildSettings.MaxEdgeLen;
    internalBuildSettings.m_EdgeMaxError = buildSettings.MaxEdgeError;
    internalBuildSettings.m_DetailSampleDistInput = buildSettings.DetailSamplingDistance;
    internalBuildSettings.m_DetailSampleMaxErrorInput = buildSettings.MaxDetailSamplingError;

    // Agent settings
    internalBuildSettings.m_AgentHeight = agentSettings.Height;
    internalBuildSettings.m_AgentRadius = agentSettings.Radius;
    internalBuildSettings.m_AgentMaxClimb = agentSettings.MaxClimb;
    internalBuildSettings.m_AgentMaxSlope = agentSettings.MaxSlope.GetDegree();

    ezNavigation::SetSettings(builder, &internalBuildSettings);
    ezNavigation::GeneratedData* generatedData = ezNavigation::BuildNavmesh(builder, inputVertices.GetData(), inputVertices.GetCount(), inputIndices.GetData(), inputIndices.GetCount());
    if (generatedData->m_Success && generatedData->m_NavmeshDataLength > 0)
    {
      meshTile = EZ_DEFAULT_NEW(ezNavigationMeshTile);

      // Copy the generated navigationMesh data
      meshTile->Data.SetCountUninitialized(generatedData->m_NavmeshDataLength);
      ezMemoryUtils::Copy(meshTile->Data.GetData(), generatedData->m_pNavmeshData, generatedData->m_NavmeshDataLength);

      // Append time stamp
      ezUInt8* timeStamp = reinterpret_cast<ezUInt8*>(&buildTimeStamp);
    }
  }

  return nullptr;
}
