#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Types/Id.h>
#include <Foundation/Types/RefCounted.h>
#include <Foundation/Types/SharedPtr.h>

// Configure the DLL Import/Export Define
#if EZ_ENABLED(EZ_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_RHI_LIB
#    define EZ_RHI_DLL __declspec(dllexport)
#  else
#    define EZ_RHI_DLL __declspec(dllimport)
#  endif
#else
#  define EZ_RHI_DLL
#endif

class ezRHIAdapter;
class ezRHIBindingSet;
class ezRHIBindingSetLayout;
class ezRHICommandList;
class ezRHICommandQueue;
class ezRHIDevice;
class ezRHIFence;
class ezRHIFramebuffer;
class ezRHIInstance;
class ezRHIMemory;
class ezRHIPipeline;
class ezRHIProgram;
class ezRHIQueryHeap;
class ezRHIRenderPass;
class ezRHIResource;
class ezRHIResourceView;
class ezRHIShader;
class ezRHISwapChain;
class ezRHIShaderReflection;

enum class ezRHIRenderAPI : ezUInt8
{
  DX12,
  Vulkan
};

enum class ezRHIPrimitiveType
{
  PointList,
  LineList,
  TriangleList,
  TriangleStrip,
  PatchList
};

enum class ezRHIMemoryType : ezUInt8
{
  Default,
  Upload,
  Readback
};

enum class ezRHICommandListType : ezUInt8
{
  Graphics,
  Compute,
  Copy,
};

enum class ezRHIResourceType : ezUInt8
{
  Unknown,
  Buffer,
  Texture,
  Sampler,
  AccelerationStructure,
};

enum class ezRHIResourceViewType : ezUInt8
{
  Unknown,
  ConstantBuffer,
  Sampler,
  Texture,
  RWTexture,
  Buffer,
  RWBuffer,
  StructuredBuffer,
  RWStructuredBuffer,
  AccelerationStructure,
  ShadingRateSource,
  RenderTarget,
  DepthStencil
};

struct EZ_RHI_DLL ezRHIResourceFormat
{
  typedef ezUInt32 StorageType;

  enum Enum
  {
    Unknown,

    R8_UINT,
    R8_SINT,
    R8_UNORM,
    R8_SNORM,
    RG8_UINT,
    RG8_SINT,
    RG8_UNORM,
    RG8_SNORM,
    R16_UINT,
    R16_SINT,
    R16_UNORM,
    R16_SNORM,
    R16_FLOAT,
    BGRA4_UNORM,
    B5G6R5_UNORM,
    B5G5R5A1_UNORM,
    RGBA8_UINT,
    RGBA8_SINT,
    RGBA8_UNORM,
    RGBA8_SNORM,
    BGRA8_UNORM,
    SRGBA8_UNORM,
    SBGRA8_UNORM,
    R10G10B10A2_UNORM,
    R11G11B10_FLOAT,
    RG16_UINT,
    RG16_SINT,
    RG16_UNORM,
    RG16_SNORM,
    RG16_FLOAT,
    R32_UINT,
    R32_SINT,
    R32_FLOAT,
    RGBA16_UINT,
    RGBA16_SINT,
    RGBA16_FLOAT,
    RGBA16_UNORM,
    RGBA16_SNORM,
    RG32_UINT,
    RG32_SINT,
    RG32_FLOAT,
    RGB32_UINT,
    RGB32_SINT,
    RGB32_FLOAT,
    RGBA32_UINT,
    RGBA32_SINT,
    RGBA32_FLOAT,

    D16,
    D24S8,
    X24G8_UINT,
    D32,
    D32S8,
    X32G8_UINT,

    BC1_UNORM,
    BC1_UNORM_SRGB,
    BC2_UNORM,
    BC2_UNORM_SRGB,
    BC3_UNORM,
    BC3_UNORM_SRGB,
    BC4_UNORM,
    BC4_SNORM,
    BC5_UNORM,
    BC5_SNORM,
    BC6H_UFLOAT,
    BC6H_SFLOAT,
    BC7_UNORM,
    BC7_UNORM_SRGB,

    ENUM_COUNT,

    Default = RGBA8_UNORM
  };
};

enum class ezRHIFormatKind : ezUInt8
{
  Integer,
  Normalized,
  Float,
  DepthStencil
};

struct ezRHIFormatInfo
{
  ezRHIResourceFormat::Enum m_Format;
  const char* m_Name;
  ezUInt8 m_Stride;
  ezUInt8 m_BlockSize;
  ezVec3U32 m_BlockExtent;
  ezRHIFormatKind m_Kind;
  bool m_HasRed : 1;
  bool m_HasGreen : 1;
  bool m_HasBlue : 1;
  bool m_HasAlpha : 1;
  bool m_HasDepth : 1;
  bool m_HasStencil : 1;
  bool m_IsSigned : 1;
  bool m_IsSRGB : 1;
};

EZ_RHI_DLL const ezRHIFormatInfo& GetFormatInfo(ezRHIResourceFormat::Enum format);

enum class ezRHIResourceViewDimension : ezUInt8
{
  Unknown,
  Buffer,
  Texture1D,
  Texture1DArray,
  Texture2D,
  Texture2DArray,
  Texture2DMS,
  Texture2DMSArray,
  Texture3D,
  TextureCube,
  TextureCubeArray,
};

struct ezRHIResourceState
{
  using StorageType = ezUInt32;
  enum Enum
  {
    Unknown = 0,
    Common = 1 << 0,
    VertexAndConstantBuffer = 1 << 1,
    IndexBuffer = 1 << 2,
    RenderTarget = 1 << 3,
    UnorderedAccess = 1 << 4,
    DepthStencilWrite = 1 << 5,
    DepthStencilRead = 1 << 6,
    NonPixelShaderResource = 1 << 7,
    PixelShaderResource = 1 << 8,
    IndirectArgument = 1 << 9,
    CopyDest = 1 << 10,
    CopySource = 1 << 11,
    RaytracingAccelerationStructure = 1 << 12,
    ShadingRateSource = 1 << 13,
    Present = 1 << 14,
    GenericRead =
      VertexAndConstantBuffer |
      IndexBuffer |
      CopySource |
      NonPixelShaderResource |
      PixelShaderResource |
      IndirectArgument,
    kUndefined = 1 << 15,
    Default = Unknown
  };

  struct Bits
  {
  };
};
EZ_DECLARE_FLAGS_OPERATORS(ezRHIResourceState);

template <>
struct ezCompareHelper<ezBitflags<ezRHIResourceState>>
{
  EZ_ALWAYS_INLINE bool Less(const ezBitflags<ezRHIResourceState>& a, const ezBitflags<ezRHIResourceState>& b) const { return a.GetValue() < b.GetValue(); }
  EZ_ALWAYS_INLINE bool Equal(const ezBitflags<ezRHIResourceState>& a, const ezBitflags<ezRHIResourceState>& b) const { return a == b; }
};

enum class ezRHITextureType : ezUInt8
{
  Texture1D,
  Texture2D,
  Texture3D
};

enum class ezRHIBindFlags // todo: flags
{
  RenderTarget = 1 << 1,
  DepthStencil = 1 << 2,
  ShaderResource = 1 << 3,
  UnorderedAccess = 1 << 4,
  ConstantBuffer = 1 << 5,
  IndexBuffer = 1 << 6,
  VertexBuffer = 1 << 7,
  AccelerationStructure = 1 << 8,
  RayTracing = 1 << 9,
  CopyDest = 1 << 10,
  CopySource = 1 << 11,
  ShadingRateSource = 1 << 12,
  ShaderTable = 1 << 13,
  IndirectBuffer = 1 << 14
};

enum class ezRHISamplerFilter : ezUInt8
{
  Anisotropic,
  MinMagMipLinear,
  ComparisonMinMagMipLinear,
};

enum class ezRHISamplerTextureAddressMode : ezUInt8
{
  Clamp,
  Wrap,
  Border,
  Mirror,
  MirrorOnce
};

enum class ezRHISamplerComparisonFunc : ezUInt8
{
  Never,
  Always,
  Less
};

enum class ezRHIShaderType : ezUInt8
{
  Unknown,
  Vertex,
  Pixel,
  Compute,
  Geometry,
  Amplification,
  Mesh,
  Library
};

enum class ezRHIShaderBlobType : ezUInt8
{
  DXIL,
  SPIRV
};

enum class ezRHIRenderPassLoadOp : ezUInt8
{
  Load,
  Clear,
  DontCare,
};

enum class ezRHIRenderPassStoreOp : ezUInt8
{
  Store = 0,
  DontCare,
};

enum class ezRHIComparisonFunc : ezUInt8
{
  Never,
  Less,
  Equal,
  LessEqual,
  Greater,
  NotEqual,
  GreaterEqual,
  Always
};

enum class ezRHIStencilOp : ezUInt8
{
  Keep,
  Zero,
  Replace,
  IncrSat,
  DecrSat,
  Invert,
  Incr,
  Decr
};

enum class ezRHIBlend : ezUInt8
{
  Zero,
  One,
  SrcColor,
  InvSrcColor,
  SrcAlpha,
  InvSrcAlpha,
  DstAlpha,
  InvDstAlpha,
  DstColor,
  InvDstColor,
  SrcAlphaSaturate,
  ConstantColor,
  InvConstantColor,
  Src1Color,
  InvSrc1Color,
  Src1Alpha,
  InvSrc1Alpha
};

enum class ezRHIBlendOp : ezUInt8
{
  Add,
  Subrtact,
  ReverseSubtract,
  Min,
  Max
};

enum class ezRHIColorMask : ezUInt8 // todo: flags
{
  None = 0,
  Red = 1,
  Green = 2,
  Blue = 3,
  Alpha = 4,
  All = (((Red | Blue) | Green) | Alpha)
};

enum class ezRHIFillMode : ezUInt8
{
  Wireframe,
  Solid
};

enum class ezRHICullMode : ezUInt8
{
  None,
  Front,
  Back,
};

enum class ezRHIQueryHeapType : ezUInt8
{
  AccelerationStructureCompactedSize
};

enum class ezRHIPipelineType : ezUInt8
{
  Graphics,
  Compute,
  RayTracing
};

enum class ezRHIShadingRate : ezUInt8
{
  Sr1x1 = 0,
  Sr1x2 = 0x1,
  Sr2x1 = 0x4,
  Sr2x2 = 0x5,
  Sr2x4 = 0x6,
  Sr4x2 = 0x9,
  Sr4x4 = 0xa,
};

enum class ezRHIShadingRateCombiner : ezUInt8
{
  Passthrough = 0,
  Override = 1,
  Min = 2,
  Max = 3,
  Sum = 4,
};

enum class ezRHICopyAccelerationStructureMode : ezUInt8
{
  Clone,
  Compact,
};

enum class ezRHIAccelerationStructureType : ezUInt8
{
  TopLevel,
  BottomLevel,
};

enum class ezRHIRayTracingShaderGroupType : ezUInt8
{
  General,
  TrianglesHitGroup,
  ProceduralHitGroup,
};

enum class ezRHIRayTracingGeometryFlags : ezUInt8 // todo: flags
{
  None,
  Opaque,
  NoDuplicateAnyHitInvocation
};

enum ezRHIBuildAccelerationStructureFlags : ezUInt8 // todo: enum flags
{
  None = 0,
  AllowUpdate = 1 << 0,
  AllowCompaction = 1 << 1,
  PreferFastTrace = 1 << 2,
  PreferFastBuild = 1 << 3,
  MinimizeMemory = 1 << 4,
};

template <typename CreationDescription>
class ezRHIObject : public ezRefCounted
{
public:
  ezRHIObject(const CreationDescription& Description)
    : m_Description(Description)
  {
  }

  EZ_ALWAYS_INLINE const CreationDescription& GetDescription() const { return m_Description; }

protected:
  const CreationDescription m_Description;
};

// Handles
namespace ezRHI
{
  typedef ezGenericId<16, 16> ez16_16Id;
  typedef ezGenericId<18, 14> ez18_14Id;
  typedef ezGenericId<20, 12> ez20_12Id;
} // namespace ezRHI

class ezRHIBindingSetHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIBindingSetHandle, ezRHI::ez18_14Id);

  friend class ezRHIDevice;
};

class ezRHIBindingSetLayoutHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIBindingSetLayoutHandle, ezRHI::ez18_14Id);

  friend class ezRHIDevice;
};

class ezRHICommandListHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHICommandListHandle, ezRHI::ez18_14Id);

  friend class ezRHIDevice;
};

//class ezRHICommandQueueHandle
//{
//  EZ_DECLARE_HANDLE_TYPE(ezRHICommandQueueHandle, ezRHI::ez16_16Id);
//
//  friend class ezRHIDevice;
//};

class ezRHIFenceHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIFenceHandle, ezRHI::ez20_12Id);

  friend class ezRHIDevice;
};

class ezRHIFramebufferHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIFramebufferHandle, ezRHI::ez20_12Id);

  friend class ezRHIDevice;
};

class ezRHIMemoryHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIMemoryHandle, ezRHI::ez20_12Id);

  friend class ezRHIDevice;
};

class ezRHIPipelineHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIPipelineHandle, ezRHI::ez18_14Id);

  friend class ezRHIDevice;
};

class ezRHIProgramHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIProgramHandle, ezRHI::ez18_14Id);

  friend class ezRHIDevice;
};

class ezRHIQueryHeapHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIQueryHeapHandle, ezRHI::ez20_12Id);

  friend class ezRHIDevice;
};

class ezRHIRenderPassHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIRenderPassHandle, ezRHI::ez20_12Id);

  friend class ezRHIDevice;
};

class ezRHIResourceHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIResourceHandle, ezRHI::ez18_14Id);

  friend class ezRHIDevice;
};

class ezRHIResourceViewHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIResourceViewHandle, ezRHI::ez18_14Id);

  friend class ezRHIDevice;
};

class ezRHIShaderHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHIShaderHandle, ezRHI::ez18_14Id);

  friend class ezRHIDevice;
};

class ezRHISwapChainHandle
{
  EZ_DECLARE_HANDLE_TYPE(ezRHISwapChainHandle, ezRHI::ez16_16Id);

  friend class ezRHIDevice;
};
