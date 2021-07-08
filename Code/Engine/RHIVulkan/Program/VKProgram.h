#pragma once
#include <RHI/Program/ProgramBase.h>
#include <RHI/Shader/Shader.h>
#include <map>
#include <set>
#include <vector>
#include <RHIVulkan/RHIVulkanDLL.h>

class VKDevice;

class VKProgram : public ProgramBase
{
public:
  VKProgram(VKDevice& device, const std::vector<std::shared_ptr<Shader>>& shaders);
};
