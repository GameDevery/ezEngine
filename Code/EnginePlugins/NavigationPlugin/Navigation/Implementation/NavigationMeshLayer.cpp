#include <NavigationPluginPCH.h>

#include <NavigationPlugin/Navigation/NavigationMeshLayer.h>

const ezMap<ezNavigation::Point, ezNavigationMeshTile*>& ezNavigationMeshLayer::GetTiles() const
{
  return m_Tiles;
}

const ezNavigationMeshTile* ezNavigationMeshLayer::FindTile(const ezNavigation::Point& tileCoordinate) const
{
  ezNavigationMeshTile* pTile = nullptr;

  if (m_Tiles.TryGetValue(tileCoordinate, pTile))
  {
    return pTile;
  }
  return nullptr;
}

ezResult ezNavigationMeshLayer::Serialize(ezStreamWriter& Writer) const
{
  //return Writer.WriteMap(m_Tiles);

  return EZ_SUCCESS;
}

ezResult ezNavigationMeshLayer::Deserialize(ezStreamReader& Reader)
{
  m_Tiles.Clear();
  //return Reader.ReadMap(m_Tiles);

  return EZ_SUCCESS;
}
