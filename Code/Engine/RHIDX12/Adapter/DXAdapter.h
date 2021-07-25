#pragma once
#include <RHI/Adapter/Adapter.h>
#include <dxgi.h>
#include <wrl.h>
using namespace Microsoft::WRL;

class DXInstance;

class DXAdapter : public Adapter
{
public:
    DXAdapter(DXInstance& instance, const ComPtr<IDXGIAdapter1>& adapter);
    const ezString& GetName() const override;
    ezSharedPtr<Device> CreateDevice() override;
    DXInstance& GetInstance();
    ComPtr<IDXGIAdapter1> GetAdapter();

private:
    DXInstance& m_Instance;
    ComPtr<IDXGIAdapter1> m_Adapter;
    ezString m_Name;
};
