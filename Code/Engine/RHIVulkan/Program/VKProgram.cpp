#include <RHIVulkanPCH.h>

#include <RHI/Shader/Shader.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Program/VKProgram.h>

VKProgram::VKProgram(VKDevice& device, const std::vector<ezSharedPtr<Shader>>& shaders)
  : ProgramBase(shaders)
{
}
