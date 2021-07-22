#include <RHIPCH.h>

#include <RHI/Descriptors/Descriptors.h>
#include <RHI/Device/CommandQueue.h>
#include <RHI/Device/Device.h>

struct RHIObjectType
{
  enum Enum
  {
    BindingSet,
    BindingSetLayout,
    CommandList,
    Fence,
    Framebuffer,
    Memory,
    Pipeline,
    Program,
    QueryHeap,
    RenderPass,
    Resource,
    ResourceView,
    Shader,
    SwapChain
  };
};

ezRHIDevice::ezRHIDevice()
  : m_Allocator("RHIDevice", ezFoundation::GetDefaultAllocator())
  , m_AllocatorWrapper(&m_Allocator)
{
}

const ezRHIDeviceCapabilities& ezRHIDevice::GetCapabilities() const
{
  return m_Capabilities;
}

ezResult ezRHIDevice::Initialize()
{
  return InitializePlatform();
}

ezResult ezRHIDevice::Shutdown()
{
  return ShutdownPlatform();
}

void ezRHIDevice::BeginFrame()
{
  {
    ezRHIDeviceEvent e;
    e.m_pDevice = this;
    e.m_Type = ezRHIDeviceEvent::BeforeBeginFrame;
    m_Events.Broadcast(e);
  }

  {
    EZ_ASSERT_DEV(!m_bBeginFrameCalled, "You must call ezRHIDevice::EndFrame before you can call ezRHIDevice::BeginFrame again");

    m_bBeginFrameCalled = true;

    BeginFramePlatform();
  }

  {
    ezRHIDeviceEvent e;
    e.m_pDevice = this;
    e.m_Type = ezRHIDeviceEvent::AfterBeginFrame;
    m_Events.Broadcast(e);
  }
}

void ezRHIDevice::EndFrame()
{
  {
    ezRHIDeviceEvent e;
    e.m_pDevice = this;
    e.m_Type = ezRHIDeviceEvent::BeforeEndFrame;
    m_Events.Broadcast(e);
  }

  {
    EZ_ASSERT_DEV(m_bBeginFrameCalled, "You must have called ezRHIDevice::Begin before you can call ezRHIDevice::EndFrame");

    EndFramePlatform();

    m_bBeginFrameCalled = false;
  }

  {
    ezRHIDeviceEvent e;
    e.m_pDevice = this;
    e.m_Type = ezRHIDeviceEvent::AfterEndFrame;
    m_Events.Broadcast(e);
  }
}

ezRHIMemoryHandle ezRHIDevice::AllocateMemory(ezUInt64 size, ezRHIMemoryType memoryType, ezUInt32 memoryTypeBits)
{
  return ezRHIMemoryHandle();
}

void ezRHIDevice::FreeMemory(ezRHIMemoryHandle hMemory)
{
}

ezSharedPtr<ezRHICommandQueue> ezRHIDevice::GetCommandQueue(ezRHICommandListType type)
{
  return GetCommandQueuePlatform(type);
}

ezRHISwapChainHandle ezRHIDevice::CreateSwapchain(const ezRHISwapChainCreationDescription& desc)
{
  return ezRHISwapChainHandle();
}

void ezRHIDevice::DestroySwapChain(ezRHISwapChainHandle hSwapChain)
{
}

ezRHICommandListHandle ezRHIDevice::CreateCommandList(ezRHICommandListType type)
{
  return ezRHICommandListHandle();
}

void ezRHIDevice::DestroyCommandList(ezRHICommandListHandle hCommandList)
{
}

ezRHIFenceHandle ezRHIDevice::CreateFence(ezUInt64 initialValue)
{
  return ezRHIFenceHandle();
}

void ezRHIDevice::DestroyFence(ezRHIFenceHandle hFence)
{
}

ezRHIResourceHandle ezRHIDevice::CreateTexture(const ezRHITextureCreationDescription& desc)
{
  return ezRHIResourceHandle();
}

void ezRHIDevice::DestroyTexture(ezRHIResourceHandle hTexture)
{
}

ezRHIResourceHandle ezRHIDevice::CreateBuffer(const ezRHIBufferCreationDescription& desc)
{
  return ezRHIResourceHandle();
}

void ezRHIDevice::DestroyBuffer(ezRHIResourceHandle hBuffer)
{
}

ezRHIResourceHandle ezRHIDevice::CreateSampler(const ezRHISamplerCreationDescription& desc)
{
  return ezRHIResourceHandle();
}

void ezRHIDevice::DestroySampler(ezRHIResourceHandle hSampler)
{
}

ezRHIResourceViewHandle ezRHIDevice::CreateResourceView(ezRHIResourceHandle hResource, const ezRHIResourceViewCreationDescription& desc)
{
  return ezRHIResourceViewHandle();
}

void ezRHIDevice::DestroyResourceView(ezRHIResourceViewHandle hResourceView)
{
}

ezRHIBindingSetLayoutHandle ezRHIDevice::CreateBindingSetLayout(const ezDynamicArray<ezRHIBindKeyDescription>& descs)
{
  return ezRHIBindingSetLayoutHandle();
}

void ezRHIDevice::DestroyBindingSetLayout(ezRHIBindingSetLayoutHandle hBindingSetLayout)
{
}

ezRHIBindingSetHandle ezRHIDevice::CreateBindingSet(ezRHIBindingSetLayoutHandle hLayout)
{
  return ezRHIBindingSetHandle();
}

void ezRHIDevice::DestroyBindingSet(ezRHIBindingSetHandle hBindingSet)
{
}

ezRHIRenderPassHandle ezRHIDevice::CreateRenderPass(const ezRHIRenderPassCreationDescription& desc)
{
  return ezRHIRenderPassHandle();
}

void ezRHIDevice::DestroyRenderPass(ezRHIRenderPassHandle hRenderPass)
{
}

ezRHIFramebufferHandle ezRHIDevice::CreateFramebuffer(const ezRHIFramebufferCreationDescription& desc)
{
  return ezRHIFramebufferHandle();
}

void ezRHIDevice::DestroyFramebuffer(ezRHIFramebufferHandle hFrameBuffer)
{
}

ezRHIShaderHandle ezRHIDevice::CreateShader(const ezRHIShaderCreationDescription& desc)
{
  return ezRHIShaderHandle();
}

void ezRHIDevice::DestroyShader(ezRHIShaderHandle hShader)
{
}

ezRHIProgramHandle ezRHIDevice::CreateProgram(const ezDynamicArray<ezRHIShaderHandle>& shaders)
{
  return ezRHIProgramHandle();
}

void ezRHIDevice::DestroyProgram(ezRHIProgramHandle hProgram)
{
}

ezRHIPipelineHandle ezRHIDevice::CreateGraphicsPipeline(const ezRHIGraphicsPipelineCreationDescription& desc)
{
  return ezRHIPipelineHandle();
}

void ezRHIDevice::DestroyGraphicsPipeline(ezRHIPipelineHandle hPipeline)
{
}

ezRHIPipelineHandle ezRHIDevice::CreateComputePipeline(const ezRHIComputePipelineCreationDescription& desc)
{
  return ezRHIPipelineHandle();
}

void ezRHIDevice::DestroyComputePipeline(ezRHIPipelineHandle hPipeline)
{
}

ezRHIPipelineHandle ezRHIDevice::CreateRayTracingPipeline(const ezRHIRayTracingPipelineCreationDescription& desc)
{
  return ezRHIPipelineHandle();
}

void ezRHIDevice::DestroyRayTracingPipeline(ezRHIPipelineHandle hPipeline)
{
}

ezRHIResourceHandle ezRHIDevice::CreateAccelerationStructure(ezRHIAccelerationStructureType type, ezRHIResourceHandle hResource, ezUInt64 offset)
{
  return ezRHIResourceHandle();
}

void ezRHIDevice::DestroyAccelerationStructure(ezRHIResourceHandle hAccelerationStructure)
{
}

ezRHIQueryHeapHandle ezRHIDevice::CreateQueryHeap(ezRHIQueryHeapType type, ezUInt32 count)
{
  return ezRHIQueryHeapHandle();
}

void ezRHIDevice::DestroyQueryHeap(ezRHIQueryHeapHandle hQueryHeap)
{
}

ezRHISwapChain* ezRHIDevice::GetSwapchain(ezRHISwapChainHandle hSwapChain)
{
  return nullptr;
}

ezRHICommandList* ezRHIDevice::GetCommandList(ezRHICommandListHandle hCommandList)
{
  return nullptr;
}

ezRHIFence* ezRHIDevice::GetFence(ezRHIFenceHandle hFence)
{
  return nullptr;
}

ezRHIResource* ezRHIDevice::GetResource(ezRHIResourceHandle hTexture)
{
  return nullptr;
}

ezRHIResourceView* ezRHIDevice::GetResourceView(ezRHIResourceViewHandle hResourceView)
{
  return nullptr;
}

ezRHIBindingSetLayout* ezRHIDevice::GetBindingSetLayout(ezRHIBindingSetLayoutHandle hBindingSetLayout)
{
  return nullptr;
}

ezRHIBindingSet* ezRHIDevice::GetBindingSet(ezRHIBindingSetHandle hBindingSet)
{
  return nullptr;
}

ezRHIRenderPass* ezRHIDevice::GetRenderPass(ezRHIRenderPassHandle hRenderPass)
{
  return nullptr;
}

ezRHIFramebuffer* ezRHIDevice::GetFramebuffer(ezRHIFramebufferHandle hFramebuffer)
{
  return nullptr;
}

ezRHIShader* ezRHIDevice::GetShader(ezRHIShaderHandle hShader)
{
  return nullptr;
}

ezRHIProgram* ezRHIDevice::GetProgram(ezRHIProgramHandle hProgram)
{
  return nullptr;
}

ezRHIPipeline* ezRHIDevice::GetPipeline(ezRHIPipelineHandle hPipeline)
{
  return nullptr;
}

ezRHIResource* ezRHIDevice::GetAccelerationStructure(ezRHIResourceHandle hAccelerationStructure)
{
  return nullptr;
}

ezRHIQueryHeap* ezRHIDevice::GetQueryHeap(ezRHIQueryHeapHandle hQueryHeap)
{
  return nullptr;
}
