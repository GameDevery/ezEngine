#pragma once

#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Utilities/Progress.h>
#include <RendererCore/RendererCoreDLL.h>

struct EZ_RENDERERCORE_DLL ezBakingSettings
{
  ezVec3 m_vProbeSpacing = ezVec3(4);
  ezUInt32 m_uiNumSamplesPerProbe = 128;
  float m_fMaxRayDistance = 1000.0f;

  ezResult Serialize(ezStreamWriter& stream) const;
  ezResult Deserialize(ezStreamReader& stream);
};

EZ_DECLARE_REFLECTABLE_TYPE(EZ_RENDERERCORE_DLL, ezBakingSettings);

class ezWorld;

class ezBakingInterface
{
public:
  /// \brief Renders a debug view of the baking scene
  virtual ezResult RenderDebugView(const ezWorld& world, const ezMat4& InverseViewProjection, ezUInt32 uiWidth, ezUInt32 uiHeight, ezDynamicArray<ezColorGammaUB>& out_Pixels, ezProgress& progress) const = 0;
};