#include <NavigationPluginPCH.h>

#include <NavigationPlugin/Navigation/NavigationMeshCache.h>

void ezNavigationMeshCache::Add(const ezStaticColliderComponent& collider, const ezNavigationMeshInputBuilder& data, const ezDynamicArray<ezPlane>& planes, ezInt32 entityColliderHash)
{
  Objects.PushBack(ezNavigationMeshCachedObject{0u, entityColliderHash, data, planes});
}
