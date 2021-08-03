#pragma once

#include <Foundation/Algorithm/HashableStruct.h>
#include <Foundation/Strings/String.h>
#include <Foundation/Types/Uuid.h>
#include <NavigationPlugin/Navigation/NavigationMeshBuildSettings.h>
#include <NavigationPlugin/Navigation/NavigationMeshGroup.h>
#include <NavigationPlugin/NavigationPluginDLL.h>

struct EZ_NAVIGATIONPLUGIN_DLL ezVertexPositionNormalTexture
{
  ezVec3 Position;

  ezVec3 Normal;

  ezVec2 TextureCoordinate;
};

template <typename T>
struct ezGeometricMeshData
{
  ezDynamicArray<T> Vertices;

  ezDynamicArray<ezInt32> Indices;

  bool IsLeftHanded;

  ezGeometricMeshData(const ezDynamicArray<T>& vertices, const ezDynamicArray<ezInt32>& indices, bool isLeftHanded)
  {
    Vertices = vertices;
    Indices = indices;
    IsLeftHanded = isLeftHanded;
  }
};

struct EZ_NAVIGATIONPLUGIN_DLL ezNavigationMeshInputBuilder
{
public:
  /// Appends another vertex data builder
  void AppendOther(const ezNavigationMeshInputBuilder& other);

  void AppendArrays(const ezDynamicArray<ezVec3>& vertices, const ezDynamicArray<ezInt32>& indices, ezMat4 transform);

  void AppendArrays(const ezDynamicArray<ezVec3>& vertices, const ezDynamicArray<ezInt32>& indices);

  void AppendMeshData(const ezGeometricMeshData<ezVertexPositionNormalTexture>& meshData, ezMat4 transform);

  EZ_ALWAYS_INLINE const ezBoundingBox& GetBoundingBox() const { return m_BoundingBox; }
  EZ_ALWAYS_INLINE const ezDynamicArray<ezVec3> GetPoints() const { return m_Points; }
  EZ_ALWAYS_INLINE const ezDynamicArray<ezInt32> GetIndices() const { return m_Indices; }

private:
  ezBoundingBox m_BoundingBox;
  ezDynamicArray<ezVec3> m_Points;
  ezDynamicArray<ezInt32> m_Indices;
};
