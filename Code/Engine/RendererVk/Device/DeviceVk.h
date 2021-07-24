#pragma once

#include <Foundation/Types/Bitflags.h>
#include <Foundation/Types/UniquePtr.h>
#include <RendererFoundation/Device/Device.h>
#include <RendererVk/RendererVkDLL.h>

#include <vulkan/vulkan.hpp>

class ezGALPassVk;

/// \brief The Vk device implementation of the graphics abstraction layer.
class EZ_RENDERERVK_DLL ezGALDeviceVk : public ezGALDevice
{
private:
  friend ezInternal::NewInstance<ezGALDevice> CreateVkDevice(ezAllocatorBase* pAllocator, const ezGALDeviceCreationDescription& Description);
  ezGALDeviceVk(const ezGALDeviceCreationDescription& Description);

public:
  virtual ~ezGALDeviceVk();

public:
  void ReportLiveGpuObjects();

  // These functions need to be implemented by a render API abstraction
protected:
  // Init & shutdown functions

  /// \brief Internal version of device init that allows to modify device creation flags and graphics adapter.
  ///
  /// \param pUsedAdapter
  ///   Null means default adapter.
  ezResult InitPlatform(ezUInt32 flags, void* pUsedAdapter);

  virtual ezResult InitPlatform() override;
  virtual ezResult ShutdownPlatform() override;

  // Pipeline & Pass functions

  virtual void BeginPipelinePlatform(const char* szName) override;
  virtual void EndPipelinePlatform() override;

  virtual ezGALPass* BeginPassPlatform(const char* szName) override;
  virtual void EndPassPlatform(ezGALPass* pPass) override;


  // State creation functions

  virtual ezGALBlendState* CreateBlendStatePlatform(const ezGALBlendStateCreationDescription& Description) override;
  virtual void DestroyBlendStatePlatform(ezGALBlendState* pBlendState) override;

  virtual ezGALDepthStencilState* CreateDepthStencilStatePlatform(const ezGALDepthStencilStateCreationDescription& Description) override;
  virtual void DestroyDepthStencilStatePlatform(ezGALDepthStencilState* pDepthStencilState) override;

  virtual ezGALRasterizerState* CreateRasterizerStatePlatform(const ezGALRasterizerStateCreationDescription& Description) override;
  virtual void DestroyRasterizerStatePlatform(ezGALRasterizerState* pRasterizerState) override;

  virtual ezGALSamplerState* CreateSamplerStatePlatform(const ezGALSamplerStateCreationDescription& Description) override;
  virtual void DestroySamplerStatePlatform(ezGALSamplerState* pSamplerState) override;


  // Resource creation functions

  virtual ezGALShader* CreateShaderPlatform(const ezGALShaderCreationDescription& Description) override;
  virtual void DestroyShaderPlatform(ezGALShader* pShader) override;

  virtual ezGALBuffer* CreateBufferPlatform(const ezGALBufferCreationDescription& Description, ezArrayPtr<const ezUInt8> pInitialData) override;
  virtual void DestroyBufferPlatform(ezGALBuffer* pBuffer) override;

  virtual ezGALTexture* CreateTexturePlatform(const ezGALTextureCreationDescription& Description, ezArrayPtr<ezGALSystemMemoryDescription> pInitialData) override;
  virtual void DestroyTexturePlatform(ezGALTexture* pTexture) override;

  virtual ezGALResourceView* CreateResourceViewPlatform(ezGALResourceBase* pResource, const ezGALResourceViewCreationDescription& Description) override;
  virtual void DestroyResourceViewPlatform(ezGALResourceView* pResourceView) override;

  virtual ezGALRenderTargetView* CreateRenderTargetViewPlatform(ezGALTexture* pTexture, const ezGALRenderTargetViewCreationDescription& Description) override;
  virtual void DestroyRenderTargetViewPlatform(ezGALRenderTargetView* pRenderTargetView) override;

  ezGALUnorderedAccessView* CreateUnorderedAccessViewPlatform(ezGALResourceBase* pResource, const ezGALUnorderedAccessViewCreationDescription& Description) override;
  virtual void DestroyUnorderedAccessViewPlatform(ezGALUnorderedAccessView* pResource) override;

  // Other rendering creation functions

  virtual ezGALSwapChain* CreateSwapChainPlatform(const ezGALSwapChainCreationDescription& Description) override;
  virtual void DestroySwapChainPlatform(ezGALSwapChain* pSwapChain) override;

  virtual ezGALFence* CreateFencePlatform(ezUInt64 initialValue) override;
  virtual void DestroyFencePlatform(ezGALFence* pFence) override;

  virtual ezGALQuery* CreateQueryPlatform(const ezGALQueryCreationDescription& Description) override;
  virtual void DestroyQueryPlatform(ezGALQuery* pQuery) override;

  virtual ezGALVertexDeclaration* CreateVertexDeclarationPlatform(const ezGALVertexDeclarationCreationDescription& Description) override;
  virtual void DestroyVertexDeclarationPlatform(ezGALVertexDeclaration* pVertexDeclaration) override;

  // Timestamp functions

  virtual ezGALTimestampHandle GetTimestampPlatform() override;
  virtual ezResult GetTimestampResultPlatform(ezGALTimestampHandle hTimestamp, ezTime& result) override;

  // Swap chain functions

  virtual void PresentPlatform(ezGALSwapChain* pSwapChain, bool bVSync) override;

  // Misc functions

  virtual void BeginFramePlatform() override;
  virtual void EndFramePlatform() override;

  virtual void SetPrimarySwapChainPlatform(ezGALSwapChain* pSwapChain) override;

  virtual void FillCapabilitiesPlatform() override;

  /// \endcond


  // Internal
public:
  EZ_ALWAYS_INLINE ezInternal::Vk::Instance* GetVkInstance() const {
    return m_pInstance;
  }

  EZ_ALWAYS_INLINE vk::Device GetVkDevice() const {
    return m_VkDevice.get();
  }

  ezInternal::Vk::CommandListType GetAvailableCommandListType(ezInternal::Vk::CommandListType type) const;
  vk::CommandPool GetCmdPool(ezInternal::Vk::CommandListType type) const;

private:
  friend class ezGALCommandEncoderImplVk;

  struct QueueInfo
  {
    ezUInt32 QueueFamilyIndex;
    ezUInt32 QueueCount;
  };
  ezMap<ezInternal::Vk::CommandListType, QueueInfo> m_QueuesInfo;

  ezInternal::Vk::Instance* m_pInstance;
  ezSharedPtr<ezInternal::Vk::Adapter> m_pAdapter;
  vk::UniqueDevice m_VkDevice;
  ezMap<ezInternal::Vk::CommandListType, vk::UniqueCommandPool> m_CmdPools;
  ezMap<ezInternal::Vk::CommandListType, ezInternal::Vk::CommandQueue*> m_CommandQueues;

  ezUInt32 m_TextureDataPitchAlignment;
  bool m_IsDxrSupported;
  bool m_IsRayQuerySupported;
  bool m_IsVariableRateShadingSupported;
  bool m_IsMeshShadingSupported;
  ezUInt32 m_ShadingRateImageTileSize;
  //ezGALMemoryBudget m_MemoryBudget;
  ezUInt32 m_ShaderGroupHandleSize;
  ezUInt32 m_ShaderRecordAlignment;
  ezUInt32 m_ShaderTableAlignment;

  ezUniquePtr<ezGALPassVk> m_pDefaultPass;
};

#include <RendererVk/Device/Implementation/DeviceVk_inl.h>
