#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Math/Vec3.h>
#include <Foundation/Containers/DynamicArray.h>
#include <Foundation/Algorithm/HashableStruct.h>

// Configure the DLL Import/Export Define
#if EZ_ENABLED(EZ_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_NAVIGATIONPLUGIN_LIB
#    define EZ_NAVIGATIONPLUGIN_DLL __declspec(dllexport)
#  else
#    define EZ_NAVIGATIONPLUGIN_DLL __declspec(dllimport)
#  endif
#else
#  define EZ_NAVIGATIONPLUGIN_DLL
#endif

class EZ_NAVIGATIONPLUGIN_DLL ezStaticColliderComponent
{
public:
  ezUInt32 GetId() const { return 0u; }
};
