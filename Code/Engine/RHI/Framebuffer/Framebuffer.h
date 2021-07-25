#pragma once
#include <RHI/RHIDLL.h>

class EZ_RHI_DLL Framebuffer : public ezRefCounted
{
public:
  virtual ~Framebuffer() = default;
};
