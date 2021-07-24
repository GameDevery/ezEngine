#pragma once

#include <Foundation/Basics.h>
#include <RendererFoundation/RendererFoundationDLL.h>
#include <RendererFoundation/Descriptors/Descriptors.h>
#include <Foundation/Types/SharedPtr.h>

// Configure the DLL Import/Export Define
#if EZ_ENABLED(EZ_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_RENDERERVK_LIB
#    define EZ_RENDERERVK_DLL __declspec(dllexport)
#  else
#    define EZ_RENDERERVK_DLL __declspec(dllimport)
#  endif
#else
#  define EZ_RENDERERVK_DLL
#endif

class ezGALDeviceVk;
class ezGALFenceVk;

//#define VK_USE_PLATFORM_WIN32_KHR

namespace ezInternal::Vk
{
  class Adapter;
  class Instance;
  class CommandQueue;
  class CommandList;

  enum class CommandListType
  {
    Graphics,
    Compute,
    Copy
  };
}
