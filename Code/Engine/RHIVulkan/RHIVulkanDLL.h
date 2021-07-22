#pragma once

#include <Foundation/Basics.h>
#include <RHI/RHIDLL.h>

// Configure the DLL Import/Export Define
#if EZ_ENABLED(EZ_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_RHIVULKAN_LIB
#    define EZ_RHIVULKAN_DLL __declspec(dllexport)
#  else
#    define EZ_RHIVULKAN_DLL __declspec(dllimport)
#  endif
#else
#  define EZ_RHIVULKAN_DLL
#endif

#include <vulkan/vulkan.hpp>

constexpr ezUInt32 MAX_BINDLESS_HEAP_SIZE = 10000;

class ezRHIVKAdapter;
class ezRHIVKBindingSet;
class ezRHIVKBindingSetLayout;
class ezRHIVKCommandList;
class ezRHIVKCommandQueue;
class ezRHIVKDevice;
class ezRHIVKTimelineSemaphore;
class ezRHIVKFramebuffer;
class ezRHIVKGPUBindlessDescriptorPoolTyped;
class ezRHIVKGPUDescriptorPool;
class ezRHIVKGPUDescriptorPoolRange;
class ezRHIVKInstance;
class ezRHIVKMemory;
class ezRHIVKComputePipeline;
class ezRHIVKGraphicsPipeline;
class ezRHIVKPipeline;
class ezRHIVKRayTracingPipeline;
class ezRHIVKProgram;
class ezRHIVKQueryHeap;
class ezRHIVKRenderPass;
class ezRHIVKResource;
class ezRHIVKSwapChain;
class ezRHIVKResourceView;

EZ_DEFINE_AS_POD_TYPE(vk::DynamicState);
EZ_DEFINE_AS_POD_TYPE(vk::FragmentShadingRateCombinerOpKHR);

struct ezRHIVKDescriptorSetPool
{
  vk::UniqueDescriptorPool m_Pool;
  vk::UniqueDescriptorSet m_Set;
};

namespace VK
{
  vk::Format ToVKFormat(ezRHIResourceFormat::Enum format);
  ezRHIResourceFormat::Enum ToEngineFormat(vk::Format format);
}