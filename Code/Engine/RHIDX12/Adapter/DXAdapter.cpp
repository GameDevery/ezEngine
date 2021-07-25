#include <RHIDX12/Adapter/DXAdapter.h>
#include <RHIDX12/Device/DXDevice.h>
#include <RHIDX12/Utilities/DXUtility.h>
#include <dxgi1_6.h>
#include <directx/d3d12.h>

DXAdapter::DXAdapter(DXInstance& instance, const ComPtr<IDXGIAdapter1>& adapter)
    : m_Instance(instance)
    , m_Adapter(adapter)
{
    DXGI_ADAPTER_DESC desc = {};
    adapter->GetDesc(&desc);
    m_Name = ezStringUtf8(desc.Description);
}

const ezString& DXAdapter::GetName() const
{
    return m_Name;
}

ezSharedPtr<Device> DXAdapter::CreateDevice()
{
    return EZ_DEFAULT_NEW(DXDevice, *this);
}

DXInstance& DXAdapter::GetInstance()
{
    return m_Instance;
}

ComPtr<IDXGIAdapter1> DXAdapter::GetAdapter()
{
    return m_Adapter;
}
