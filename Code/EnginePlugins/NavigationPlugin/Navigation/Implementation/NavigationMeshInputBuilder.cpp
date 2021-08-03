#include <NavigationPluginPCH.h>

#include <NavigationPlugin/Navigation/NavigationMeshInputBuilder.h>

void ezNavigationMeshInputBuilder::AppendOther(const ezNavigationMeshInputBuilder& other)
{
  ezUInt32 vBase = m_Points.GetCount();

  // Copy vertices
  for (ezUInt32 i = 0; i < other.GetPoints().GetCount(); i++)
  {
    ezVec3 point = other.GetPoints()[i];
    m_Points.PushBack(point);
    m_BoundingBox.ExpandToInclude(point);
  }

  // Copy indices with offset applied
  for (ezInt32 index : other.GetIndices())
  {
    m_Indices.PushBack(index + vBase);
  }
}

void ezNavigationMeshInputBuilder::AppendArrays(const ezDynamicArray<ezVec3>& vertices, const ezDynamicArray<ezInt32>& indices, ezMat4 transform)
{
  ezUInt32 vBase = m_Points.GetCount();

  for (ezUInt32 i = 0; i < vertices.GetCount(); i++)
  {
    ezVec3 vertex = transform.TransformPosition(vertices[i]); // todo: verify behaviour
    m_Points.PushBack(vertex);
    m_BoundingBox.ExpandToInclude(vertex);
  }

  // Copy indices with offset applied
  for (ezInt32 index : indices)
  {
    m_Indices.PushBack(index + vBase);
  }
}

void ezNavigationMeshInputBuilder::AppendArrays(const ezDynamicArray<ezVec3>& vertices, const ezDynamicArray<ezInt32>& indices)
{
  ezUInt32 vBase = m_Points.GetCount();

  for (ezUInt32 i = 0; i < vertices.GetCount(); i++)
  {
    m_Points.PushBack(vertices[i]);
    m_BoundingBox.ExpandToInclude(vertices[i]);
  }

  // Copy indices with offset applied
  for (ezInt32 index : indices)
  {
    m_Indices.PushBack(index + vBase);
  }
}

void ezNavigationMeshInputBuilder::AppendMeshData(const ezGeometricMeshData<ezVertexPositionNormalTexture>& meshData, ezMat4 transform)
{
  ezUInt32 vBase = m_Points.GetCount();

  // Transform box points
  for (ezUInt32 i = 0; i < meshData.Vertices.GetCount(); i++)
  {
    ezVertexPositionNormalTexture point = meshData.Vertices[i];
    point.Position = transform.TransformPosition(point.Position);
    m_Points.PushBack(point.Position);
    m_BoundingBox.ExpandToInclude(point.Position);

  }

  if (meshData.IsLeftHanded)
  {
    // Copy indices with offset applied
    for (ezUInt32 i = 0; i < meshData.Indices.GetCount(); i += 3)
    {
      m_Indices.PushBack(meshData.Indices[i] + vBase);
      m_Indices.PushBack(meshData.Indices[i + 2] + vBase);
      m_Indices.PushBack(meshData.Indices[i + 1] + vBase);
    }
  }
  else
  {
    // Copy indices with offset applied
    for (ezUInt32 i = 0; i < meshData.Indices.GetCount(); i++)
    {
      m_Indices.PushBack(meshData.Indices[i] + vBase);
    }
  }
}
