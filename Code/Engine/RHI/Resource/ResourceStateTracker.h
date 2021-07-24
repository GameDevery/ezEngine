#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Instance/BaseTypes.h>
#include <map>
#include <functional>

class EZ_RHI_DLL ResourceStateTracker
{
public:
    ResourceStateTracker(Resource& resource);
    bool HasResourceState() const;
    ResourceState GetResourceState() const;
    void SetResourceState(ResourceState state);
    ResourceState GetSubresourceState(ezUInt32 mip_level, ezUInt32 array_layer) const;
    void SetSubresourceState(ezUInt32 mip_level, ezUInt32 array_layer, ResourceState state);
    void Merge(const ResourceStateTracker& other);

private:
    Resource& m_resource;
    std::map<std::tuple<ezUInt32, ezUInt32>, ResourceState> m_subresource_states;
    std::map<ResourceState, size_t> m_subresource_state_groups;
    ResourceState m_resource_state = ResourceState::kUnknown;
};
