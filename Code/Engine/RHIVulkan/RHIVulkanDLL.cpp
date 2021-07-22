#include <RHIVulkanPCH.h>

#include <RHIVulkan/RHIVulkanDLL.h>

struct FormatMapping
{
  ezRHIResourceFormat::Enum m_EngineFormat;
  vk::Format m_VKFormat;
};

static const FormatMapping c_FormatMap[] = {
  {ezRHIResourceFormat::Unknown, vk::Format(VK_FORMAT_UNDEFINED)},
  {ezRHIResourceFormat::R8_UINT, vk::Format(VK_FORMAT_R8_UINT)},
  {ezRHIResourceFormat::R8_SINT, vk::Format(VK_FORMAT_R8_SINT)},
  {ezRHIResourceFormat::R8_UNORM, vk::Format(VK_FORMAT_R8_UNORM)},
  {ezRHIResourceFormat::R8_SNORM, vk::Format(VK_FORMAT_R8_SNORM)},
  {ezRHIResourceFormat::RG8_UINT, vk::Format(VK_FORMAT_R8G8_UINT)},
  {ezRHIResourceFormat::RG8_SINT, vk::Format(VK_FORMAT_R8G8_SINT)},
  {ezRHIResourceFormat::RG8_UNORM, vk::Format(VK_FORMAT_R8G8_UNORM)},
  {ezRHIResourceFormat::RG8_SNORM, vk::Format(VK_FORMAT_R8G8_SNORM)},
  {ezRHIResourceFormat::R16_UINT, vk::Format(VK_FORMAT_R16_UINT)},
  {ezRHIResourceFormat::R16_SINT, vk::Format(VK_FORMAT_R16_SINT)},
  {ezRHIResourceFormat::R16_UNORM, vk::Format(VK_FORMAT_R16_UNORM)},
  {ezRHIResourceFormat::R16_SNORM, vk::Format(VK_FORMAT_R16_SNORM)},
  {ezRHIResourceFormat::R16_FLOAT, vk::Format(VK_FORMAT_R16_SFLOAT)},
  {ezRHIResourceFormat::BGRA4_UNORM, vk::Format(VK_FORMAT_B4G4R4A4_UNORM_PACK16)},
  {ezRHIResourceFormat::B5G6R5_UNORM, vk::Format(VK_FORMAT_B5G6R5_UNORM_PACK16)},
  {ezRHIResourceFormat::B5G5R5A1_UNORM, vk::Format(VK_FORMAT_B5G5R5A1_UNORM_PACK16)},
  {ezRHIResourceFormat::RGBA8_UINT, vk::Format(VK_FORMAT_R8G8B8A8_UINT)},
  {ezRHIResourceFormat::RGBA8_SINT, vk::Format(VK_FORMAT_R8G8B8A8_SINT)},
  {ezRHIResourceFormat::RGBA8_UNORM, vk::Format(VK_FORMAT_R8G8B8A8_UNORM)},
  {ezRHIResourceFormat::RGBA8_SNORM, vk::Format(VK_FORMAT_R8G8B8A8_SNORM)},
  {ezRHIResourceFormat::BGRA8_UNORM, vk::Format(VK_FORMAT_B8G8R8A8_UNORM)},
  {ezRHIResourceFormat::SRGBA8_UNORM, vk::Format(VK_FORMAT_R8G8B8A8_SRGB)},
  {ezRHIResourceFormat::SBGRA8_UNORM, vk::Format(VK_FORMAT_B8G8R8A8_SRGB)},
  {ezRHIResourceFormat::R10G10B10A2_UNORM, vk::Format(VK_FORMAT_A2B10G10R10_UNORM_PACK32)},
  {ezRHIResourceFormat::R11G11B10_FLOAT, vk::Format(VK_FORMAT_B10G11R11_UFLOAT_PACK32)},
  {ezRHIResourceFormat::RG16_UINT, vk::Format(VK_FORMAT_R16G16_UINT)},
  {ezRHIResourceFormat::RG16_SINT, vk::Format(VK_FORMAT_R16G16_SINT)},
  {ezRHIResourceFormat::RG16_UNORM, vk::Format(VK_FORMAT_R16G16_UNORM)},
  {ezRHIResourceFormat::RG16_SNORM, vk::Format(VK_FORMAT_R16G16_SNORM)},
  {ezRHIResourceFormat::RG16_FLOAT, vk::Format(VK_FORMAT_R16G16_SFLOAT)},
  {ezRHIResourceFormat::R32_UINT, vk::Format(VK_FORMAT_R32_UINT)},
  {ezRHIResourceFormat::R32_SINT, vk::Format(VK_FORMAT_R32_SINT)},
  {ezRHIResourceFormat::R32_FLOAT, vk::Format(VK_FORMAT_R32_SFLOAT)},
  {ezRHIResourceFormat::RGBA16_UINT, vk::Format(VK_FORMAT_R16G16B16A16_UINT)},
  {ezRHIResourceFormat::RGBA16_SINT, vk::Format(VK_FORMAT_R16G16B16A16_SINT)},
  {ezRHIResourceFormat::RGBA16_FLOAT, vk::Format(VK_FORMAT_R16G16B16A16_SFLOAT)},
  {ezRHIResourceFormat::RGBA16_UNORM, vk::Format(VK_FORMAT_R16G16B16A16_UNORM)},
  {ezRHIResourceFormat::RGBA16_SNORM, vk::Format(VK_FORMAT_R16G16B16A16_SNORM)},
  {ezRHIResourceFormat::RG32_UINT, vk::Format(VK_FORMAT_R32G32_UINT)},
  {ezRHIResourceFormat::RG32_SINT, vk::Format(VK_FORMAT_R32G32_SINT)},
  {ezRHIResourceFormat::RG32_FLOAT, vk::Format(VK_FORMAT_R32G32_SFLOAT)},
  {ezRHIResourceFormat::RGB32_UINT, vk::Format(VK_FORMAT_R32G32B32_UINT)},
  {ezRHIResourceFormat::RGB32_SINT, vk::Format(VK_FORMAT_R32G32B32_SINT)},
  {ezRHIResourceFormat::RGB32_FLOAT, vk::Format(VK_FORMAT_R32G32B32_SFLOAT)},
  {ezRHIResourceFormat::RGBA32_UINT, vk::Format(VK_FORMAT_R32G32B32A32_UINT)},
  {ezRHIResourceFormat::RGBA32_SINT, vk::Format(VK_FORMAT_R32G32B32A32_SINT)},
  {ezRHIResourceFormat::RGBA32_FLOAT, vk::Format(VK_FORMAT_R32G32B32A32_SFLOAT)},
  {ezRHIResourceFormat::D16, vk::Format(VK_FORMAT_D16_UNORM)},
  {ezRHIResourceFormat::D24S8, vk::Format(VK_FORMAT_D24_UNORM_S8_UINT)},
  {ezRHIResourceFormat::X24G8_UINT, vk::Format(VK_FORMAT_D24_UNORM_S8_UINT)},
  {ezRHIResourceFormat::D32, vk::Format(VK_FORMAT_D32_SFLOAT)},
  {ezRHIResourceFormat::D32S8, vk::Format(VK_FORMAT_D32_SFLOAT_S8_UINT)},
  {ezRHIResourceFormat::X32G8_UINT, vk::Format(VK_FORMAT_D32_SFLOAT_S8_UINT)},
  {ezRHIResourceFormat::BC1_UNORM, vk::Format(VK_FORMAT_BC1_RGB_UNORM_BLOCK)},
  {ezRHIResourceFormat::BC1_UNORM_SRGB, vk::Format(VK_FORMAT_BC1_RGB_SRGB_BLOCK)},
  {ezRHIResourceFormat::BC2_UNORM, vk::Format(VK_FORMAT_BC2_UNORM_BLOCK)},
  {ezRHIResourceFormat::BC2_UNORM_SRGB, vk::Format(VK_FORMAT_BC2_SRGB_BLOCK)},
  {ezRHIResourceFormat::BC3_UNORM, vk::Format(VK_FORMAT_BC3_UNORM_BLOCK)},
  {ezRHIResourceFormat::BC3_UNORM_SRGB, vk::Format(VK_FORMAT_BC3_SRGB_BLOCK)},
  {ezRHIResourceFormat::BC4_UNORM, vk::Format(VK_FORMAT_BC4_UNORM_BLOCK)},
  {ezRHIResourceFormat::BC4_SNORM, vk::Format(VK_FORMAT_BC4_SNORM_BLOCK)},
  {ezRHIResourceFormat::BC5_UNORM, vk::Format(VK_FORMAT_BC5_UNORM_BLOCK)},
  {ezRHIResourceFormat::BC5_SNORM, vk::Format(VK_FORMAT_BC5_SNORM_BLOCK)},
  {ezRHIResourceFormat::BC6H_UFLOAT, vk::Format(VK_FORMAT_BC6H_UFLOAT_BLOCK)},
  {ezRHIResourceFormat::BC6H_SFLOAT, vk::Format(VK_FORMAT_BC6H_SFLOAT_BLOCK)},
  {ezRHIResourceFormat::BC7_UNORM, vk::Format(VK_FORMAT_BC7_UNORM_BLOCK)},
  {ezRHIResourceFormat::BC7_UNORM_SRGB, vk::Format(VK_FORMAT_BC7_SRGB_BLOCK)},
};

vk::Format VK::ToVKFormat(ezRHIResourceFormat::Enum format)
{
  EZ_ASSERT_ALWAYS(format < ezRHIResourceFormat::ENUM_COUNT, "");
  EZ_ASSERT_ALWAYS(c_FormatMap[uint32_t(format)].m_EngineFormat == format, "");

  return c_FormatMap[uint32_t(format)].m_VKFormat;
}

ezRHIResourceFormat::Enum VK::ToEngineFormat(vk::Format format)
{
  //EZ_ASSERT_ALWAYS(format < ezRHIResourceFormat::ENUM_COUNT, "");
  //EZ_ASSERT_ALWAYS(c_FormatMap[uint32_t(format)].m_VKFormat == format, "");

  //return c_FormatMap[uint32_t(format)].m_EngineFormat;

  return ezRHIResourceFormat::Unknown;
}
