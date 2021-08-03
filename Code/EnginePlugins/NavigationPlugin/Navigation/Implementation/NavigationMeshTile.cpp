#include <NavigationPluginPCH.h>

#include <NavigationPlugin/Navigation/Navigation.h>
#include <NavigationPlugin/Navigation/NavigationMeshTile.h>

bool ezNavigationMeshTile::GetTileVertices(ezDynamicArray<ezVec3>& vertices, ezDynamicArray<ezInt32>& indices)
{
  if (Data.GetCount() == 0)
    return false;

  ezUInt8* pData = Data.GetData();

  ezNavigation::TileHeader* header = (ezNavigation::TileHeader*)pData;
  if (header->VertCount == 0)
    return false;

  ezInt32 headerSize = ezNavigation::DtAlign4(sizeof(ezNavigation::TileHeader));
  ezInt32 vertsSize = ezNavigation::DtAlign4(sizeof(float) * 3 * header->VertCount);

  ezUInt8* ptr = pData;
  ptr += headerSize;

  ezVec3* vertexPtr = (ezVec3*)ptr;
  ptr += vertsSize;
  ezNavigation::Poly* polyPtr = (ezNavigation::Poly*)ptr;

  for (ezUInt32 i = 0; i < header->VertCount; i++)
  {
    vertices.PushBack(vertexPtr[i]);
  }

  for (ezUInt32 i = 0; i < header->PolyCount; i++)
  {
    ezNavigation::Poly poly = polyPtr[i];
    for (ezUInt32 j = 0; j < poly.VertexCount - 3; j++)
    {
      indices.PushBack(poly.Vertices[0]);
      indices.PushBack(poly.Vertices[j + 1]);
      indices.PushBack(poly.Vertices[j + 2]);
    }
  }

  return true;
}

ezResult ezNavigationMeshTile::Serialize(ezStreamWriter& Writer) const
{
  Writer << Data.GetCount();
  return Writer.WriteArray(Data);
}

ezResult ezNavigationMeshTile::Deserialize(ezStreamReader& Reader)
{
  ezUInt32 count = 0;

  Reader >> count;
  Data.Clear();
  Data.SetCountUninitialized(count);
  if (count > 0)
  {
    return Reader.ReadArray(Data);
  }

  return EZ_SUCCESS;
}
