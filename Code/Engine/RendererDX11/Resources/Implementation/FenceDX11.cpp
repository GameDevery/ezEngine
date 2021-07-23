#include <RendererDX11PCH.h>

#include <RendererDX11/Device/DeviceDX11.h>
#include <RendererDX11/RendererDX11DLL.h>
#include <RendererDX11/Resources/FenceDX11.h>

#include <d3d11_4.h>

ezGALFenceDX11::ezGALFenceDX11()
  : m_pDXFence(nullptr)
  , m_pDXFence2{nullptr}
  , m_pDXDeviceContext{nullptr}
{
}

ezGALFenceDX11::~ezGALFenceDX11() {}


ezResult ezGALFenceDX11::InitPlatform(ezGALDevice* pDevice, ezUInt64 initialValue)
{
  ezGALDeviceDX11* pDXDevice = static_cast<ezGALDeviceDX11*>(pDevice);

  ID3D11Device5* pD3d11Device5 = nullptr;

  if (SUCCEEDED(pDXDevice->GetDXImmediateContext()->QueryInterface(__uuidof(ID3D11DeviceContext4), (void**)&m_pDXDeviceContext)) &&
      SUCCEEDED(pDXDevice->GetDXDevice()->QueryInterface(__uuidof(ID3D11Device5), (void**)&pD3d11Device5)))
  {
    if (SUCCEEDED(pD3d11Device5->CreateFence(initialValue, D3D11_FENCE_FLAG_NONE, __uuidof(ID3D11Fence), (void**)&m_pDXFence2)))
    {
      m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

      EZ_GAL_DX11_RELEASE(pD3d11Device5);
      return EZ_SUCCESS;
    }
    else
    {
      ezLog::Error("Creation of native DirectX fence failed!");
      return EZ_FAILURE;
    }
  }
  else
  {
  }

  D3D11_QUERY_DESC QueryDesc;
  QueryDesc.Query = D3D11_QUERY_EVENT;
  QueryDesc.MiscFlags = 0;

  if (SUCCEEDED(pDXDevice->GetDXDevice()->CreateQuery(&QueryDesc, &m_pDXFence)))
  {
    return EZ_SUCCESS;
  }
  else
  {
    ezLog::Error("Creation of native DirectX fence failed!");
    return EZ_FAILURE;
  }
}

ezResult ezGALFenceDX11::DeInitPlatform(ezGALDevice* pDevice)
{
  //EZ_GAL_DX11_RELEASE(m_pDXFence);
  EZ_GAL_DX11_RELEASE(m_pDXFence2);
  EZ_GAL_DX11_RELEASE(m_pDXDeviceContext);

  return EZ_SUCCESS;
}

ezUInt64 ezGALFenceDX11::GetCompletedValuePlatform()
{
  return m_pDXFence2->GetCompletedValue();
}

void ezGALFenceDX11::WaitPlatform(ezUInt64 value)
{
  if (GetCompletedValuePlatform() < value)
  {
    m_pDXFence2->SetEventOnCompletion(value, m_FenceEvent);
    WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
  }
}

void ezGALFenceDX11::SignalPlatform(ezUInt64 value)
{
  m_pDXDeviceContext->Signal(m_pDXFence2, value);
}


EZ_STATICLINK_FILE(RendererDX11, RendererDX11_Resources_Implementation_FenceDX11);
