#pragma once

#include <RHI/Base/Shader.h>
#include <RHI/Vulkan/VKStd.h>

class CCVKGPUShader;

class EZ_VULKAN_API CCVKShader final : public Shader
{
public:
  CCVKShader();
  ~CCVKShader() override;

  inline CCVKGPUShader* gpuShader() const { return _gpuShader; }

protected:
  void doInit(const ShaderInfo& info) override;
  void doDestroy() override;

  CCVKGPUShader* _gpuShader = nullptr;
};
