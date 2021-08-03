#include <NavigationPluginPCH.h>

#include <Core/Assets/AssetFileHeader.h>
#include <Foundation/IO/ChunkStream.h>
#include <Navigation/Navigation.h>
#include <NavigationPlugin/Resources/NavigationMeshResource.h>

// clang-format off
EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezNavigationMeshResource, 1, ezRTTIDefaultAllocator<ezNavigationMeshResource>)
EZ_END_DYNAMIC_REFLECTED_TYPE;

EZ_RESOURCE_IMPLEMENT_COMMON_CODE(ezNavigationMeshResource);
// clang-format on

//////////////////////////////////////////////////////////////////////////

ezNavigationMeshResourceDescriptor::ezNavigationMeshResourceDescriptor() = default;
ezNavigationMeshResourceDescriptor::ezNavigationMeshResourceDescriptor(ezNavigationMeshResourceDescriptor&& rhs)
{
  *this = std::move(rhs);
}

ezNavigationMeshResourceDescriptor::~ezNavigationMeshResourceDescriptor()
{
}

void ezNavigationMeshResourceDescriptor::operator=(ezNavigationMeshResourceDescriptor&& rhs)
{
}


//////////////////////////////////////////////////////////////////////////

ezResult ezNavigationMeshResourceDescriptor::Serialize(ezStreamWriter& stream) const
{
  stream.WriteVersion(1);
  return EZ_SUCCESS;
}

ezResult ezNavigationMeshResourceDescriptor::Deserialize(ezStreamReader& stream)
{
  return EZ_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////

ezNavigationMeshResource::ezNavigationMeshResource()
  : ezResource(DoUpdate::OnAnyThread, 1)
{
  ModifyMemoryUsage().m_uiMemoryCPU = sizeof(ezNavigationMeshResource);
}

ezNavigationMeshResource::~ezNavigationMeshResource()
{
}

ezResourceLoadDesc ezNavigationMeshResource::UnloadData(Unload WhatToUnload)
{
  ezResourceLoadDesc res;
  res.m_uiQualityLevelsDiscardable = 0;
  res.m_uiQualityLevelsLoadable = 0;
  res.m_State = ezResourceState::Unloaded;

  return res;
}

ezResourceLoadDesc ezNavigationMeshResource::UpdateContent(ezStreamReader* Stream)
{
  EZ_LOG_BLOCK("ezNavigationMeshResource::UpdateContent", GetResourceDescription().GetData());

  ezResourceLoadDesc res;
  res.m_uiQualityLevelsDiscardable = 0;
  res.m_uiQualityLevelsLoadable = 0;

  if (Stream == nullptr)
  {
    res.m_State = ezResourceState::LoadedResourceMissing;
    return res;
  }

  // skip the absolute file path data that the standard file reader writes into the stream
  {
    ezStringBuilder sAbsFilePath;
    (*Stream) >> sAbsFilePath;
  }

  ezAssetFileHeader AssetHash;
  AssetHash.Read(*Stream).IgnoreResult();

  ezNavigationMeshResourceDescriptor descriptor;
  descriptor.Deserialize(*Stream).IgnoreResult();

  return CreateResource(std::move(descriptor));
}

void ezNavigationMeshResource::UpdateMemoryUsage(MemoryUsage& out_NewMemoryUsage)
{
  out_NewMemoryUsage.m_uiMemoryCPU = sizeof(ezNavigationMeshResource);
  out_NewMemoryUsage.m_uiMemoryGPU = 0;
}

ezResourceLoadDesc ezNavigationMeshResource::CreateResource(ezNavigationMeshResourceDescriptor&& descriptor)
{
  ezResourceLoadDesc res;
  res.m_uiQualityLevelsDiscardable = 0;
  res.m_uiQualityLevelsLoadable = 0;
  res.m_State = ezResourceState::Loaded;

  return res;
}
