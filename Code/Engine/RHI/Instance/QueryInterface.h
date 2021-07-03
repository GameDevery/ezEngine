#pragma once
#include <RHI/RHIDLL.h>
#include <Foundation/Types/SharedPtr.h>

class EZ_RHI_DLL QueryInterface : public ezRefCounted
{
public:
    virtual ~QueryInterface() = default;

    template<typename T>
    T& As()
    {
        return static_cast<T&>(*this);
    }

    template<typename T>
    const T& As() const
    {
        return static_cast<T&>(*this);
    }
};
