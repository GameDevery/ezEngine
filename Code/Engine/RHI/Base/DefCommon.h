#pragma once

#include <RHI/RHIDLL.h>

class RHIObject;
class Device;
class Swapchain;
class Buffer;
class GlobalBarrier;
class TextureBarrier;
class Texture;
class Sampler;
class Shader;
class InputAssembler;
class RenderPass;
class Framebuffer;
class DescriptorSetLayout;
class PipelineLayout;
class PipelineState;
class DescriptorSet;
class CommandBuffer;
class Queue;
class Window;
class Context;

using TextureBarrierList = ezDynamicArray<TextureBarrier*>;
using BufferDataList = ezDynamicArray<const ezUInt8*>;
using CommandBufferList = ezDynamicArray<CommandBuffer*>;


constexpr ezUInt32 MAX_ATTACHMENTS = 4U;
constexpr ezUInt32 INVALID_BINDING = ~0U;
constexpr ezUInt32 SUBPASS_EXTERNAL = ~0U;

using BufferList = ezDynamicArray<Buffer*>;
using TextureList = ezDynamicArray<Texture*>;
using SamplerList = ezDynamicArray<Sampler*>;
using DescriptorSetLayoutList = ezDynamicArray<DescriptorSetLayout*>;

enum class ObjectType : ezUInt32
{
  UNKNOWN,
  SWAPCHAIN,
  BUFFER,
  TEXTURE,
  RENDER_PASS,
  FRAMEBUFFER,
  SAMPLER,
  SHADER,
  DESCRIPTOR_SET_LAYOUT,
  PIPELINE_LAYOUT,
  PIPELINE_STATE,
  DESCRIPTOR_SET,
  INPUT_ASSEMBLER,
  COMMAND_BUFFER,
  QUEUE,
  GLOBAL_BARRIER,
  TEXTURE_BARRIER,
  BUFFER_BARRIER,
  COUNT,
};
EZ_ENUM_CONVERSION_OPERATOR(ObjectType);

enum class Status : ezUInt32
{
  UNREADY,
  FAILED,
  SUCCESS,
};
EZ_ENUM_CONVERSION_OPERATOR(Status);

enum class API : ezUInt32
{
  UNKNOWN,
  GLES2,
  GLES3,
  METAL,
  VULKAN,
  WEBGL,
  WEBGL2,
  WEBGPU,
};
EZ_ENUM_CONVERSION_OPERATOR(API);

enum class SurfaceTransform : ezUInt32
{
  IDENTITY,
  ROTATE_90,
  ROTATE_180,
  ROTATE_270,
};
EZ_ENUM_CONVERSION_OPERATOR(SurfaceTransform);

enum class Feature : ezUInt32
{
  COLOR_FLOAT,
  COLOR_HALF_FLOAT,
  TEXTURE_FLOAT,
  TEXTURE_HALF_FLOAT,
  TEXTURE_FLOAT_LINEAR,
  TEXTURE_HALF_FLOAT_LINEAR,
  FORMAT_R11G11B10F,
  FORMAT_SRGB,
  FORMAT_ETC1,
  FORMAT_ETC2,
  FORMAT_DXT,
  FORMAT_PVRTC,
  FORMAT_ASTC,
  FORMAT_RGB8,
  ELEMENT_INDEX_UINT,
  INSTANCED_ARRAYS,
  MULTIPLE_RENDER_TARGETS,
  BLEND_MINMAX,
  COMPUTE_SHADER,
  // This flag indicates whether the device can benefit from subpass-style usages.
  // Specifically, this only differs on the GLES backends: the Framebuffer Fetch
  // extension is used to simulate input attachments, so the flag is not set when
  // the extension is not supported, and you should switch to the fallback branch
  // (without the extension requirement) in GLSL shader sources accordingly.
  // Everything else can remain the same.
  //
  // Another caveat when using the Framebuffer Fetch extensions in shaders is that
  // for subpasses with exactly 4 inout attachments the output is automatically set
  // to the last attachment (taking advantage of 'inout' property), and a separate
  // blit operation (if needed) will be added for you afterwards to transfer the
  // rendering result to the corrent subpass output texture. This is to ameliorate
  // the max number of attachment limit(4) situation for many devices, and shader
  // sources inside this kind of subpass must match this behavior.
  INPUT_ATTACHMENT_BENEFIT,
  COUNT,
};
EZ_ENUM_CONVERSION_OPERATOR(Feature);

enum class Format : ezUInt32
{

  UNKNOWN,

  A8,
  L8,
  LA8,

  R8,
  R8SN,
  R8UI,
  R8I,
  R16F,
  R16UI,
  R16I,
  R32F,
  R32UI,
  R32I,

  RG8,
  RG8SN,
  RG8UI,
  RG8I,
  RG16F,
  RG16UI,
  RG16I,
  RG32F,
  RG32UI,
  RG32I,

  RGB8,
  SRGB8,
  RGB8SN,
  RGB8UI,
  RGB8I,
  RGB16F,
  RGB16UI,
  RGB16I,
  RGB32F,
  RGB32UI,
  RGB32I,

  RGBA8,
  BGRA8,
  SRGB8_A8,
  RGBA8SN,
  RGBA8UI,
  RGBA8I,
  RGBA16F,
  RGBA16UI,
  RGBA16I,
  RGBA32F,
  RGBA32UI,
  RGBA32I,

  // Special Format
  R5G6B5,
  R11G11B10F,
  RGB5A1,
  RGBA4,
  RGB10A2,
  RGB10A2UI,
  RGB9E5,

  // Depth-Stencil Format
  DEPTH,
  DEPTH_STENCIL,

  // Compressed Format

  // Block Compression Format, DDS (DirectDraw Surface)
  // DXT1: 3 channels (5:6:5), 1/8 origianl size, with 0 or 1 bit of alpha
  BC1,
  BC1_ALPHA,
  BC1_SRGB,
  BC1_SRGB_ALPHA,
  // DXT3: 4 channels (5:6:5), 1/4 origianl size, with 4 bits of alpha
  BC2,
  BC2_SRGB,
  // DXT5: 4 channels (5:6:5), 1/4 origianl size, with 8 bits of alpha
  BC3,
  BC3_SRGB,
  // 1 channel (8), 1/4 origianl size
  BC4,
  BC4_SNORM,
  // 2 channels (8:8), 1/2 origianl size
  BC5,
  BC5_SNORM,
  // 3 channels (16:16:16), half-floating point, 1/6 origianl size
  // UF16: unsigned float, 5 exponent bits + 11 mantissa bits
  // SF16: signed float, 1 signed bit + 5 exponent bits + 10 mantissa bits
  BC6H_UF16,
  BC6H_SF16,
  // 4 channels (4~7 bits per channel) with 0 to 8 bits of alpha, 1/3 original size
  BC7,
  BC7_SRGB,

  // Ericsson Texture Compression Format
  ETC_RGB8,
  ETC2_RGB8,
  ETC2_SRGB8,
  ETC2_RGB8_A1,
  ETC2_SRGB8_A1,
  ETC2_RGBA8,
  ETC2_SRGB8_A8,
  EAC_R11,
  EAC_R11SN,
  EAC_RG11,
  EAC_RG11SN,

  // PVRTC (PowerVR)
  PVRTC_RGB2,
  PVRTC_RGBA2,
  PVRTC_RGB4,
  PVRTC_RGBA4,
  PVRTC2_2BPP,
  PVRTC2_4BPP,

  // ASTC (Adaptive Scalable Texture Compression)
  ASTC_RGBA_4X4,
  ASTC_RGBA_5X4,
  ASTC_RGBA_5X5,
  ASTC_RGBA_6X5,
  ASTC_RGBA_6X6,
  ASTC_RGBA_8X5,
  ASTC_RGBA_8X6,
  ASTC_RGBA_8X8,
  ASTC_RGBA_10X5,
  ASTC_RGBA_10X6,
  ASTC_RGBA_10X8,
  ASTC_RGBA_10X10,
  ASTC_RGBA_12X10,
  ASTC_RGBA_12X12,

  // ASTC (Adaptive Scalable Texture Compression) SRGB
  ASTC_SRGBA_4X4,
  ASTC_SRGBA_5X4,
  ASTC_SRGBA_5X5,
  ASTC_SRGBA_6X5,
  ASTC_SRGBA_6X6,
  ASTC_SRGBA_8X5,
  ASTC_SRGBA_8X6,
  ASTC_SRGBA_8X8,
  ASTC_SRGBA_10X5,
  ASTC_SRGBA_10X6,
  ASTC_SRGBA_10X8,
  ASTC_SRGBA_10X10,
  ASTC_SRGBA_12X10,
  ASTC_SRGBA_12X12,

  // Total count
  COUNT,
};
EZ_ENUM_CONVERSION_OPERATOR(Format);

enum class FormatType : ezUInt32
{
  NONE,
  UNORM,
  SNORM,
  UINT,
  INT,
  UFLOAT,
  FLOAT,
};
EZ_ENUM_CONVERSION_OPERATOR(FormatType);

enum class Type : ezUInt32
{
  UNKNOWN,
  BOOL,
  BOOL2,
  BOOL3,
  BOOL4,
  INT,
  INT2,
  INT3,
  INT4,
  UINT,
  UINT2,
  UINT3,
  UINT4,
  FLOAT,
  FLOAT2,
  FLOAT3,
  FLOAT4,
  MAT2,
  MAT2X3,
  MAT2X4,
  MAT3X2,
  MAT3,
  MAT3X4,
  MAT4X2,
  MAT4X3,
  MAT4,
  // combined image samplers
  SAMPLER1D,
  SAMPLER1D_ARRAY,
  SAMPLER2D,
  SAMPLER2D_ARRAY,
  SAMPLER3D,
  SAMPLER_CUBE,
  // sampler
  SAMPLER,
  // sampled textures
  TEXTURE1D,
  TEXTURE1D_ARRAY,
  TEXTURE2D,
  TEXTURE2D_ARRAY,
  TEXTURE3D,
  TEXTURE_CUBE,
  // storage images
  IMAGE1D,
  IMAGE1D_ARRAY,
  IMAGE2D,
  IMAGE2D_ARRAY,
  IMAGE3D,
  IMAGE_CUBE,
  // input attachment
  SUBPASS_INPUT,
  COUNT,
};
EZ_ENUM_CONVERSION_OPERATOR(Type);

bool IsCombinedImageSampler(Type type);
bool IsSampledImage(Type type);
bool IsStorageImage(Type type);

enum class BufferUsageBit : ezUInt32
{
  NONE = 0,
  TRANSFER_SRC = 0x1,
  TRANSFER_DST = 0x2,
  INDEX = 0x4,
  VERTEX = 0x8,
  UNIFORM = 0x10,
  STORAGE = 0x20,
  INDIRECT = 0x40,
};
using BufferUsage = BufferUsageBit;
EZ_ENUM_BITWISE_OPERATORS(BufferUsageBit);

enum class BufferFlagBit : ezUInt32
{
  NONE = 0,
};
using BufferFlags = BufferFlagBit;
EZ_ENUM_BITWISE_OPERATORS(BufferFlagBit);

enum class MemoryAccessBit : ezUInt32
{
  NONE = 0,
  READ_ONLY = 0x1,
  WRITE_ONLY = 0x2,
  READ_WRITE = READ_ONLY | WRITE_ONLY,
};
using MemoryAccess = MemoryAccessBit;
EZ_ENUM_BITWISE_OPERATORS(MemoryAccessBit);

enum class MemoryUsageBit : ezUInt32
{
  NONE = 0,
  DEVICE = 0x1, // for rarely-updated resources, use MemoryUsageBit::DEVICE
  HOST = 0x2,   // for frequently-updated resources, use MemoryUsageBit::DEVICE | MemoryUsageBit::HOST
};
using MemoryUsage = MemoryUsageBit;
EZ_ENUM_BITWISE_OPERATORS(MemoryUsageBit);

enum class TextureType : ezUInt32
{
  TEX1D,
  TEX2D,
  TEX3D,
  CUBE,
  TEX1D_ARRAY,
  TEX2D_ARRAY,
};
EZ_ENUM_CONVERSION_OPERATOR(TextureType);

enum class TextureUsageBit : ezUInt32
{
  NONE = 0,
  TRANSFER_SRC = 0x1,
  TRANSFER_DST = 0x2,
  SAMPLED = 0x4,
  STORAGE = 0x8,
  COLOR_ATTACHMENT = 0x10,
  DEPTH_STENCIL_ATTACHMENT = 0x20,
  INPUT_ATTACHMENT = 0x40,
};
using TextureUsage = TextureUsageBit;
EZ_ENUM_BITWISE_OPERATORS(TextureUsageBit);

enum class TextureFlagBit : ezUInt32
{
  NONE = 0,
  GEN_MIPMAP = 0x1,     // Generate mipmaps using bilinear filter
  GENERAL_LAYOUT = 0x2, // For inout framebuffer attachments
};
using TextureFlags = TextureFlagBit;
EZ_ENUM_BITWISE_OPERATORS(TextureFlagBit);

enum class SampleCount : ezUInt32
{
  ONE,                  // Single sample
  MULTIPLE_PERFORMANCE, // Multiple samples prioritizing performance over quality
  MULTIPLE_BALANCE,     // Multiple samples leveraging both quality and performance
  MULTIPLE_QUALITY,     // Multiple samples prioritizing quality over performance
};
EZ_ENUM_CONVERSION_OPERATOR(SampleCount);

enum class VsyncMode : ezUInt32
{
  // The application does not synchronizes with the vertical sync.
  // If application renders faster than the display refreshes, frames are wasted and tearing may be observed.
  // FPS is uncapped. Maximum power consumption. If unsupported, "ON" value will be used instead. Minimum latency.
  OFF,
  // The application is always synchronized with the vertical sync. Tearing does not happen.
  // FPS is capped to the display's refresh rate. For fast applications, battery life is improved. Always supported.
  ON,
  // The application synchronizes with the vertical sync, but only if the application rendering speed is greater than refresh rate.
  // Compared to OFF, there is no tearing. Compared to ON, the FPS will be improved for "slower" applications.
  // If unsupported, "ON" value will be used instead. Recommended for most applications. Default if supported.
  RELAXED,
  // The presentation engine will always use the latest fully rendered image.
  // Compared to OFF, no tearing will be observed.
  // Compared to ON, battery power will be worse, especially for faster applications.
  // If unsupported,  "OFF" will be attempted next.
  MAILBOX,
  // The application is capped to using half the vertical sync time.
  // FPS artificially capped to Half the display speed (usually 30fps) to maintain battery.
  // Best possible battery savings. Worst possible performance.
  // Recommended for specific applications where battery saving is critical.
  HALF,
};
EZ_ENUM_CONVERSION_OPERATOR(VsyncMode);

enum class Filter : ezUInt32
{
  NONE,
  POINT,
  LINEAR,
  ANISOTROPIC,
};
EZ_ENUM_CONVERSION_OPERATOR(Filter);

enum class Address : ezUInt32
{
  WRAP,
  MIRROR,
  CLAMP,
  BORDER,
};
EZ_ENUM_CONVERSION_OPERATOR(Address);

enum class ComparisonFunc : ezUInt32
{
  NEVER,
  LESS,
  EQUAL,
  LESS_EQUAL,
  GREATER,
  NOT_EQUAL,
  GREATER_EQUAL,
  ALWAYS,
};
EZ_ENUM_CONVERSION_OPERATOR(ComparisonFunc);

enum class StencilOp : ezUInt32
{
  ZERO,
  KEEP,
  REPLACE,
  INCR,
  DECR,
  INVERT,
  INCR_WRAP,
  DECR_WRAP,
};
EZ_ENUM_CONVERSION_OPERATOR(StencilOp);

enum class BlendFactor : ezUInt32
{
  ZERO,
  ONE,
  SRC_ALPHA,
  DST_ALPHA,
  ONE_MINUS_SRC_ALPHA,
  ONE_MINUS_DST_ALPHA,
  SRC_COLOR,
  DST_COLOR,
  ONE_MINUS_SRC_COLOR,
  ONE_MINUS_DST_COLOR,
  SRC_ALPHA_SATURATE,
  CONSTANT_COLOR,
  ONE_MINUS_CONSTANT_COLOR,
  CONSTANT_ALPHA,
  ONE_MINUS_CONSTANT_ALPHA,
};
EZ_ENUM_CONVERSION_OPERATOR(BlendFactor);

enum class BlendOp : ezUInt32
{
  ADD,
  SUB,
  REV_SUB,
  MIN,
  MAX,
};
EZ_ENUM_CONVERSION_OPERATOR(BlendOp);

enum class ColorMask : ezUInt32
{
  NONE = 0x0,
  R = 0x1,
  G = 0x2,
  B = 0x4,
  A = 0x8,
  ALL = R | G | B | A,
};
EZ_ENUM_BITWISE_OPERATORS(ColorMask);

enum class ShaderStageFlagBit : ezUInt32
{
  NONE = 0x0,
  VERTEX = 0x1,
  CONTROL = 0x2,
  EVALUATION = 0x4,
  GEOMETRY = 0x8,
  FRAGMENT = 0x10,
  COMPUTE = 0x20,
  ALL = 0x3f,
};
using ShaderStageFlags = ShaderStageFlagBit;
EZ_ENUM_BITWISE_OPERATORS(ShaderStageFlagBit);

enum class LoadOp : ezUInt32
{
  LOAD,    // Load the contents from the fbo from previous
  CLEAR,   // Clear the fbo
  DISCARD, // Ignore writing to the fbo and keep old data
};
EZ_ENUM_CONVERSION_OPERATOR(LoadOp);

enum class StoreOp : ezUInt32
{
  STORE,   // Write the source to the destination
  DISCARD, // Don't write the source to the destination
};
EZ_ENUM_CONVERSION_OPERATOR(StoreOp);

enum class AccessType : ezUInt32
{
  NONE,

  // Read access
  INDIRECT_BUFFER,                                     // Read as an indirect buffer for drawing or dispatch
  INDEX_BUFFER,                                        // Read as an index buffer for drawing
  VERTEX_BUFFER,                                       // Read as a vertex buffer for drawing
  VERTEX_SHADER_READ_UNIFORM_BUFFER,                   // Read as a uniform buffer in a vertex shader
  VERTEX_SHADER_READ_TEXTURE,                          // Read as a sampled image/uniform texel buffer in a vertex shader
  VERTEX_SHADER_READ_OTHER,                            // Read as any other resource in a vertex shader
  FRAGMENT_SHADER_READ_UNIFORM_BUFFER,                 // Read as a uniform buffer in a fragment shader
  FRAGMENT_SHADER_READ_TEXTURE,                        // Read as a sampled image/uniform texel buffer in a fragment shader
  FRAGMENT_SHADER_READ_COLOR_INPUT_ATTACHMENT,         // Read as an input attachment with a color format in a fragment shader
  FRAGMENT_SHADER_READ_DEPTH_STENCIL_INPUT_ATTACHMENT, // Read as an input attachment with a depth/stencil format in a fragment shader
  FRAGMENT_SHADER_READ_OTHER,                          // Read as any other resource in a fragment shader
  COLOR_ATTACHMENT_READ,                               // Read by standard blending/logic operations or subpass load operations
  DEPTH_STENCIL_ATTACHMENT_READ,                       // Read by depth/stencil tests or subpass load operations
  COMPUTE_SHADER_READ_UNIFORM_BUFFER,                  // Read as a uniform buffer in a compute shader
  COMPUTE_SHADER_READ_TEXTURE,                         // Read as a sampled image/uniform texel buffer in a compute shader
  COMPUTE_SHADER_READ_OTHER,                           // Read as any other resource in a compute shader
  TRANSFER_READ,                                       // Read as the source of a transfer operation
  HOST_READ,                                           // Read on the host
  PRESENT,                                             // Read by the presentation engine

  // Write access
  VERTEX_SHADER_WRITE,            // Written as any resource in a vertex shader
  FRAGMENT_SHADER_WRITE,          // Written as any resource in a fragment shader
  COLOR_ATTACHMENT_WRITE,         // Written as a color attachment during rendering, or via a subpass store op
  DEPTH_STENCIL_ATTACHMENT_WRITE, // Written as a depth/stencil attachment during rendering, or via a subpass store op
  COMPUTE_SHADER_WRITE,           // Written as any resource in a compute shader
  TRANSFER_WRITE,                 // Written as the destination of a transfer operation
  HOST_PREINITIALIZED,            // Data pre-filled by host before device access starts
  HOST_WRITE,                     // Written on the host
};
EZ_ENUM_CONVERSION_OPERATOR(AccessType);

EZ_DEFINE_AS_POD_TYPE(AccessType);

using AccessTypeList = ezDynamicArray<AccessType>;

enum class ResolveMode : ezUInt32
{
  NONE,
  SAMPLE_ZERO,
  AVERAGE,
  MIN,
  MAX,
};
EZ_ENUM_CONVERSION_OPERATOR(ResolveMode);

enum class PipelineBindPoint : ezUInt32
{
  GRAPHICS,
  COMPUTE,
  RAY_TRACING,
};
EZ_ENUM_CONVERSION_OPERATOR(PipelineBindPoint);

enum class PrimitiveMode : ezUInt32
{
  POINT_LIST,
  LINE_LIST,
  LINE_STRIP,
  LINE_LOOP,
  LINE_LIST_ADJACENCY,
  LINE_STRIP_ADJACENCY,
  ISO_LINE_LIST,
  // raycast detectable:
  TRIANGLE_LIST,
  TRIANGLE_STRIP,
  TRIANGLE_FAN,
  TRIANGLE_LIST_ADJACENCY,
  TRIANGLE_STRIP_ADJACENCY,
  TRIANGLE_PATCH_ADJACENCY,
  QUAD_PATCH_LIST,
};
EZ_ENUM_CONVERSION_OPERATOR(PrimitiveMode);

enum class PolygonMode : ezUInt32
{
  FILL,
  POINT,
  LINE,
};
EZ_ENUM_CONVERSION_OPERATOR(PolygonMode);

enum class ShadeModel : ezUInt32
{
  GOURAND,
  FLAT,
};
EZ_ENUM_CONVERSION_OPERATOR(ShadeModel);

enum class CullMode : ezUInt32
{
  NONE,
  FRONT,
  BACK,
};
EZ_ENUM_CONVERSION_OPERATOR(CullMode);

enum class DynamicStateFlagBit : ezUInt32
{
  NONE = 0x0,
  LINE_WIDTH = 0x1,
  DEPTH_BIAS = 0x2,
  BLEND_CONSTANTS = 0x4,
  DEPTH_BOUNDS = 0x8,
  STENCIL_WRITE_MASK = 0x10,
  STENCIL_COMPARE_MASK = 0x20,
};
using DynamicStateFlags = DynamicStateFlagBit;
EZ_ENUM_BITWISE_OPERATORS(DynamicStateFlagBit);

using DynamicStateList = ezDynamicArray<DynamicStateFlagBit>;

enum class StencilFace : ezUInt32
{
  FRONT = 0x1,
  BACK = 0x2,
  ALL = 0x3,
};
EZ_ENUM_BITWISE_OPERATORS(StencilFace);

enum class DescriptorType : ezUInt32
{
  UNKNOWN = 0,
  UNIFORM_BUFFER = 0x1,
  DYNAMIC_UNIFORM_BUFFER = 0x2,
  STORAGE_BUFFER = 0x4,
  DYNAMIC_STORAGE_BUFFER = 0x8,
  SAMPLER_TEXTURE = 0x10,
  SAMPLER = 0x20,
  TEXTURE = 0x40,
  STORAGE_IMAGE = 0x80,
  INPUT_ATTACHMENT = 0x100,
};
EZ_ENUM_BITWISE_OPERATORS(DescriptorType);

enum class QueueType : ezUInt32
{
  GRAPHICS,
  COMPUTE,
  TRANSFER,
};
EZ_ENUM_CONVERSION_OPERATOR(QueueType);

enum class CommandBufferType : ezUInt32
{
  PRIMARY,
  SECONDARY,
};
EZ_ENUM_CONVERSION_OPERATOR(CommandBufferType);

enum class ClearFlagBit : ezUInt32
{
  NONE = 0,
  COLOR = 0x1,
  DEPTH = 0x2,
  STENCIL = 0x4,
  DEPTH_STENCIL = DEPTH | STENCIL,
  ALL = COLOR | DEPTH | STENCIL,
};
using ClearFlags = ClearFlagBit;
EZ_ENUM_BITWISE_OPERATORS(ClearFlagBit);

struct Size
{
  ezUInt32 x{0U};
  ezUInt32 y{0U};
  ezUInt32 z{0U};
};

struct DeviceCaps
{
  ezUInt32 maxVertexAttributes{0U};
  ezUInt32 maxVertexUniformVectors{0U};
  ezUInt32 maxFragmentUniformVectors{0U};
  ezUInt32 maxTextureUnits{0U};
  ezUInt32 maxImageUnits{0U};
  ezUInt32 maxVertexTextureUnits{0U};
  ezUInt32 maxColorRenderTargets{0U};
  ezUInt32 maxShaderStorageBufferBindings{0U};
  ezUInt32 maxShaderStorageBlockSize{0U};
  ezUInt32 maxUniformBufferBindings{0U};
  ezUInt32 maxUniformBlockSize{0U};
  ezUInt32 maxTextureSize{0U};
  ezUInt32 maxCubeMapTextureSize{0U};
  ezUInt32 uboOffsetAlignment{1U};

  ezUInt32 maxComputeSharedMemorySize{0U};
  ezUInt32 maxComputeWorkGroupInvocations{0U};
  Size maxComputeWorkGroupSize;
  Size maxComputeWorkGroupCount;

  float clipSpaceMinZ{-1.F};
  float screenSpaceSignY{1.F};
  float clipSpaceSignY{1.F};
};

struct Offset
{
  ezInt32 x{0};
  ezInt32 y{0};
  ezInt32 z{0};
};

struct Rect
{
  ezInt32 x{0};
  ezInt32 y{0};
  ezUInt32 width{0U};
  ezUInt32 height{0U};

  bool operator==(const Rect& rs) const
  {
    return x == rs.x &&
           y == rs.y &&
           width == rs.width &&
           height == rs.height;
  }

  bool operator!=(const Rect& rs) const
  {
    return !(*this == rs);
  }
};

struct Extent
{
  ezUInt32 width{0U};
  ezUInt32 height{0U};
  ezUInt32 depth{1U};
};

struct TextureSubresLayers
{
  ezUInt32 mipLevel{0U};
  ezUInt32 baseArrayLayer{0U};
  ezUInt32 layerCount{1U};
};

struct TextureSubresRange
{
  ezUInt32 baseMipLevel{0U};
  ezUInt32 levelCount{1U};
  ezUInt32 baseArrayLayer{0U};
  ezUInt32 layerCount{1U};
};

struct TextureCopy
{
  TextureSubresLayers srcSubres;
  Offset srcOffset;
  TextureSubresLayers dstSubres;
  Offset dstOffset;
  Extent extent;
};

struct TextureBlit
{
  TextureSubresLayers srcSubres;
  Offset srcOffset;
  Extent srcExtent;
  TextureSubresLayers dstSubres;
  Offset dstOffset;
  Extent dstExtent;
};
using TextureBlitList = ezDynamicArray<TextureBlit>;

struct BufferTextureCopy
{
  ezUInt32 buffStride{0U};
  ezUInt32 buffTexHeight{0U};
  Offset texOffset;
  Extent texExtent;
  TextureSubresLayers texSubres;
};
using BufferTextureCopyList = ezDynamicArray<BufferTextureCopy>;

struct Viewport
{
  ezInt32 left{0};
  ezInt32 top{0};
  ezUInt32 width{0U};
  ezUInt32 height{0U};
  float minDepth{0.F};
  float maxDepth{1.F};

  bool operator==(const Viewport& rs) const
  {
    return (left == rs.left &&
            top == rs.top &&
            width == rs.width &&
            height == rs.height &&
            ezMath::IsEqual(minDepth, rs.minDepth, ezMath::SmallEpsilon<float>()) &&
            ezMath::IsEqual(maxDepth, maxDepth, ezMath::SmallEpsilon<float>()));
  }

  bool operator!=(const Viewport& rs) const
  {
    return !(*this == rs);
  }
};

struct Color
{
  float x{0.F};
  float y{0.F};
  float z{0.F};
  float w{0.F};

  bool operator==(const Color& rhs) const
  {
    return (ezMath::IsEqual(x, rhs.x, ezMath::SmallEpsilon<float>()) &&
            ezMath::IsEqual(y, rhs.y, ezMath::SmallEpsilon<float>()) &&
            ezMath::IsEqual(z, rhs.z, ezMath::SmallEpsilon<float>()) &&
            ezMath::IsEqual(w, rhs.w, ezMath::SmallEpsilon<float>()));
  }
};

//using Color = ezColor;
using ColorList = ezDynamicArray<Color>;

/**
 * For non-vulkan backends, to maintain compatibility and maximize
 * descriptor cache-locality, descriptor-set-based binding numbers need
 * to be mapped to backend-specific bindings based on maximum limit
 * of available descriptor slots in each set.
 *
 * The GFX layer assumes the binding numbers for each descriptor type inside each set
 * are guaranteed to be consecutive, so the mapping procedure is reduced
 * to a simple shifting operation. This data structure specifies the
 * offsets for each descriptor type in each set.
 */
struct BindingMappingInfo
{
  ezDynamicArray<ezInt32> bufferOffsets;
  ezDynamicArray<ezInt32> samplerOffsets;
  ezUInt32 flexibleSet{0U};
};

struct SwapchainInfo
{
  void* windowHandle{nullptr}; // @ts-overrides { type: 'HTMLCanvasElement' }
  VsyncMode vsyncMode{VsyncMode::ON};

  ezUInt32 width{0U};
  ezUInt32 height{0U};
};

struct DeviceInfo
{
  BindingMappingInfo bindingMappingInfo;
};

struct BufferInfo
{
  BufferUsage usage{BufferUsageBit::NONE};
  MemoryUsage memUsage{MemoryUsageBit::NONE};
  ezUInt32 size{0U};
  ezUInt32 stride{0U}; // in bytes
  BufferFlags flags{BufferFlagBit::NONE};
};

struct BufferViewInfo
{
  Buffer* buffer{nullptr};
  ezUInt32 offset{0U};
  ezUInt32 range{0U};
};

struct DrawInfo
{
  ezUInt32 vertexCount{0U};
  ezUInt32 firstVertex{0U};
  ezUInt32 indexCount{0U};
  ezUInt32 firstIndex{0U};
  ezInt32 vertexOffset{0};
  ezUInt32 instanceCount{0U};
  ezUInt32 firstInstance{0U};
};

using DrawInfoList = ezDynamicArray<DrawInfo>;

struct DispatchInfo
{
  ezUInt32 groupCountX{0U};
  ezUInt32 groupCountY{0U};
  ezUInt32 groupCountZ{0U};

  Buffer* indirectBuffer{nullptr}; // @ts-nullable
  ezUInt32 indirectOffset{0U};
};

using DispatchInfoList = ezDynamicArray<DispatchInfo>;

struct IndirectBuffer
{
  DrawInfoList drawInfos;
};

struct TextureInfo
{
  TextureType type{TextureType::TEX2D};
  TextureUsage usage{TextureUsageBit::NONE};
  Format format{Format::UNKNOWN};
  ezUInt32 width{0U};
  ezUInt32 height{0U};
  TextureFlags flags{TextureFlagBit::NONE};
  ezUInt32 layerCount{1U};
  ezUInt32 levelCount{1U};
  SampleCount samples{SampleCount::ONE};
  ezUInt32 depth{1U};
  void* externalRes{nullptr}; // CVPixelBuffer for Metal, EGLImage for GLES
};

struct TextureViewInfo
{
  Texture* texture{nullptr};
  TextureType type{TextureType::TEX2D};
  Format format{Format::UNKNOWN};
  ezUInt32 baseLevel{0U};
  ezUInt32 levelCount{1U};
  ezUInt32 baseLayer{0U};
  ezUInt32 layerCount{1U};
};

struct SamplerInfo
{
  Filter minFilter{Filter::LINEAR};
  Filter magFilter{Filter::LINEAR};
  Filter mipFilter{Filter::NONE};
  Address addressU{Address::WRAP};
  Address addressV{Address::WRAP};
  Address addressW{Address::WRAP};
  ezUInt32 maxAnisotropy{0U};
  ComparisonFunc cmpFunc{ComparisonFunc::ALWAYS};
};

struct Uniform
{
  ezString name;
  Type type{Type::UNKNOWN};
  ezUInt32 count{0U};
};

using UniformList = ezDynamicArray<Uniform>;

struct UniformBlock
{
  ezUInt32 set{0U};
  ezUInt32 binding{0U};
  ezString name;
  UniformList members;
  ezUInt32 count{0U};
};

using UniformBlockList = ezDynamicArray<UniformBlock>;

struct UniformSamplerTexture
{
  ezUInt32 set{0U};
  ezUInt32 binding{0U};
  ezString name;
  Type type{Type::UNKNOWN};
  ezUInt32 count{0U};
};

using UniformSamplerTextureList = ezDynamicArray<UniformSamplerTexture>;

struct UniformSampler
{
  ezUInt32 set{0U};
  ezUInt32 binding{0U};
  ezString name;
  ezUInt32 count{0U};
};

using UniformSamplerList = ezDynamicArray<UniformSampler>;

struct UniformTexture
{
  ezUInt32 set{0U};
  ezUInt32 binding{0U};
  ezString name;
  Type type{Type::UNKNOWN};
  ezUInt32 count{0U};
};

using UniformTextureList = ezDynamicArray<UniformTexture>;

struct UniformStorageImage
{
  ezUInt32 set{0U};
  ezUInt32 binding{0U};
  ezString name;
  Type type{Type::UNKNOWN};
  ezUInt32 count{0U};
  MemoryAccess memoryAccess{MemoryAccessBit::READ_WRITE};
};

using UniformStorageImageList = ezDynamicArray<UniformStorageImage>;

struct UniformStorageBuffer
{
  ezUInt32 set{0U};
  ezUInt32 binding{0U};
  ezString name;
  ezUInt32 count{0U};
  MemoryAccess memoryAccess{MemoryAccessBit::READ_WRITE};
};

using UniformStorageBufferList = ezDynamicArray<UniformStorageBuffer>;

struct UniformInputAttachment
{
  ezUInt32 set{0U};
  ezUInt32 binding{0U};
  ezString name;
  ezUInt32 count{0U};
};

using UniformInputAttachmentList = ezDynamicArray<UniformInputAttachment>;

struct ShaderStage
{
  ShaderStageFlagBit stage{ShaderStageFlagBit::NONE};
  ezString source;
};

using ShaderStageList = ezDynamicArray<ShaderStage>;

struct Attribute
{
  ezString name;
  Format format{Format::UNKNOWN};
  bool isNormalized{false};
  ezUInt32 stream{0U};
  bool isInstanced{false};
  ezUInt32 location{0U};
};

using AttributeList = ezDynamicArray<Attribute>;

struct ShaderInfo
{
  ezString name;
  ShaderStageList stages;
  AttributeList attributes;
  UniformBlockList blocks;
  UniformStorageBufferList buffers;
  UniformSamplerTextureList samplerTextures;
  UniformSamplerList samplers;
  UniformTextureList textures;
  UniformStorageImageList images;
  UniformInputAttachmentList subpassInputs;
};

struct InputAssemblerInfo
{
  AttributeList attributes;
  BufferList vertexBuffers;
  Buffer* indexBuffer{nullptr};    // @ts-nullable
  Buffer* indirectBuffer{nullptr}; // @ts-nullable
};

struct ColorAttachment
{
  Format format{Format::UNKNOWN};
  SampleCount sampleCount{SampleCount::ONE};
  LoadOp loadOp{LoadOp::CLEAR};
  StoreOp storeOp{StoreOp::STORE};
  ezDynamicArray<AccessType> beginAccesses;
  ezDynamicArray<AccessType> endAccesses; //{AccessType::COLOR_ATTACHMENT_WRITE};
  bool IsGeneralLayout{false};

  inline ColorAttachment() {
    endAccesses.PushBack(AccessType::COLOR_ATTACHMENT_WRITE);
  }
};

using ColorAttachmentList = ezDynamicArray<ColorAttachment>;

struct DepthStencilAttachment
{
  Format format{Format::UNKNOWN};
  SampleCount sampleCount{SampleCount::ONE};
  LoadOp depthLoadOp{LoadOp::CLEAR};
  StoreOp depthStoreOp{StoreOp::STORE};
  LoadOp stencilLoadOp{LoadOp::CLEAR};
  StoreOp stencilStoreOp{StoreOp::STORE};
  ezDynamicArray<AccessType> beginAccesses;
  ezDynamicArray<AccessType> endAccesses; //{AccessType::DEPTH_STENCIL_ATTACHMENT_WRITE};
  bool IsGeneralLayout{false};

  inline DepthStencilAttachment() {
    endAccesses.PushBack(AccessType::DEPTH_STENCIL_ATTACHMENT_WRITE);
  }
};

struct SubpassInfo
{
  ezDynamicArray<ezUInt32> inputs;
  ezDynamicArray<ezUInt32> colors;
  ezDynamicArray<ezUInt32> resolves;
  ezDynamicArray<ezUInt32> preserves;

  ezUInt32 depthStencil{INVALID_BINDING};
  ezUInt32 depthStencilResolve{INVALID_BINDING};
  ResolveMode depthResolveMode{ResolveMode::NONE};
  ResolveMode stencilResolveMode{ResolveMode::NONE};
};

using SubpassInfoList = ezDynamicArray<SubpassInfo>;

struct SubpassDependency
{
  ezUInt32 srcSubpass{0U};
  ezUInt32 dstSubpass{0U};
  ezDynamicArray<AccessType> srcAccesses;
  ezDynamicArray<AccessType> dstAccesses;
};

using SubpassDependencyList = ezDynamicArray<SubpassDependency>;

struct RenderPassInfo
{
  ColorAttachmentList colorAttachments;
  DepthStencilAttachment depthStencilAttachment;
  SubpassInfoList subpasses;
  SubpassDependencyList dependencies;
};

struct GlobalBarrierInfo
{
  ezDynamicArray<AccessType> prevAccesses;
  ezDynamicArray<AccessType> nextAccesses;
};
using GlobalBarrierInfoList = ezDynamicArray<GlobalBarrierInfo>;

struct TextureBarrierInfo
{
  ezDynamicArray<AccessType> prevAccesses;
  ezDynamicArray<AccessType> nextAccesses;

  bool discardContents{false};

  Queue* srcQueue{nullptr}; // @ts-nullable
  Queue* dstQueue{nullptr}; // @ts-nullable
};
using TextureBarrierInfoList = ezDynamicArray<TextureBarrierInfo>;

struct FramebufferInfo
{
  RenderPass* renderPass{nullptr};
  TextureList colorTextures;
  Texture* depthStencilTexture{nullptr}; // @ts-nullable
};

struct DescriptorSetLayoutBinding
{
  ezUInt32 binding{INVALID_BINDING};
  DescriptorType descriptorType{DescriptorType::UNKNOWN};
  ezUInt32 count{0U};
  ShaderStageFlags stageFlags{ShaderStageFlagBit::NONE};
  SamplerList immutableSamplers;
};
using DescriptorSetLayoutBindingList = ezDynamicArray<DescriptorSetLayoutBinding>;

struct DescriptorSetLayoutInfo
{
  DescriptorSetLayoutBindingList bindings;
};

struct DescriptorSetInfo
{
  DescriptorSetLayout* layout{nullptr};
};

struct PipelineLayoutInfo
{
  DescriptorSetLayoutList setLayouts;
};

struct InputState
{
  AttributeList attributes;
};

// Use ezUInt32 for all boolean values to convert memory to RasterizerState* in shared memory.
struct RasterizerState
{
  ezUInt32 isDiscard{0U};
  PolygonMode polygonMode{PolygonMode::FILL};
  ShadeModel shadeModel{ShadeModel::GOURAND};
  CullMode cullMode{CullMode::BACK};
  ezUInt32 isFrontFaceCCW{1U};
  ezUInt32 depthBiasEnabled{0U};
  float depthBias{0.F};
  float depthBiasClamp{0.F};
  float depthBiasSlop{0.F};
  ezUInt32 isDepthClip{1U};
  ezUInt32 isMultisample{0U};
  float lineWidth{1.F};
};

// Use ezUInt32 for all boolean values to convert memory to DepthStencilState* in shared memory.
struct DepthStencilState
{
  ezUInt32 depthTest{1U};
  ezUInt32 depthWrite{1U};
  ComparisonFunc depthFunc{ComparisonFunc::LESS};
  ezUInt32 stencilTestFront{0U};
  ComparisonFunc stencilFuncFront{ComparisonFunc::ALWAYS};
  ezUInt32 stencilReadMaskFront{0xffffffffU};
  ezUInt32 stencilWriteMaskFront{0xffffffffU};
  StencilOp stencilFailOpFront{StencilOp::KEEP};
  StencilOp stencilZFailOpFront{StencilOp::KEEP};
  StencilOp stencilPassOpFront{StencilOp::KEEP};
  ezUInt32 stencilRefFront{1U};
  ezUInt32 stencilTestBack{0U};
  ComparisonFunc stencilFuncBack{ComparisonFunc::ALWAYS};
  ezUInt32 stencilReadMaskBack{0xffffffffU};
  ezUInt32 stencilWriteMaskBack{0xffffffffU};
  StencilOp stencilFailOpBack{StencilOp::KEEP};
  StencilOp stencilZFailOpBack{StencilOp::KEEP};
  StencilOp stencilPassOpBack{StencilOp::KEEP};
  ezUInt32 stencilRefBack{1U};
};

// Use ezUInt32 for all boolean values to do convert memory to BlendTarget* in shared memory.
struct BlendTarget
{
  ezUInt32 blend{0U};
  BlendFactor blendSrc{BlendFactor::ONE};
  BlendFactor blendDst{BlendFactor::ZERO};
  BlendOp blendEq{BlendOp::ADD};
  BlendFactor blendSrcAlpha{BlendFactor::ONE};
  BlendFactor blendDstAlpha{BlendFactor::ZERO};
  BlendOp blendAlphaEq{BlendOp::ADD};
  ColorMask blendColorMask{ColorMask::ALL};
};

using BlendTargetList = ezDynamicArray<BlendTarget>;

// Use ezUInt32 for all boolean values to do memeory copy in shared memory.
struct BlendState
{
  ezUInt32 isA2C{0U};
  ezUInt32 isIndepend{0U};
  Color blendColor;
  BlendTargetList targets; //{1U};

  BlendState() {
    targets.PushBack(BlendTarget{1U});
  }
};

struct PipelineStateInfo
{
  Shader* shader{nullptr};
  PipelineLayout* pipelineLayout{nullptr};
  RenderPass* renderPass{nullptr};
  InputState inputState;
  RasterizerState rasterizerState;
  DepthStencilState depthStencilState;
  BlendState blendState;
  PrimitiveMode primitive{PrimitiveMode::TRIANGLE_LIST};
  DynamicStateFlags dynamicStates{DynamicStateFlagBit::NONE};
  PipelineBindPoint bindPoint{PipelineBindPoint::GRAPHICS};
  ezUInt32 subpass{0U};
};

struct CommandBufferInfo
{
  Queue* queue{nullptr};
  CommandBufferType type{CommandBufferType::PRIMARY};
};

struct QueueInfo
{
  QueueType type{QueueType::GRAPHICS};
};

struct FormatInfo
{
  const ezString name;
  const ezUInt32 size{0U};
  const ezUInt32 count{0U};
  const FormatType type{FormatType::NONE};
  const bool hasAlpha{false};
  const bool hasDepth{false};
  const bool hasStencil{false};
  const bool isCompressed{false};
};

struct MemoryStatus
{
  ezUInt32 bufferSize{0U};
  ezUInt32 textureSize{0U};
};

struct DynamicStencilStates
{
  ezUInt32 writeMask{0U};
  ezUInt32 compareMask{0U};
  ezUInt32 reference{0U};
};

struct DynamicStates
{
  Viewport viewport;
  Rect scissor;
  Color blendConstant;
  float lineWidth{1.F};
  float depthBiasConstant{0.F};
  float depthBiasClamp{0.F};
  float depthBiasSlope{0.F};
  float depthMinBounds{0.F};
  float depthMaxBounds{0.F};

  DynamicStencilStates stencilStatesFront;
  DynamicStencilStates stencilStatesBack;
};
