#include <Core/Input/InputManager.h>
#include <Core/ResourceManager/ResourceManager.h>
#include <Foundation/Configuration/Startup.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/Logging/ConsoleWriter.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Logging/VisualStudioWriter.h>
#include <Foundation/Time/Clock.h>
#include <RHISampleLowLevel/RHISampleLowLevel.h>

#include <Core/System/Window.h>

#include <RHIShaderCompilerHLSL/Compiler.h>
#include <RHIShaderCompilerHLSL/ShaderReflection.h>

static ezUInt32 g_uiWindowWidth = 640;
static ezUInt32 g_uiWindowHeight = 480;

class ezRHISampleWindow : public ezWindow
{
public:
  ezRHISampleWindow(ezRHISampleApp* pApp)
    : ezWindow()
  {
    m_pApp = pApp;
    m_bCloseRequested = false;
  }

  virtual void OnClickClose() override { m_bCloseRequested = true; }

  virtual void OnResize(const ezSizeU32& newWindowSize) override
  {
    if (m_pApp)
    {
      m_CreationDescription.m_Resolution = newWindowSize;
      m_pApp->OnResize(m_CreationDescription.m_Resolution.width, m_CreationDescription.m_Resolution.height);
    }
  }

  bool m_bCloseRequested;

private:
  ezRHISampleApp* m_pApp = nullptr;
};

ezSharedPtr<Shader> CreateShader()
{
  return nullptr;
}

ezRHISampleApp::ezRHISampleApp()
  : ezApplication("RHI Sample")
{
}

void ezRHISampleApp::AfterCoreSystemsStartup()
{
  ezStringBuilder sProjectDir = ">sdk/Data/Samples/RHISample";
  ezStringBuilder sProjectDirResolved;
  ezFileSystem::ResolveSpecialDirectory(sProjectDir, sProjectDirResolved).IgnoreResult();

  ezFileSystem::SetSpecialDirectory("project", sProjectDirResolved);

  ezFileSystem::AddDataDirectory("", "", ":", ezFileSystem::AllowWrites).IgnoreResult();
  ezFileSystem::AddDataDirectory(">appdir/", "AppBin", "bin", ezFileSystem::AllowWrites).IgnoreResult();                              // writing to the binary directory
  ezFileSystem::AddDataDirectory(">appdir/", "ShaderCache", "shadercache", ezFileSystem::AllowWrites).IgnoreResult();                 // for shader files
  ezFileSystem::AddDataDirectory(">user/ezEngine Project/RHISample", "AppData", "appdata", ezFileSystem::AllowWrites).IgnoreResult(); // app user data

  ezFileSystem::AddDataDirectory(">sdk/Data/Base", "Base", "base").IgnoreResult();
  ezFileSystem::AddDataDirectory(">project/", "Project", "project", ezFileSystem::AllowWrites).IgnoreResult();

  ezGlobalLog::AddLogWriter(ezLogWriter::Console::LogMessageHandler);
  ezGlobalLog::AddLogWriter(ezLogWriter::VisualStudio::LogMessageHandler);

  ezPlugin::LoadPlugin("ezInspectorPlugin").IgnoreResult();

  // Register Input
  {
    ezInputActionConfig cfg;

    cfg = ezInputManager::GetInputActionConfig("Main", "CloseApp");
    cfg.m_sInputSlotTrigger[0] = ezInputSlot_KeyEscape;
    ezInputManager::SetInputActionConfig("Main", "CloseApp", cfg, true);
  }

  
  ShaderBlobType shaderBlobType = ShaderBlobType::kSPIRV;
  ApiType apiType = ApiType::kVulkan;

  const char* szRendererName = ezCommandLineUtils::GetGlobalInstance()->GetStringOption("-rhi", 0, "dx");
  {
    if (ezStringUtils::Compare(szRendererName, "dx") == 0)
    {
      shaderBlobType = ShaderBlobType::kDXIL;
      apiType = ApiType::kDX12;
    }

    if (ezStringUtils::Compare(szRendererName, "vk") == 0)
    {
      shaderBlobType = ShaderBlobType::kSPIRV;
      apiType = ApiType::kVulkan;
    }
  }
  // Create a window for rendering
  {
    ezWindowCreationDesc WindowCreationDesc;
    WindowCreationDesc.m_Resolution.width = g_uiWindowWidth;
    WindowCreationDesc.m_Resolution.height = g_uiWindowHeight;
    WindowCreationDesc.m_Title = ezStringBuilder("RHISample ", szRendererName);
    WindowCreationDesc.m_bShowMouseCursor = true;
    WindowCreationDesc.m_bClipMouseCursor = false;
    WindowCreationDesc.m_WindowMode = ezWindowMode::WindowResizable;
    m_pWindow = EZ_DEFAULT_NEW(ezRHISampleWindow, this);
    m_pWindow->Initialize(WindowCreationDesc).IgnoreResult();
  }

  renderDeviceDesc.api_type = apiType;
  renderDeviceDesc.frame_count = FRAME_COUNT;
  renderDeviceDesc.round_fps = true;
  renderDeviceDesc.vsync = true;

  m_pInstance = InstanceFactory::CreateInstance(renderDeviceDesc.api_type);
  m_pAdapter = std::move(m_pInstance->EnumerateAdapters()[renderDeviceDesc.required_gpu_index]);
  m_pDevice = m_pAdapter->CreateDevice();

  // now that we have a window and m_pDevice, tell the engine to initialize the rendering infrastructure
  ezStartup::StartupHighLevelSystems();

  m_pCommandQueue = m_pDevice->GetCommandQueue(CommandListType::kGraphics);
  m_pSwapchain = m_pDevice->CreateSwapchain(m_pWindow->GetNativeWindowHandle(),
    m_pWindow->GetClientAreaSize().width,
    m_pWindow->GetClientAreaSize().height,
    FRAME_COUNT, renderDeviceDesc.vsync);
  m_pFence = m_pDevice->CreateFence(m_FenceValue);

  std::vector<ezUInt32> indexData = {0, 1, 2};
  m_pIndexBuffer = m_pDevice->CreateBuffer(BindFlag::kIndexBuffer | BindFlag::kCopyDest, (ezUInt32)(sizeof(ezUInt32) * indexData.size()));
  m_pIndexBuffer->CommitMemory(MemoryType::kUpload);
  m_pIndexBuffer->UpdateUploadBuffer(0, indexData.data(), sizeof(indexData.front()) * indexData.size());

  std::vector<ezVec3> vertexData = {ezVec3(-0.5, -0.5, 0.0), ezVec3(0.0, 0.5, 0.0), ezVec3(0.5, -0.5, 0.0)};
  m_pVertexBuffer = m_pDevice->CreateBuffer(BindFlag::kVertexBuffer | BindFlag::kCopyDest, (ezUInt32)(sizeof(vertexData.front()) * vertexData.size()));
  m_pVertexBuffer->CommitMemory(MemoryType::kUpload);
  m_pVertexBuffer->UpdateUploadBuffer(0, vertexData.data(), sizeof(vertexData.front()) * vertexData.size());

  ezVec4 constantData = ezVec4(1, 0, 0, 1);
  m_pConstantBuffer = m_pDevice->CreateBuffer(BindFlag::kConstantBuffer | BindFlag::kCopyDest, sizeof(constantData));
  m_pConstantBuffer->CommitMemory(MemoryType::kUpload);
  m_pConstantBuffer->UpdateUploadBuffer(0, &constantData, sizeof(constantData));

  ezStringBuilder projectDirAbsolutePath;
  if (!ezFileSystem::ResolveSpecialDirectory(">project", projectDirAbsolutePath).Succeeded())
  {
    EZ_REPORT_FAILURE("Project directory could not be resolved.");
  }

  ezStringBuilder vsShaderPath(projectDirAbsolutePath, "/shaders/Triangle/VertexShader.hlsl");
  vsShaderPath.MakeCleanPath();
  ShaderDesc vsDesc{vsShaderPath.GetData(), "main", ShaderType::kVertex, "6_0"};
  auto vsBlob = Compile(vsDesc, shaderBlobType);
  auto vsReflection = CreateShaderReflection(shaderBlobType, vsBlob.GetData(), vsBlob.GetCount());
  m_pVertexShader = m_pDevice->CreateShader(vsDesc, vsBlob, vsReflection);


  ezStringBuilder psShaderPath(projectDirAbsolutePath, "/shaders/Triangle/PixelShader.hlsl");
  psShaderPath.MakeCleanPath();
  ShaderDesc psDesc{psShaderPath.GetData(), "main", ShaderType::kPixel, "6_0"};
  auto psBlob = Compile(psDesc, shaderBlobType);
  auto psReflection = CreateShaderReflection(shaderBlobType, psBlob.GetData(), psBlob.GetCount());
  m_pPixelShader = m_pDevice->CreateShader(psDesc, psBlob, psReflection);

  m_pProgram = m_pDevice->CreateProgram({m_pVertexShader, m_pPixelShader});

  ViewDesc constantBufferViewDesc = {};
  constantBufferViewDesc.view_type = ViewType::kConstantBuffer;
  constantBufferViewDesc.dimension = ViewDimension::kBuffer;
  m_pConstantBufferView = m_pDevice->CreateView(m_pConstantBuffer, constantBufferViewDesc);

  BindKey settingsKey = {ShaderType::kPixel, ViewType::kConstantBuffer, 0, 0, 1};
  m_pLayout = m_pDevice->CreateBindingSetLayout({settingsKey});
  m_pBindingSet = m_pDevice->CreateBindingSet(m_pLayout);
  m_pBindingSet->WriteBindings({{settingsKey, m_pConstantBufferView}});

  RenderPassDesc renderPassDesc = {
    {{m_pSwapchain->GetFormat(), RenderPassLoadOp::kClear, RenderPassStoreOp::kStore}},
  };
  m_pRenderPass = m_pDevice->CreateRenderPass(renderPassDesc);


  GraphicsPipelineDesc pipelineDesc = {
    m_pProgram,
    m_pLayout,
    {{0, "POSITION", ezRHIResourceFormat::R32G32B32_FLOAT, sizeof(vertexData.front())}},
    m_pRenderPass};
  m_pPipeline = m_pDevice->CreateGraphicsPipeline(pipelineDesc);

  //for (ezUInt32 i = 0; i < FRAME_COUNT; ++i)
  //{
  //  ViewDesc backBufferViewDesc = {};
  //  backBufferViewDesc.view_type = ViewType::kRenderTarget;
  //  backBufferViewDesc.dimension = ViewDimension::kTexture2D;
  //  ezSharedPtr<Resource> backBuffer = m_pSwapchain->GetBackBuffer(i);
  //  ezSharedPtr<View> backBufferView = m_pDevice->CreateView(backBuffer, backBufferViewDesc);
  //  FramebufferDesc framebufferDesc = {};
  //  framebufferDesc.m_pRenderPass = m_pRenderPass;
  //  framebufferDesc.width = m_pWindow->GetClientAreaSize().width;
  //  framebufferDesc.height = m_pWindow->GetClientAreaSize().height;
  //  framebufferDesc.colors = {backBufferView};
  //  ezSharedPtr<Framebuffer> framebuffer = framebuffers.emplace_back(m_pDevice->CreateFramebuffer(framebufferDesc));
  //  std::shared_ptr<CommandList> commandList = command_lists.emplace_back(m_pDevice->CreateCommandList(CommandListType::kGraphics));
  //  commandList->BindPipeline(m_pPipeline);
  //  commandList->BindBindingSet(m_pBindingSet);
  //  commandList->SetViewport(0, 0, (float)m_pWindow->GetClientAreaSize().width, (float)m_pWindow->GetClientAreaSize().height);
  //  commandList->SetScissorRect(0, 0, m_pWindow->GetClientAreaSize().width, m_pWindow->GetClientAreaSize().height);
  //  commandList->IASetIndexBuffer(m_pIndexBuffer, ezRHIResourceFormat::R32_UINT);
  //  commandList->IASetVertexBuffer(0, m_pVertexBuffer);
  //  commandList->ResourceBarrier({{backBuffer, ResourceState::kPresent, ResourceState::kRenderTarget}});
  //  commandList->BeginRenderPass(m_pRenderPass, framebuffer, clear_desc);
  //  commandList->DrawIndexed(3, 1, 0, 0, 0);
  //  commandList->EndRenderPass();
  //  commandList->ResourceBarrier({{backBuffer, ResourceState::kRenderTarget, ResourceState::kPresent}});
  //  commandList->Close();
  //}
}

void ezRHISampleApp::BeforeHighLevelSystemsShutdown()
{
  // tell the engine that we are about to destroy window and graphics m_pDevice,
  // and that it therefore needs to cleanup anything that depends on that
  ezStartup::ShutdownHighLevelSystems();

  // destroy m_pDevice

  // finally destroy the window
  m_pWindow->Destroy().IgnoreResult();
  EZ_DEFAULT_DELETE(m_pWindow);
}

void ezRHISampleApp::OnResize(ezUInt32 width, ezUInt32 height)
{
  //m_pCommandQueue->Signal(m_pFence, ++m_FenceValue);
  //m_pFence->Wait(m_FenceValue);

  m_pSwapchain.Clear();
  m_pSwapchain = m_pDevice->CreateSwapchain(m_pWindow->GetNativeWindowHandle(), width, height, FRAME_COUNT, renderDeviceDesc.vsync);
  m_FrameIndex = 0;
}

ezApplication::Execution ezRHISampleApp::Run()
{
  m_pWindow->ProcessWindowMessages();

  if (m_pWindow->m_bCloseRequested || ezInputManager::GetInputActionState("Main", "CloseApp") == ezKeyState::Pressed)
    return Execution::Quit;

  // make sure time goes on
  ezClock::GetGlobalClock()->Update();

  // update all input state
  ezInputManager::Update(ezClock::GetGlobalClock()->GetTimeDiff());


  // do the rendering
  {
    m_FrameIndex = m_pSwapchain->NextImage(m_pFence, ++m_FenceValue);

    ClearDesc clear_desc = {{{0.0, 0.2, 0.4, 1.0}}};

    ViewDesc backBufferViewDesc = {};
    backBufferViewDesc.view_type = ViewType::kRenderTarget;
    backBufferViewDesc.dimension = ViewDimension::kTexture2D;
    ezSharedPtr<Resource> backBuffer = m_pSwapchain->GetBackBuffer(m_FrameIndex);
    ezSharedPtr<View> backBufferView = m_pDevice->CreateView(backBuffer, backBufferViewDesc);
    FramebufferDesc framebufferDesc = {};
    framebufferDesc.render_pass = m_pRenderPass;
    framebufferDesc.width = m_pWindow->GetClientAreaSize().width;
    framebufferDesc.height = m_pWindow->GetClientAreaSize().height;
    framebufferDesc.colors = {backBufferView};
    ezSharedPtr<Framebuffer> framebuffer = m_pDevice->CreateFramebuffer(framebufferDesc);
    ezSharedPtr<CommandList> commandList = m_pDevice->CreateCommandList(CommandListType::kGraphics);
    commandList->BindPipeline(m_pPipeline);
    commandList->BindBindingSet(m_pBindingSet);
    commandList->SetViewport(0, 0, (float)m_pWindow->GetClientAreaSize().width, (float)m_pWindow->GetClientAreaSize().height);
    commandList->SetScissorRect(0, 0, m_pWindow->GetClientAreaSize().width, m_pWindow->GetClientAreaSize().height);
    commandList->IASetIndexBuffer(m_pIndexBuffer, ezRHIResourceFormat::R32_UINT);
    commandList->IASetVertexBuffer(0, m_pVertexBuffer);
    commandList->ResourceBarrier({{backBuffer, ResourceState::kPresent, ResourceState::kRenderTarget}});
    commandList->BeginRenderPass(m_pRenderPass, framebuffer, clear_desc);
    commandList->DrawIndexed(3, 1, 0, 0, 0);
    commandList->EndRenderPass();
    commandList->ResourceBarrier({{backBuffer, ResourceState::kRenderTarget, ResourceState::kPresent}});
    commandList->Close();



    m_pCommandQueue->Wait(m_pFence, m_FenceValue);
    m_pFence->Wait(m_FenceValues[m_FrameIndex]);
    m_pCommandQueue->ExecuteCommandLists({commandList});
    m_pCommandQueue->Signal(m_pFence, m_FenceValues[m_FrameIndex] = ++m_FenceValue);
    m_pSwapchain->Present(m_pFence, m_FenceValues[m_FrameIndex]);


    m_pCommandQueue->Signal(m_pFence, ++m_FenceValue);
    m_pFence->Wait(m_FenceValue);
  }

  // needs to be called once per frame
  ezResourceManager::PerFrameUpdate();

  // tell the task system to finish its work for this frame
  // this has to be done at the very end, so that the task system will only use up the time that is left in this frame for
  // uploading GPU data etc.
  ezTaskSystem::FinishFrameTasks();

  return ezApplication::Execution::Continue;
}

EZ_CONSOLEAPP_ENTRY_POINT(ezRHISampleApp);
