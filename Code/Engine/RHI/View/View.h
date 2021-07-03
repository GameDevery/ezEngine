#pragma once
#include <RHI/Instance/QueryInterface.h>
#include <RHI/Instance/BaseTypes.h>
#include <memory>

class Resource;

class View : public QueryInterface
{
public:
    virtual ~View() = default;
    virtual ezSharedPtr<Resource> GetResource() = 0;
    virtual uint32_t GetDescriptorId() const = 0;
    virtual uint32_t GetBaseMipLevel() const = 0;
    virtual uint32_t GetLevelCount() const = 0;
    virtual uint32_t GetBaseArrayLayer() const = 0;
    virtual uint32_t GetLayerCount() const = 0;
};
