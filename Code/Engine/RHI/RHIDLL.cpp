#include <RHIPCH.h>

#include <RHI/RHIDLL.h>

// Format mapping table. The rows must be in the exactly same order as Format enum members are defined.
static const ezRHIFormatInfo c_FormatInfo[] = {
  //        format                   name             bytes blk        extent         kind               red   green   blue  alpha  depth  stencl signed  srgb
  {ezRHIResourceFormat::Unknown, "Unknown", 0, 0, ezVec3U32(0, 0, 0), ezRHIFormatKind::Integer, false, false, false, false, false, false, false, false},
  {ezRHIResourceFormat::R8_UINT, "R8_UINT", 1, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, true, false, false, false, false, false, false, false},
  {ezRHIResourceFormat::R8_SINT, "R8_SINT", 1, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, true, false, false, false, false, false, true, false},
  {ezRHIResourceFormat::R8_UNORM, "R8_UNORM", 1, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Normalized, true, false, false, false, false, false, false, false},
  {ezRHIResourceFormat::R8_SNORM, "R8_SNORM", 1, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Normalized, true, false, false, false, false, false, false, false},
  {ezRHIResourceFormat::RG8_UINT, "RG8_UINT", 2, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, true, true, false, false, false, false, false, false},
  {ezRHIResourceFormat::RG8_SINT, "RG8_SINT", 2, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, true, true, false, false, false, false, true, false},
  {ezRHIResourceFormat::RG8_UNORM, "RG8_UNORM", 2, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Normalized, true, true, false, false, false, false, false, false},
  {ezRHIResourceFormat::RG8_SNORM, "RG8_SNORM", 2, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Normalized, true, true, false, false, false, false, false, false},
  {ezRHIResourceFormat::R16_UINT, "R16_UINT", 2, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, true, false, false, false, false, false, false, false},
  {ezRHIResourceFormat::R16_SINT, "R16_SINT", 2, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, true, false, false, false, false, false, true, false},
  {ezRHIResourceFormat::R16_UNORM, "R16_UNORM", 2, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Normalized, true, false, false, false, false, false, false, false},
  {ezRHIResourceFormat::R16_SNORM, "R16_SNORM", 2, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Normalized, true, false, false, false, false, false, false, false},
  {ezRHIResourceFormat::R16_FLOAT, "R16_FLOAT", 2, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Float, true, false, false, false, false, false, true, false},
  {ezRHIResourceFormat::BGRA4_UNORM, "BGRA4_UNORM", 2, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Normalized, true, true, true, true, false, false, false, false},
  {ezRHIResourceFormat::B5G6R5_UNORM, "B5G6R5_UNORM", 2, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Normalized, true, true, true, false, false, false, false, false},
  {ezRHIResourceFormat::B5G5R5A1_UNORM, "B5G5R5A1_UNORM", 2, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Normalized, true, true, true, true, false, false, false, false},
  {ezRHIResourceFormat::RGBA8_UINT, "RGBA8_UINT", 4, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, true, true, true, true, false, false, false, false},
  {ezRHIResourceFormat::RGBA8_SINT, "RGBA8_SINT", 4, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, true, true, true, true, false, false, true, false},
  {ezRHIResourceFormat::RGBA8_UNORM, "RGBA8_UNORM", 4, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Normalized, true, true, true, true, false, false, false, false},
  {ezRHIResourceFormat::RGBA8_SNORM, "RGBA8_SNORM", 4, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Normalized, true, true, true, true, false, false, false, false},
  {ezRHIResourceFormat::BGRA8_UNORM, "BGRA8_UNORM", 4, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Normalized, true, true, true, true, false, false, false, false},
  {ezRHIResourceFormat::SRGBA8_UNORM, "SRGBA8_UNORM", 4, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Normalized, true, true, true, true, false, false, false, true},
  {ezRHIResourceFormat::SBGRA8_UNORM, "SBGRA8_UNORM", 4, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Normalized, true, true, true, true, false, false, false, false},
  {ezRHIResourceFormat::R10G10B10A2_UNORM, "R10G10B10A2_UNORM", 4, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Normalized, true, true, true, true, false, false, false, false},
  {ezRHIResourceFormat::R11G11B10_FLOAT, "R11G11B10_FLOAT", 4, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Float, true, true, true, false, false, false, false, false},
  {ezRHIResourceFormat::RG16_UINT, "RG16_UINT", 4, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, true, true, false, false, false, false, false, false},
  {ezRHIResourceFormat::RG16_SINT, "RG16_SINT", 4, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, true, true, false, false, false, false, true, false},
  {ezRHIResourceFormat::RG16_UNORM, "RG16_UNORM", 4, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Normalized, true, true, false, false, false, false, false, false},
  {ezRHIResourceFormat::RG16_SNORM, "RG16_SNORM", 4, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Normalized, true, true, false, false, false, false, false, false},
  {ezRHIResourceFormat::RG16_FLOAT, "RG16_FLOAT", 4, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Float, true, true, false, false, false, false, true, false},
  {ezRHIResourceFormat::R32_UINT, "R32_UINT", 4, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, true, false, false, false, false, false, false, false},
  {ezRHIResourceFormat::R32_SINT, "R32_SINT", 4, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, true, false, false, false, false, false, true, false},
  {ezRHIResourceFormat::R32_FLOAT, "R32_FLOAT", 4, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Float, true, false, false, false, false, false, true, false},
  {ezRHIResourceFormat::RGBA16_UINT, "RGBA16_UINT", 8, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, true, true, true, true, false, false, false, false},
  {ezRHIResourceFormat::RGBA16_SINT, "RGBA16_SINT", 8, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, true, true, true, true, false, false, true, false},
  {ezRHIResourceFormat::RGBA16_FLOAT, "RGBA16_FLOAT", 8, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Float, true, true, true, true, false, false, true, false},
  {ezRHIResourceFormat::RGBA16_UNORM, "RGBA16_UNORM", 8, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Normalized, true, true, true, true, false, false, false, false},
  {ezRHIResourceFormat::RGBA16_SNORM, "RGBA16_SNORM", 8, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Normalized, true, true, true, true, false, false, false, false},
  {ezRHIResourceFormat::RG32_UINT, "RG32_UINT", 8, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, true, true, false, false, false, false, false, false},
  {ezRHIResourceFormat::RG32_SINT, "RG32_SINT", 8, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, true, true, false, false, false, false, true, false},
  {ezRHIResourceFormat::RG32_FLOAT, "RG32_FLOAT", 8, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Float, true, true, false, false, false, false, true, false},
  {ezRHIResourceFormat::RGB32_UINT, "RGB32_UINT", 12, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, true, true, true, false, false, false, false, false},
  {ezRHIResourceFormat::RGB32_SINT, "RGB32_SINT", 12, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, true, true, true, false, false, false, true, false},
  {ezRHIResourceFormat::RGB32_FLOAT, "RGB32_FLOAT", 12, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Float, true, true, true, false, false, false, true, false},
  {ezRHIResourceFormat::RGBA32_UINT, "RGBA32_UINT", 16, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, true, true, true, true, false, false, false, false},
  {ezRHIResourceFormat::RGBA32_SINT, "RGBA32_SINT", 16, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, true, true, true, true, false, false, true, false},
  {ezRHIResourceFormat::RGBA32_FLOAT, "RGBA32_FLOAT", 16, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Float, true, true, true, true, false, false, true, false},
  {ezRHIResourceFormat::D16, "D16", 2, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::DepthStencil, false, false, false, false, true, false, false, false},
  {ezRHIResourceFormat::D24S8, "D24S8", 4, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::DepthStencil, false, false, false, false, true, true, false, false},
  {ezRHIResourceFormat::X24G8_UINT, "X24G8_UINT", 4, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, false, false, false, false, false, true, false, false},
  {ezRHIResourceFormat::D32, "D32", 4, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::DepthStencil, false, false, false, false, true, false, false, false},
  {ezRHIResourceFormat::D32S8, "D32S8", 8, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::DepthStencil, false, false, false, false, true, true, false, false},
  {ezRHIResourceFormat::X32G8_UINT, "X32G8_UINT", 8, 1, ezVec3U32(1, 1, 1), ezRHIFormatKind::Integer, false, false, false, false, false, true, false, false},
  {ezRHIResourceFormat::BC1_UNORM, "BC1_UNORM", 8, 4, ezVec3U32(4, 4, 1), ezRHIFormatKind::Normalized, true, true, true, true, false, false, false, false},
  {ezRHIResourceFormat::BC1_UNORM_SRGB, "BC1_UNORM_SRGB", 8, 4, ezVec3U32(4, 4, 1), ezRHIFormatKind::Normalized, true, true, true, true, false, false, false, true},
  {ezRHIResourceFormat::BC2_UNORM, "BC2_UNORM", 16, 4, ezVec3U32(4, 4, 1), ezRHIFormatKind::Normalized, true, true, true, true, false, false, false, false},
  {ezRHIResourceFormat::BC2_UNORM_SRGB, "BC2_UNORM_SRGB", 16, 4, ezVec3U32(4, 4, 1), ezRHIFormatKind::Normalized, true, true, true, true, false, false, false, true},
  {ezRHIResourceFormat::BC3_UNORM, "BC3_UNORM", 16, 4, ezVec3U32(4, 4, 1), ezRHIFormatKind::Normalized, true, true, true, true, false, false, false, false},
  {ezRHIResourceFormat::BC3_UNORM_SRGB, "BC3_UNORM_SRGB", 16, 4, ezVec3U32(4, 4, 1), ezRHIFormatKind::Normalized, true, true, true, true, false, false, false, true},
  {ezRHIResourceFormat::BC4_UNORM, "BC4_UNORM", 8, 4, ezVec3U32(4, 4, 1), ezRHIFormatKind::Normalized, true, false, false, false, false, false, false, false},
  {ezRHIResourceFormat::BC4_SNORM, "BC4_SNORM", 8, 4, ezVec3U32(4, 4, 1), ezRHIFormatKind::Normalized, true, false, false, false, false, false, false, false},
  {ezRHIResourceFormat::BC5_UNORM, "BC5_UNORM", 16, 4, ezVec3U32(4, 4, 1), ezRHIFormatKind::Normalized, true, true, false, false, false, false, false, false},
  {ezRHIResourceFormat::BC5_SNORM, "BC5_SNORM", 16, 4, ezVec3U32(4, 4, 1), ezRHIFormatKind::Normalized, true, true, false, false, false, false, false, false},
  {ezRHIResourceFormat::BC6H_UFLOAT, "BC6H_UFLOAT", 16, 4, ezVec3U32(4, 4, 1), ezRHIFormatKind::Float, true, true, true, false, false, false, false, false},
  {ezRHIResourceFormat::BC6H_SFLOAT, "BC6H_SFLOAT", 16, 4, ezVec3U32(4, 4, 1), ezRHIFormatKind::Float, true, true, true, false, false, false, true, false},
  {ezRHIResourceFormat::BC7_UNORM, "BC7_UNORM", 16, 4, ezVec3U32(4, 4, 1), ezRHIFormatKind::Normalized, true, true, true, true, false, false, false, false},
  {ezRHIResourceFormat::BC7_UNORM_SRGB, "BC7_UNORM_SRGB", 16, 4, ezVec3U32(4, 4, 1), ezRHIFormatKind::Normalized, true, true, true, true, false, false, false, true},
};

const ezRHIFormatInfo& GetFormatInfo(ezRHIResourceFormat::Enum format)
{
  EZ_CHECK_AT_COMPILETIME_MSG(sizeof(c_FormatInfo) / sizeof(ezRHIFormatInfo) == size_t(ezRHIResourceFormat::ENUM_COUNT),
    "The format info table doesn't have the right number of elements");

  if (ezUInt32(format) >= ezUInt32(ezRHIResourceFormat::ENUM_COUNT))
    return c_FormatInfo[0]; // Unknown

  const ezRHIFormatInfo& info = c_FormatInfo[ezUInt32(format)];
  EZ_ASSERT_ALWAYS(info.m_Format == format, "");
  return info;
}
