#pragma once

#include <RHI/RHIDLL.h>

#include <Foundation/Algorithm/HashableStruct.h>
#include <Foundation/Containers/ArrayMap.h>

class ezWindowBase;

/// \brief Type for important RHI events.
struct ezRHIDeviceEvent
{
  enum Type
  {
    AfterInitialize,
    BeforeShutdown,
    BeforeBeginFrame,
    AfterBeginFrame,
    BeforeEndFrame,
    AfterEndFrame,
    // could add resource creation/destruction events, if this would be useful
  };

  Type m_Type;
  class ezRHIDevice* m_pDevice;
};

struct ezRHIBufferCreationDescription : public ezHashableStruct<ezRHIBufferCreationDescription>
{
  ezInt32 m_BindFlags;
  ezInt32 m_Size;
};

struct ezRHITextureCreationDescription : public ezHashableStruct<ezRHITextureCreationDescription>
{
  ezRHITextureType m_Type;
  ezUInt32 m_BindFlags;
  ezRHIResourceFormat m_Format;
  ezUInt32 m_SampleCount;
  ezInt32 m_Width;
  ezInt32 m_Height;
  ezInt32 m_Depth;
  ezInt32 m_MipLevels;
};

struct ezRHIResourceBarrierDescription
{
  ezRHIResourceHandle m_hResource;
  ezRHIResourceState m_StateBefore;
  ezRHIResourceState m_StateAfter;
  ezUInt32 m_BaseMipLevel = 0;
  ezUInt32 m_LevelCount = 1;
  ezUInt32 m_BaseArrayLayer = 0;
  ezUInt32 m_LayerCount = 1;
};

struct ezRHISamplerCreationDescription : public ezHashableStruct<ezRHISamplerCreationDescription>
{
  ezRHISamplerFilter m_Filter;
  ezRHISamplerTextureAddressMode m_Mode;
  ezRHISamplerComparisonFunc m_ComparisonFunc;
};

struct ezRHISwapChainCreationDescription : public ezHashableStruct<ezRHISwapChainCreationDescription>
{
  ezWindowBase* m_pWindow;
  ezUInt32 m_Width;
  ezUInt32 m_Height;
  ezUInt32 m_FrameCount;
  bool m_VSync;
};

struct ezRHIResourceViewCreationDescription : public ezHashableStruct<ezRHIResourceViewCreationDescription>
{
  ezRHIResourceViewType m_ViewType = ezRHIResourceViewType::Unknown;
  ezRHIResourceViewDimension m_Dimension = ezRHIResourceViewDimension::Unknown;
  ezUInt32 m_BaseMipLevel = 0;
  ezUInt32 m_LevelCount = static_cast<ezUInt32>(-1);
  ezUInt32 m_BaseArrayLayer = 0;
  ezUInt32 m_LayerCount = static_cast<ezUInt32>(-1);
  ezUInt32 m_PlaneSlice = 0;
  ezUInt64 m_Offset = 0;
  ezUInt32 m_StructureStride = 0;
  ezUInt64 m_BufferSize = static_cast<ezUInt64>(-1);
  ezRHIResourceFormat::Enum m_BufferFormat = ezRHIResourceFormat::Unknown;
  bool m_Bindless = false;
};

struct ezRHIShaderCreationDescription : public ezHashableStruct<ezRHIShaderCreationDescription>
{
  ezString m_ShaderPath;
  ezString m_EntryPoint;
  ezRHIShaderType m_ShaderType;
  ezRHIShaderBlobType m_ShaderBlobType;
  ezString m_Model;
  ezArrayMap<ezString, ezString> m_Defines;
};

struct ezRHIFramebufferCreationDescription : public ezHashableStruct<ezRHIFramebufferCreationDescription>
{
  ezRHIRenderPassHandle m_hRenderPass;
  ezUInt32 m_Width;
  ezUInt32 m_Height;
  ezDynamicArray<ezRHIResourceViewHandle> m_Colors;
  ezRHIResourceViewHandle m_hDepthStencil;
  ezRHIResourceViewHandle m_hShadingRateImage;
};

struct ezRHIRenderPassColorDescription : public ezHashableStruct<ezRHIRenderPassColorDescription>
{
  ezRHIResourceFormat::Enum m_Format = ezRHIResourceFormat::Unknown;
  ezRHIRenderPassLoadOp m_LoadOp = ezRHIRenderPassLoadOp::Load;
  ezRHIRenderPassStoreOp m_StoreOp = ezRHIRenderPassStoreOp::Store;
};

struct ezRHIRenderPassDepthStencilDescription : public ezHashableStruct<ezRHIRenderPassDepthStencilDescription>
{
  ezRHIResourceFormat::Enum m_Format = ezRHIResourceFormat::Unknown;
  ezRHIRenderPassLoadOp m_DepthLoadOp = ezRHIRenderPassLoadOp::Load;
  ezRHIRenderPassStoreOp m_DepthStoreOp = ezRHIRenderPassStoreOp::Store;
  ezRHIRenderPassLoadOp m_StencilLoadOp = ezRHIRenderPassLoadOp::Load;
  ezRHIRenderPassStoreOp m_StencilStoreOp = ezRHIRenderPassStoreOp::Store;
};

struct ezRHIRenderPassCreationDescription : public ezHashableStruct<ezRHIRenderPassCreationDescription>
{
  ezDynamicArray<ezRHIRenderPassColorDescription> m_Colors;
  ezRHIRenderPassDepthStencilDescription m_DepthStencil;
  ezRHIResourceFormat::Enum m_ShadingRateFormat = ezRHIResourceFormat::Unknown;
  ezUInt32 m_SampleCount = 1;
};

struct ezRHIStencilOpDescription : public ezHashableStruct<ezRHIStencilOpDescription>
{
  ezRHIStencilOp m_FailOp = ezRHIStencilOp::Keep;
  ezRHIStencilOp m_DepthFailOp = ezRHIStencilOp::Keep;
  ezRHIStencilOp m_PassOp = ezRHIStencilOp::Keep;
  ezRHIComparisonFunc m_StencilFunc = ezRHIComparisonFunc::Always;
};

struct ezRHIDepthStencilDescription : public ezHashableStruct<ezRHIDepthStencilDescription>
{
  bool m_DepthTestEnabled = true;
  bool m_DepthWriteEnabled = true;
  ezRHIComparisonFunc m_DepthFunc = ezRHIComparisonFunc::Less;
  bool m_DepthBoundsTestEnabled = false;
  bool m_StencilEnabled = false;
  ezUInt8 m_StencilReadMask = 0xff;
  ezUInt8 m_StencilWriteMask = 0xff;
  ezRHIStencilOpDescription m_FrontFace = {};
  ezRHIStencilOpDescription m_BackFace = {};
};

struct ezRHIBlendDescription : public ezHashableStruct<ezRHIBlendDescription>
{
  bool m_BlendEnabled = false;
  ezRHIBlend m_BlendSrc;
  ezRHIBlend m_BlendDest;
  ezRHIBlendOp m_BlendOp;
  ezRHIBlend m_BlendSrcAlpha;
  ezRHIBlend m_BlendDestApha;
  ezRHIBlendOp m_BlendOpAlpha;
};

struct ezRHIRasterizerDescription : ezHashableStruct<ezRHIRasterizerDescription>
{
  ezRHIFillMode m_FillMode = ezRHIFillMode::Solid;
  ezRHICullMode m_CullMode = ezRHICullMode::Back;
  ezInt32 m_DepthBias = 0;
};

struct ezRHIInputElementDescription : public ezHashableStruct<ezRHIInputElementDescription>
{
  ezUInt32 m_Slot = 0;
  ezString m_SemanticName;
  ezRHIResourceFormat::Enum m_Format = ezRHIResourceFormat::Unknown;
  ezUInt32 m_Stride = 0;
};

struct ezRHIGraphicsPipelineCreationDescription : public ezHashableStruct<ezRHIRenderPassCreationDescription>
{
  ezRHIProgramHandle m_hProgram;
  ezRHIBindingSetLayoutHandle m_hBindingSetLayout;
  ezDynamicArray<ezRHIInputElementDescription> m_InputLayout;
  ezRHIRenderPassHandle m_RenderPass;
  ezRHIDepthStencilDescription m_DepthStencilDesc;
  ezRHIBlendDescription m_BlendDesc;
  ezRHIRasterizerDescription m_RasterizerDesc;
};

struct ezRHIComputePipelineCreationDescription : public ezHashableStruct<ezRHIRenderPassCreationDescription>
{
  ezRHIProgramHandle m_hProgram;
  ezRHIBindingSetLayoutHandle m_hBindingSetLayout;
};

// Basic descriptions
struct ezRHIBufferDescription
{
  ezRHIResourceHandle m_hResource;
  ezRHIResourceFormat::Enum m_Format = ezRHIResourceFormat::Unknown;
  ezUInt32 m_Count = 0;
  ezUInt32 m_Offset = 0;
};

struct ezRHIBindKeyDescription : public ezHashableStruct<ezRHIBindKeyDescription>
{
  ezRHIShaderType m_ShaderType = ezRHIShaderType::Unknown;
  ezRHIResourceViewType m_ViewType = ezRHIResourceViewType::Unknown;
  ezUInt32 m_Slot = 0;
  ezUInt32 m_Space = 0;
  ezUInt32 m_Count = 0;

  //EZ_ALWAYS_INLINE bool operator==(const ezRHIBindKeyDescription& other) const
  //{
  //  return m_ShaderType == other.m_ShaderType &&
  //         m_ViewType == other.m_ViewType &&
  //         m_Slot == other.m_Slot &&
  //         m_Space == other.m_Space &&
  //         m_Count == other.m_Count;
  //}
};

template <>
struct ezCompareHelper<ezRHIBindKeyDescription>
{
  EZ_ALWAYS_INLINE bool Less(const ezRHIBindKeyDescription& a, const ezRHIBindKeyDescription& b) const { return a.CalculateHash() < b.CalculateHash(); }
  EZ_ALWAYS_INLINE bool Equal(const ezRHIBindKeyDescription& a, const ezRHIBindKeyDescription& b) const { return a.CalculateHash() == b.CalculateHash(); }
};

struct ezRHIBindingDescription : public ezHashableStruct<ezRHIBindingDescription>
{
  ezRHIBindKeyDescription m_BindKey;
  ezRHIResourceViewHandle m_hResourceView;
};

enum class ezRHIReturnType
{
  Unknown,
  Float,
  Uint,
  Int,
  Double,
};

struct ezRHIResourceBindingDescription : public ezHashableStruct<ezRHIResourceBindingDescription>
{
  ezString m_Name;
  ezRHIResourceViewType m_Type;
  ezUInt32 m_Slot;
  ezUInt32 m_Space;
  ezUInt32 m_Count;
  ezRHIResourceViewDimension m_Dimension;
  ezRHIReturnType m_ReturnType;
  ezUInt32 m_StructureStride;
};

// Raytracing
struct ezRHIRayTracingShaderGroup
{
  ezRHIRayTracingShaderGroupType m_Type = ezRHIRayTracingShaderGroupType::General;
  ezUInt64 m_General = 0;
  ezUInt64 m_ClosestHit = 0;
  ezUInt64 m_AnyHit = 0;
  ezUInt64 m_Intersection = 0;
};

struct ezRHIRayTracingPipelineCreationDescription : public ezHashableStruct<ezRHIRayTracingPipelineCreationDescription>
{
  ezRHIProgramHandle m_hProgram;
  ezRHIBindingSetLayoutHandle m_hBindingSetLayout;
  ezDynamicArray<ezRHIRayTracingShaderGroup> m_Groups;
};

struct ezRHIRayTracingASPrebuildInfo
{
  ezUInt64 m_AccelerationStructureSize = 0;
  ezUInt64 m_BuildScratchDataSize = 0;
  ezUInt64 m_UpdateScratchDataSize = 0;
};

struct ezRHIRayTracingGeometryDescription
{
  ezRHIBufferDescription m_Vertex;
  ezRHIBufferDescription m_Index;
  ezRHIRayTracingGeometryFlags flags = ezRHIRayTracingGeometryFlags::None;
};

struct ezRHIRayTracingShaderTable : ezHashableStruct<ezRHIRayTracingShaderTable>
{
  ezRHIResourceHandle m_hResource;
  ezUInt64 m_Offset;
  ezUInt64 m_Size;
  ezUInt64 m_Stride;
};

struct ezRHIRayTracingShaderTables : ezHashableStruct<ezRHIRayTracingShaderTables>
{
  ezRHIRayTracingShaderTable m_Raygen;
  ezRHIRayTracingShaderTable m_Miss;
  ezRHIRayTracingShaderTable m_Hit;
  ezRHIRayTracingShaderTable m_Callable;
};

struct ezRHIClearDescription
{
  ezDynamicArray<ezColor> m_Colors;
  float m_Depth = 1.0f;
  ezUInt8 m_Stencil = 0;
};

struct ezRHITextureOffset
{
  ezInt32 m_OffsetX;
  ezInt32 m_OffsetY;
  ezInt32 m_OffsetZ;
};

struct ezRHITextureExtent3D
{
  ezUInt32 m_Width;
  ezUInt32 m_Height;
  ezUInt32 m_Depth;
};

struct ezRHIBufferToTextureCopyRegion
{
  ezInt64 m_BufferOffset;
  ezUInt32 m_BufferRowPitch;
  ezUInt32 m_TextureMipLevel;
  ezUInt32 m_TextureArrayLayer;
  ezRHITextureOffset m_TextureOffset;
  ezRHITextureExtent3D m_TextureExtent;
};

struct ezRHITextureCopyRegion
{
  ezRHITextureExtent3D m_Extent;
  ezUInt32 m_SrcMipLevel;
  ezUInt32 m_SrcArrayLayer;
  ezRHITextureOffset m_SrcOffset;
  ezUInt32 m_DstMipLevel;
  ezUInt32 m_DstArrayLayer;
  ezRHITextureOffset m_DstOffset;
};

struct ezRHIBufferCopyRegion
{
  ezInt64 m_SrcOffset;
  ezInt64 m_DstOffset;
  ezInt64 m_NumBytes;
};

struct ezRHIDeviceCapabilities
{
  ezUInt32 m_TextureDataPitchAlignment = 0;
  bool m_DxrSupported = false;
  bool m_RayQuerySupported = false;
  bool m_VariableRateShadingSupported = false;
  bool m_MeshShadingSupported = false;
  ezUInt32 m_ShadingRateImageTileSize = 0;
  ezUInt32 m_ShaderGroupHandleSize = 0;
  ezUInt32 m_ShaderRecordAlignment = 0;
  ezUInt32 m_ShaderTableAlignment = 0;
};
