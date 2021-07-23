#include <RendererFoundationPCH.h>

#include <RendererFoundation/Resources/Fence.h>

ezUInt64 ezGALFence::GetCompletedValue()
{
  return GetCompletedValuePlatform();
}

void ezGALFence::Wait(ezUInt64 value)
{
  WaitPlatform(value);
}

void ezGALFence::Signal(ezUInt64 value)
{
  SignalPlatform(value);
}

ezGALFence::ezGALFence() {}

ezGALFence::~ezGALFence() {}



EZ_STATICLINK_FILE(RendererFoundation, RendererFoundation_Resources_Implementation_Fence);
