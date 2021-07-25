#pragma once

#include <memory>
#include <Foundation/Application/Application.h>
#include <Foundation/Types/UniquePtr.h>

#include <RHI/Device/Device.h>
#include <RHI/Swapchain/Swapchain.h>
#include <RHI/Fence/Fence.h>
#include <RHI/Instance/Instance.h>

class ezRHISampleWindow;

constexpr ezUInt32 FRAME_COUNT = 3;

class ezRHISampleApp : public ezApplication
{
public:
  typedef ezApplication SUPER;

  ezRHISampleApp();

  virtual Execution Run() override;

  virtual void AfterCoreSystemsStartup() override;

  virtual void BeforeHighLevelSystemsShutdown() override;

  void OnResize(ezUInt32 width, ezUInt32 height);

private:
  ezSharedPtr<Instance> m_pInstance;
  ezSharedPtr<Adapter> m_pAdapter;
  ezSharedPtr<Device> m_pDevice;
  ezRHISampleWindow* m_pWindow = nullptr;
  ezSharedPtr<Swapchain> m_pSwapchain = nullptr;
  ezSharedPtr<Fence> m_pFence = nullptr;
  ezUInt64 m_FenceValue = 0;
  ezSharedPtr<CommandQueue> m_pCommandQueue = nullptr;
  std::array<ezUInt64, FRAME_COUNT> m_FenceValues = {};
  //std::vector<std::shared_ptr<CommandList>> command_lists;
  //std::vector<ezSharedPtr<Framebuffer>> framebuffers;
  ezSharedPtr<Pipeline> m_pPipeline;
  ezSharedPtr<RenderPass> m_pRenderPass;
  ezSharedPtr<Program> m_pProgram;
  ezSharedPtr<View> m_pConstantBufferView;
  ezSharedPtr<Resource> m_pConstantBuffer;
  ezSharedPtr<Resource> m_pIndexBuffer;
  ezSharedPtr<Resource> m_pVertexBuffer;
  ezDynamicArray<ezSharedPtr<Shader>> m_Shaders;
  //ezSharedPtr<Shader> m_pVertexShader;
  //ezSharedPtr<Shader> m_pPixelShader;
  ezSharedPtr<BindingSetLayout> m_pLayout;
  ezSharedPtr<BindingSet> m_pBindingSet;
  ezUInt32 m_FrameIndex = 0;
  RenderDeviceDesc renderDeviceDesc;
};
