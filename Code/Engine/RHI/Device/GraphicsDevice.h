#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/Swapchain.h>
#include <RHI/Resources/CommandList.h>
#include <RHI/Resources/Fence.h>
#include <RHI/Resources/DeviceBuffer.h>
#include <RHI/Resources/MappedResource.h>
#include <RHI/Resources/MappedResourceView.h>
#include <RHI/Resources/Sampler.h>
#include <RHI/Descriptors/SamplerDescription.h>

#include <RHI/Device/ResourceFactory.h>

#include <optional>


/// <summary>
/// A structure describing several common properties of a GraphicsDevice.
/// </summary>
struct EZ_RHI_DLL RHIGraphicsDeviceOptions
{
  /// <summary>
  /// Indicates whether the GraphicsDevice will support debug features, provided they are supported by the host system.
  /// </summary>
  bool Debug = false;

  /// <summary>
  /// Indicates whether the Graphicsdevice will include a "main" Swapchain. If this value is true, then the GraphicsDevice
  /// must be created with one of the overloads that provides Swapchain source information.
  /// </summary>
  bool HasMainSwapchain = false;

  /// <summary>
  /// An optional <see cref="PixelFormat"/> to be used for the depth buffer of the swapchain. If this value is null, then
  /// no depth buffer will be present on the swapchain.
  /// </summary>
  std::optional<ezEnum<RHIPixelFormat>> SwapchainDepthFormat;

  /// <summary>
  /// Indicates whether the main Swapchain will be synchronized to the window system's vertical refresh rate.
  /// </summary>
  bool SyncToVerticalBlank = false;

  /// <summary>
  /// Specifies which model the rendering backend should use for binding resources. This can be overridden per-pipeline
  /// by specifying a value in <see cref="GraphicsPipelineDescription.ResourceBindingModel"/>.
  /// </summary>
  ezEnum<RHIResourceBindingModel> ResourceBindingModel;

  /// <summary>
  /// Indicates whether a 0-to-1 depth range mapping is preferred. For OpenGL, this is not the default, and is not available
  /// on all systems.
  /// </summary>
  bool PreferDepthRangeZeroToOne = false;

  /// <summary>
  /// Indicates whether a bottom-to-top-increasing clip space Y direction is preferred. For Vulkan, this is not the
  /// default, and may not be available on all systems.
  /// </summary>
  bool PreferStandardClipSpaceYDirection = false;

  /// <summary>
  /// Indicates whether the main Swapchain should use an sRGB format. This value is only used in cases where the properties
  /// of the main SwapChain are not explicitly specified with a <see cref="SwapchainDescription"/>. If they are, then the
  /// value of <see cref="SwapchainDescription.ColorSrgb"/> will supercede the value specified here.
  /// </summary>
  bool SwapchainSrgbFormat = false;

  /// <summary>
  /// Constructs a new GraphicsDeviceOptions for a device with no main Swapchain.
  /// </summary>
  /// <param name="debug">Indicates whether the GraphicsDevice will support debug features, provided they are supported by
  /// the host system.</param>
  RHIGraphicsDeviceOptions(bool debug);

  /// <summary>
  /// Constructs a new GraphicsDeviceOptions for a device with a main Swapchain.
  /// </summary>
  /// <param name="debug">Indicates whether the GraphicsDevice will enable debug features, provided they are supported by
  /// the host system.</param>
  /// <param name="swapchainDepthFormat">An optional <see cref="PixelFormat"/> to be used for the depth buffer of the
  /// swapchain. If this value is null, then no depth buffer will be present on the swapchain.</param>
  /// <param name="syncToVerticalBlank">Indicates whether the main Swapchain will be synchronized to the window system's
  /// vertical refresh rate.</param>
  RHIGraphicsDeviceOptions(bool debug, std::optional<ezEnum<RHIPixelFormat>> swapchainDepthFormat, bool syncToVerticalBlank);

  /// <summary>
  /// Constructs a new GraphicsDeviceOptions for a device with a main Swapchain.
  /// </summary>
  /// <param name="debug">Indicates whether the GraphicsDevice will enable debug features, provided they are supported by
  /// the host system.</param>
  /// <param name="swapchainDepthFormat">An optional <see cref="PixelFormat"/> to be used for the depth buffer of the
  /// swapchain. If this value is null, then no depth buffer will be present on the swapchain.</param>
  /// <param name="syncToVerticalBlank">Indicates whether the main Swapchain will be synchronized to the window system's
  /// vertical refresh rate.</param>
  /// <param name="resourceBindingModel">Specifies which model the rendering backend should use for binding resources.</param>
  RHIGraphicsDeviceOptions(
    bool debug,
    std::optional<ezEnum<RHIPixelFormat>> swapchainDepthFormat,
    bool syncToVerticalBlank,
    ezEnum<RHIResourceBindingModel> resourceBindingModel);

  /// <summary>
  /// Constructs a new GraphicsDeviceOptions for a device with a main Swapchain.
  /// </summary>
  /// <param name="debug">Indicates whether the GraphicsDevice will enable debug features, provided they are supported by
  /// the host system.</param>
  /// <param name="swapchainDepthFormat">An optional <see cref="PixelFormat"/> to be used for the depth buffer of the
  /// swapchain. If this value is null, then no depth buffer will be present on the swapchain.</param>
  /// <param name="syncToVerticalBlank">Indicates whether the main Swapchain will be synchronized to the window system's
  /// vertical refresh rate.</param>
  /// <param name="resourceBindingModel">Specifies which model the rendering backend should use for binding resources.</param>
  /// <param name="preferDepthRangeZeroToOne">Indicates whether a 0-to-1 depth range mapping is preferred. For OpenGL,
  /// this is not the default, and is not available on all systems.</param>
  RHIGraphicsDeviceOptions(
    bool debug,
    std::optional<ezEnum<RHIPixelFormat>> swapchainDepthFormat,
    bool syncToVerticalBlank,
    ezEnum<RHIResourceBindingModel> resourceBindingModel,
    bool preferDepthRangeZeroToOne);

  /// <summary>
  /// Constructs a new GraphicsDeviceOptions for a device with a main Swapchain.
  /// </summary>
  /// <param name="debug">Indicates whether the GraphicsDevice will enable debug features, provided they are supported by
  /// the host system.</param>
  /// <param name="swapchainDepthFormat">An optional <see cref="PixelFormat"/> to be used for the depth buffer of the
  /// swapchain. If this value is null, then no depth buffer will be present on the swapchain.</param>
  /// <param name="syncToVerticalBlank">Indicates whether the main Swapchain will be synchronized to the window system's
  /// vertical refresh rate.</param>
  /// <param name="resourceBindingModel">Specifies which model the rendering backend should use for binding resources.</param>
  /// <param name="preferDepthRangeZeroToOne">Indicates whether a 0-to-1 depth range mapping is preferred. For OpenGL,
  /// this is not the default, and is not available on all systems.</param>
  /// <param name="preferStandardClipSpaceYDirection">Indicates whether a bottom-to-top-increasing clip space Y direction
  /// is preferred. For Vulkan, this is not the default, and is not available on all systems.</param>
  RHIGraphicsDeviceOptions(
    bool debug,
    std::optional<ezEnum<RHIPixelFormat>> swapchainDepthFormat,
    bool syncToVerticalBlank,
    ezEnum<RHIResourceBindingModel> resourceBindingModel,
    bool preferDepthRangeZeroToOne,
    bool preferStandardClipSpaceYDirection);

  /// <summary>
  /// Constructs a new GraphicsDeviceOptions for a device with a main Swapchain.
  /// </summary>
  /// <param name="debug">Indicates whether the GraphicsDevice will enable debug features, provided they are supported by
  /// the host system.</param>
  /// <param name="swapchainDepthFormat">An optional <see cref="PixelFormat"/> to be used for the depth buffer of the
  /// swapchain. If this value is null, then no depth buffer will be present on the swapchain.</param>
  /// <param name="syncToVerticalBlank">Indicates whether the main Swapchain will be synchronized to the window system's
  /// vertical refresh rate.</param>
  /// <param name="resourceBindingModel">Specifies which model the rendering backend should use for binding resources.</param>
  /// <param name="preferDepthRangeZeroToOne">Indicates whether a 0-to-1 depth range mapping is preferred. For OpenGL,
  /// this is not the default, and is not available on all systems.</param>
  /// <param name="preferStandardClipSpaceYDirection">Indicates whether a bottom-to-top-increasing clip space Y direction
  /// is preferred. For Vulkan, this is not the default, and is not available on all systems.</param>
  /// <param name="swapchainSrgbFormat">Indicates whether the main Swapchain should use an sRGB format. This value is only
  /// used in cases where the properties of the main SwapChain are not explicitly specified with a
  /// <see cref="SwapchainDescription"/>. If they are, then the value of <see cref="SwapchainDescription.ColorSrgb"/> will
  /// supercede the value specified here.</param>
  RHIGraphicsDeviceOptions(
    bool debug,
    std::optional<ezEnum<RHIPixelFormat>> swapchainDepthFormat,
    bool syncToVerticalBlank,
    ezEnum<RHIResourceBindingModel> resourceBindingModel,
    bool preferDepthRangeZeroToOne,
    bool preferStandardClipSpaceYDirection,
    bool swapchainSrgbFormat);
};

/// <summary>
/// Represents an abstract graphics device, capable of creating device resources and executing commands.
/// </summary>
class RHIGraphicsDevice
{
public:
  /// <summary>
  /// Gets a value identifying the specific graphics API used by this instance.
  /// </summary>
  ezEnum<RHIGraphicsBackend> GetBackendType() const;

  /// <summary>
  /// Gets a value identifying whether texture coordinates begin in the top left corner of a Texture.
  /// If true, (0, 0) refers to the top-left texel of a Texture. If false, (0, 0) refers to the bottom-left
  /// texel of a Texture. This property is useful for determining how the output of a Framebuffer should be sampled.
  /// </summary>
  bool GetIsUvOriginTopLeft() const;

  /// <summary>
  /// Gets a value indicating whether this device's depth values range from 0 to 1.
  /// If false, depth values instead range from -1 to 1.
  /// </summary>
  bool GetIsDepthRangeZeroToOne() const;

  /// <summary>
  /// Gets a value indicating whether this device's clip space Y values increase from top (-1) to bottom (1).
  /// If false, clip space Y values instead increase from bottom (-1) to top (1).
  /// </summary>
  bool IsClipSpaceYInverted() const;

  /// <summary>
  /// Gets the <see cref="ResourceFactory"/> controlled by this instance.
  /// </summary>
  RHIResourceFactory* GetResourceFactory() const;

  /// <summary>
  /// Retrieves the main Swapchain for this device. This property is only valid if the device was created with a main
  /// Swapchain, and will return null otherwise.
  /// </summary>
  RHISwapchain* GetMainSwapchain() const;

  /// <summary>
  /// Gets a <see cref="GraphicsDeviceFeatures"/> which enumerates the optional features supported by this instance.
  /// </summary>
  const RHIGraphicsDeviceFeatures& GetFeatures() const;

  /// <summary>
  /// Gets whether the main Swapchain's <see cref="SwapBuffers()"/> should be synchronized to the window system's
  /// vertical refresh rate.
  /// This is equivalent to <see cref="MainSwapchain"/>.<see cref="Swapchain.GetSyncToVerticalBlank"/>.
  /// This property cannot be set if this GraphicsDevice was created without a main Swapchain.
  /// </summary>
  bool GetSyncToVerticalBlank() const;

  /// <summary>
  /// Sets whether the main Swapchain's <see cref="SwapBuffers()"/> should be synchronized to the window system's
  /// vertical refresh rate.
  /// This is equivalent to <see cref="MainSwapchain"/>.<see cref="Swapchain.SetSyncToVerticalBlank"/>.
  /// This property cannot be set if this GraphicsDevice was created without a main Swapchain.
  /// </summary>
  void SetSyncToVerticalBlank(bool value);

  /// <summary>
  /// The required alignment, in bytes, for uniform buffer offsets. <see cref="DeviceBufferRange.Offset"/> must be a
  /// multiple of this value. When binding a <see cref="ResourceSet"/> to a <see cref="CommandList"/> with an overload
  /// accepting dynamic offsets, each offset must be a multiple of this value.
  /// </summary>
  ezUInt32 GetUniformBufferMinOffsetAlignment() const;

  /// <summary>
  /// The required alignment, in bytes, for structured buffer offsets. <see cref="DeviceBufferRange.Offset"/> must be a
  /// multiple of this value. When binding a <see cref="ResourceSet"/> to a <see cref="CommandList"/> with an overload
  /// accepting dynamic offsets, each offset must be a multiple of this value.
  /// </summary>
  ezUInt32 GetStructuredBufferMinOffsetAlignment() const;

  /// <summary>
  /// Submits the given <see cref="CommandList"/> for execution by this device.
  /// Commands submitted in this way may not be completed when this method returns.
  /// Use <see cref="WaitForIdle"/> to wait for all submitted commands to complete.
  /// <see cref="CommandList.End"/> must have been called on <paramref name="commandList"/> for this method to succeed.
  /// </summary>
  /// <param name="commandList">The completed <see cref="CommandList"/> to execute. <see cref="CommandList.End"/> must have
  /// been previously called on this object.</param>
  void SubmitCommands(RHICommandList* commandList);

  /// <summary>
  /// Submits the given <see cref="CommandList"/> for execution by this device.
  /// Commands submitted in this way may not be completed when this method returns.
  /// Use <see cref="WaitForIdle"/> to wait for all submitted commands to complete.
  /// <see cref="CommandList.End"/> must have been called on <paramref name="commandList"/> for this method to succeed.
  /// </summary>
  /// <param name="commandList">The completed <see cref="CommandList"/> to execute. <see cref="CommandList.End"/> must have
  /// been previously called on this object.</param>
  /// <param name="fence">A <see cref="Fence"/> which will become signaled after this submission fully completes
  /// execution.</param>
  void SubmitCommands(RHICommandList* commandList, RHIFence* fence);

  /// <summary>
  /// Blocks the calling thread until the given <see cref="Fence"/> becomes signaled.
  /// </summary>
  /// <param name="fence">The <see cref="Fence"/> instance to wait on.</param>
  void WaitForFence(RHIFence* fence);

  /// <summary>
  /// Blocks the calling thread until the given <see cref="Fence"/> becomes signaled, or until a time greater than the
  /// given TimeSpan has elapsed.
  /// </summary>
  /// <param name="fence">The <see cref="Fence"/> instance to wait on.</param>
  /// <param name="nanosecondTimeout">A value in nanoseconds, indicating the maximum time to wait on the Fence.</param>
  /// <returns>True if the Fence was signaled. False if the timeout was reached instead.</returns>
  bool WaitForFence(RHIFence* fence, ezUInt64 nanosecondTimeout);

  /// <summary>
  /// Blocks the calling thread until one or all of the given <see cref="Fence"/> instances have become signaled.
  /// </summary>
  /// <param name="fences">An array of <see cref="Fence"/> objects to wait on.</param>
  /// <param name="waitAll">If true, then this method blocks until all of the given Fences become signaled.
  /// If false, then this method only waits until one of the Fences become signaled.</param>
  void WaitForFences(ezDynamicArray<RHIFence*> fences, bool waitAll);

  /// <summary>
  /// Blocks the calling thread until one or all of the given <see cref="Fence"/> instances have become signaled,
  /// or until the given timeout has been reached.
  /// </summary>
  /// <param name="fences">An array of <see cref="Fence"/> objects to wait on.</param>
  /// <param name="waitAll">If true, then this method blocks until all of the given Fences become signaled.
  /// If false, then this method only waits until one of the Fences become signaled.</param>
  /// <param name="nanosecondTimeout">A value in nanoseconds, indicating the maximum time to wait on the Fence.</param>
  /// <returns>True if the Fence was signaled. False if the timeout was reached instead.</returns>
  bool WaitForFences(ezDynamicArray<RHIFence*> fences, bool waitAll, ezUInt64 nanosecondTimeout);

  /// <summary>
  /// Resets the given <see cref="Fence"/> to the unsignaled state.
  /// </summary>
  /// <param name="fence">The <see cref="Fence"/> instance to reset.</param>
  void ResetFence(RHIFence* fence);

  /// <summary>
  /// Swaps the buffers of the main swapchain and presents the rendered image to the screen.
  /// This is equivalent to passing <see cref="MainSwapchain"/> to <see cref="SwapBuffers(Swapchain)"/>.
  /// This method can only be called if this GraphicsDevice was created with a main Swapchain.
  /// </summary>
  void SwapBuffers();

  /// <summary>
  /// Swaps the buffers of the given swapchain.
  /// </summary>
  /// <param name="swapchain">The <see cref="Swapchain"/> to swap and present.</param>
  void SwapBuffers(RHISwapchain* swapchain);

  /// <summary>
  /// Gets a <see cref="Framebuffer"/> object representing the render targets of the main swapchain.
  /// This is equivalent to <see cref="MainSwapchain"/>.<see cref="Swapchain.Framebuffer"/>.
  /// If this GraphicsDevice was created without a main Swapchain, then this returns null.
  /// </summary>
  RHIFramebuffer* GetSwapchainFramebuffer() const;

  /// <summary>
  /// Notifies this instance that the main window has been resized. This causes the <see cref="SwapchainFramebuffer"/> to
  /// be appropriately resized and recreated.
  /// This is equivalent to calling <see cref="MainSwapchain"/>.<see cref="Swapchain.Resize(ezUInt32, ezUInt32)"/>.
  /// This method can only be called if this GraphicsDevice was created with a main Swapchain.
  /// </summary>
  /// <param name="width">The new width of the main window.</param>
  /// <param name="height">The new height of the main window.</param>
  void ResizeMainWindow(ezUInt32 width, ezUInt32 height);

  /// <summary>
  /// A blocking method that returns when all submitted <see cref="CommandList"/> objects have fully completed.
  /// </summary>
  void WaitForIdle();

  /// <summary>
  /// Gets the maximum sample count supported by the given <see cref="PixelFormat"/>.
  /// </summary>
  /// <param name="format">The format to query.</param>
  /// <param name="depthFormat">Whether the format will be used in a depth texture.</param>
  /// <returns>A <see cref="TextureSampleCount"/> value representing the maximum count that a <see cref="Texture"/> of that
  /// format can be created with.</returns>
  virtual ezEnum<RHITextureSampleCount> GetSampleCountLimit(ezEnum<RHIPixelFormat> format, bool depthFormat) = 0;

  /// <summary>
  /// Maps a <see cref="DeviceBuffer"/> or <see cref="Texture"/> into a CPU-accessible data region. For Texture resources, this
  /// overload maps the first subresource.
  /// </summary>
  /// <param name="resource">The <see cref="DeviceBuffer"/> or <see cref="Texture"/> resource to map.</param>
  /// <param name="mode">The <see cref="MapMode"/> to use.</param>
  /// <returns>A <see cref="MappedResource"/> structure describing the mapped data region.</returns>
  RHIMappedResource* Map(RHIDeviceResource* resource, ezEnum<RHIMapMode> mode);

  /// <summary>
  /// Maps a <see cref="DeviceBuffer"/> or <see cref="Texture"/> into a CPU-accessible data region.
  /// </summary>
  /// <param name="resource">The <see cref="DeviceBuffer"/> or <see cref="Texture"/> resource to map.</param>
  /// <param name="mode">The <see cref="MapMode"/> to use.</param>
  /// <param name="subresource">The subresource to map. Subresources are indexed first by mip slice, then by array layer.
  /// For <see cref="DeviceBuffer"/> resources, this parameter must be 0.</param>
  /// <returns>A <see cref="MappedResource"/> structure describing the mapped data region.</returns>
  RHIMappedResource* Map(RHIDeviceResource* resource, ezEnum<RHIMapMode> mode, ezUInt32 subresource);

  /// <summary>
  /// Maps a <see cref="DeviceBuffer"/> or <see cref="Texture"/> into a CPU-accessible data region, and returns a structured
  /// view over that region. For Texture resources, this overload maps the first subresource.
  /// </summary>
  /// <param name="resource">The <see cref="DeviceBuffer"/> or <see cref="Texture"/> resource to map.</param>
  /// <param name="mode">The <see cref="MapMode"/> to use.</param>
  /// <typeparam name="T">The blittable value type which mapped data is viewed as.</typeparam>
  /// <returns>A <see cref="MappedResource"/> structure describing the mapped data region.</returns>
  template <typename T>
  RHIMappedResourceView<T> Map(RHIDeviceResource* resource, ezEnum<RHIMapMode> mode);

  /// <summary>
  /// Maps a <see cref="DeviceBuffer"/> or <see cref="Texture"/> into a CPU-accessible data region, and returns a structured
  /// view over that region.
  /// </summary>
  /// <param name="resource">The <see cref="DeviceBuffer"/> or <see cref="Texture"/> resource to map.</param>
  /// <param name="mode">The <see cref="MapMode"/> to use.</param>
  /// <param name="subresource">The subresource to map. Subresources are indexed first by mip slice, then by array layer.</param>
  /// <typeparam name="T">The blittable value type which mapped data is viewed as.</typeparam>
  /// <returns>A <see cref="MappedResource"/> structure describing the mapped data region.</returns>
  template <typename T>
  RHIMappedResourceView<T> Map(RHIDeviceResource* resource, ezEnum<RHIMapMode> mode, ezUInt32 subresource);

  /// <summary>
  /// Invalidates a previously-mapped data region for the given <see cref="DeviceBuffer"/> or <see cref="Texture"/>.
  /// For <see cref="Texture"/> resources, this unmaps the first subresource.
  /// </summary>
  /// <param name="resource">The resource to unmap.</param>
  void Unmap(RHIDeviceResource* resource);

  /// <summary>
  /// Invalidates a previously-mapped data region for the given <see cref="DeviceBuffer"/> or <see cref="Texture"/>.
  /// </summary>
  /// <param name="resource">The resource to unmap.</param>
  /// <param name="subresource">The subresource to unmap. Subresources are indexed first by mip slice, then by array layer.
  /// For <see cref="DeviceBuffer"/> resources, this parameter must be 0.</param>
  void Unmap(RHIDeviceResource* resource, ezUInt32 subresource);

  /// <summary>
  /// Updates a portion of a <see cref="Texture"/> resource with new data.
  /// </summary>
  /// <param name="texture">The resource to update.</param>
  /// <param name="source">A pointer to the start of the data to upload. This must point to tightly-packed pixel data for
  /// the region specified.</param>
  /// <param name="size">The number of bytes to upload. This value must match the total size of the texture region
  /// specified.</param>
  /// <param name="x">The minimum X value of the updated region.</param>
  /// <param name="y">The minimum Y value of the updated region.</param>
  /// <param name="z">The minimum Z value of the updated region.</param>
  /// <param name="width">The width of the updated region, in texels.</param>
  /// <param name="height">The height of the updated region, in texels.</param>
  /// <param name="depth">The depth of the updated region, in texels.</param>
  /// <param name="mipLevel">The mipmap level to update. Must be less than the total number of mipmaps contained in the
  /// <see cref="Texture"/>.</param>
  /// <param name="arrayLayer">The array layer to update. Must be less than the total array layer count contained in the
  /// <see cref="Texture"/>.</param>
  void UpdateTexture(
    RHITexture* texture,
    ezUInt8* source,
    ezUInt32 size,
    ezUInt32 x, ezUInt32 y, ezUInt32 z,
    ezUInt32 width, ezUInt32 height, ezUInt32 depth,
    ezUInt32 mipLevel, ezUInt32 arrayLayer);

  /// <summary>
  /// Updates a <see cref="DeviceBuffer"/> region with new data.
  /// This function must be used with a blittable value type <typeparamref name="T"/>.
  /// </summary>
  /// <typeparam name="T">The type of data to upload.</typeparam>
  /// <param name="buffer">The resource to update.</param>
  /// <param name="bufferOffset">An offset, in bytes, from the beginning of the <see cref="DeviceBuffer"/> storage, at
  /// which new data will be uploaded.</param>
  /// <param name="source">The value to upload.</param>
  template <typename T>
  void UpdateBuffer(RHIDeviceBuffer* buffer, ezUInt32 bufferOffset, T source);

  /// <summary>
  /// Updates a <see cref="DeviceBuffer"/> region with new data.
  /// This function must be used with a blittable value type <typeparamref name="T"/>.
  /// </summary>
  /// <typeparam name="T">The type of data to upload.</typeparam>
  /// <param name="buffer">The resource to update.</param>
  /// <param name="bufferOffset">An offset, in bytes, from the beginning of the <see cref="DeviceBuffer"/>'s storage, at
  /// which new data will be uploaded.</param>
  /// <param name="source">A reference to the single value to upload.</param>
  template <typename T>
  void UpdateBuffer(RHIDeviceBuffer* buffer, ezUInt32 bufferOffset, const T& source);

  /// <summary>
  /// Updates a <see cref="DeviceBuffer"/> region with new data.
  /// This function must be used with a blittable value type <typeparamref name="T"/>.
  /// </summary>
  /// <typeparam name="T">The type of data to upload.</typeparam>
  /// <param name="buffer">The resource to update.</param>
  /// <param name="bufferOffset">An offset, in bytes, from the beginning of the <see cref="DeviceBuffer"/>'s storage, at
  /// which new data will be uploaded.</param>
  /// <param name="source">A reference to the first of a series of values to upload.</param>
  /// <param name="size">The total size of the uploaded data, in bytes.</param>
  template <typename T>
  void UpdateBuffer(
    RHIDeviceBuffer* buffer,
    ezUInt32 bufferOffset,
    const T& source,
    ezUInt32 size);

  /// <summary>
  /// Updates a <see cref="DeviceBuffer"/> region with new data.
  /// This function must be used with a blittable value type <typeparamref name="T"/>.
  /// </summary>
  /// <typeparam name="T">The type of data to upload.</typeparam>
  /// <param name="buffer">The resource to update.</param>
  /// <param name="bufferOffset">An offset, in bytes, from the beginning of the <see cref="DeviceBuffer"/>'s storage, at
  /// which new data will be uploaded.</param>
  /// <param name="source">An array containing the data to upload.</param>
  template <typename T>
  void UpdateBuffer(
    RHIDeviceBuffer* buffer,
    ezUInt32 bufferOffset,
    ezDynamicArray<T> source);

  /// <summary>
  /// Updates a <see cref="DeviceBuffer"/> region with new data.
  /// </summary>
  /// <param name="buffer">The resource to update.</param>
  /// <param name="bufferOffset">An offset, in bytes, from the beginning of the <see cref="DeviceBuffer"/>'s storage, at
  /// which new data will be uploaded.</param>
  /// <param name="source">A pointer to the start of the data to upload.</param>
  /// <param name="size">The total size of the uploaded data, in bytes.</param>
  template <typename T>
  void UpdateBuffer(
    RHIDeviceBuffer* buffer,
    ezUInt32 bufferOffset,
    ezUInt8* source,
    ezUInt32 size);

  /// <summary>
  /// Gets whether or not the given <see cref="PixelFormat"/>, <see cref="TextureType"/>, and <see cref="TextureUsage"/>
  /// combination is supported by this instance.
  /// </summary>
  /// <param name="format">The PixelFormat to query.</param>
  /// <param name="type">The TextureType to query.</param>
  /// <param name="usage">The TextureUsage to query.</param>
  /// <returns>True if the given combination is supported; false otherwise.</returns>
  bool GetPixelFormatSupport(
    ezEnum<RHIPixelFormat> format,
    ezEnum<RHITextureType> type,
    ezEnum<RHITextureUsage> usage);

  /// <summary>
  /// Gets whether or not the given <see cref="PixelFormat"/>, <see cref="TextureType"/>, and <see cref="TextureUsage"/>
  /// combination is supported by this instance, and also gets the device-specific properties supported by this instance.
  /// </summary>
  /// <param name="format">The PixelFormat to query.</param>
  /// <param name="type">The TextureType to query.</param>
  /// <param name="usage">The TextureUsage to query.</param>
  /// <param name="properties">If the combination is supported, then this parameter describes the limits of a Texture
  /// created using the given combination of attributes.</param>
  /// <returns>True if the given combination is supported; false otherwise. If the combination is supported,
  /// then <paramref name="properties"/> contains the limits supported by this instance.</returns>
  bool GetPixelFormatSupport(
    ezEnum<RHIPixelFormat> format,
    ezEnum<RHITextureType> type,
    ezEnum<RHITextureUsage> usage,
    RHIPixelFormatProperties& properties);

  /// <summary>
  /// Gets a simple point-filtered <see cref="Sampler"/> object owned by this instance.
  /// This object is created with <see cref="SamplerDescription.Point"/>.
  /// </summary>
  RHISampler* GetPointSampler() const;

  /// <summary>
  /// Gets a simple linear-filtered <see cref="Sampler"/> object owned by this instance.
  /// This object is created with <see cref="SamplerDescription.Linear"/>.
  /// </summary>
  RHISampler* GetLinearSampler() const;

  /// <summary>
  /// Gets a simple 4x anisotropic-filtered <see cref="Sampler"/> object owned by this instance.
  /// This object is created with <see cref="SamplerDescription.Aniso4x"/>.
  /// This property can only be used when <see cref="GraphicsDeviceFeatures.SamplerAnisotropy"/> is supported.
  /// </summary>
  RHISampler* GetAniso4xSampler();

protected:
  /// <summary>
  /// Creates and caches common device resources after device creation completes.
  /// </summary>
  void PostDeviceCreated();


protected:
  virtual ezUInt32 GetUniformBufferMinOffsetAlignmentCore() const = 0;
  virtual ezUInt32 GetStructuredBufferMinOffsetAlignmentCore() const = 0;
  virtual void SubmitCommandsCore(RHICommandList* commandList, RHIFence* fence) = 0;
  virtual bool WaitForFenceCore(RHIFence* fence, ezUInt64 nanosecondTimeout) = 0;
  virtual bool WaitForFencesCore(ezDynamicArray<RHIFence*> fences, bool waitAll, ezUInt64 nanosecondTimeout) = 0;
  virtual void ResetFenceCore(RHIFence* fence) = 0;
  virtual void SwapBuffersCore(RHISwapchain* swapchain) = 0;
  virtual void WaitForIdleCore() = 0;
  virtual RHIMappedResource* MapCore(RHIDeviceResource* resource, ezEnum<RHIMapMode> mode, ezUInt32 subresource) = 0;
  virtual void UnmapCore(RHIDeviceResource* resource, ezUInt32 subresource) = 0;
  virtual void UpdateTextureCore(
    RHITexture* texture,
    ezUInt8* source,
    ezUInt32 size,
    ezUInt32 x, ezUInt32 y, ezUInt32 z,
    ezUInt32 width, ezUInt32 height, ezUInt32 depth,
    ezUInt32 mipLevel, ezUInt32 arrayLayer) = 0;
  virtual void UpdateBufferCore(RHIDeviceBuffer* buffer, ezUInt32 bufferOffset, ezUInt8* source, ezUInt32 size) = 0;
  virtual bool GetPixelFormatSupportCore(
    ezEnum<RHIPixelFormat> format,
    ezEnum<RHITextureType> type,
    ezEnum<RHITextureUsage> usage,
    RHIPixelFormatProperties& properties) = 0;

private:
  static void ValidateUpdateTextureParameters(
    RHITexture* texture,
    ezUInt32 size,
    ezUInt32 x, ezUInt32 y, ezUInt32 z,
    ezUInt32 width, ezUInt32 height, ezUInt32 depth,
    ezUInt32 mipLevel, ezUInt32 arrayLayer);

private:
  ezEnum<RHIGraphicsBackend> BackendType;
  bool IsUvOriginTopLeft;
  bool IsDepthRangeZeroToOne;
  RHIResourceFactory* ResourceFactory;
  RHISwapchain* MainSwapchain;
  RHIGraphicsDeviceFeatures Features;
  bool SyncToVerticalBlank;
  RHISampler* PointSampler;
  RHISampler* LinearSampler;
  RHISampler* Aniso4xSampler;
};

/// <summary>
/// Updates a <see cref="DeviceBuffer"/> region with new data.
/// </summary>
/// <param name="buffer">The resource to update.</param>
/// <param name="bufferOffset">An offset, in bytes, from the beginning of the <see cref="DeviceBuffer"/>'s storage, at
/// which new data will be uploaded.</param>
/// <param name="source">A pointer to the start of the data to upload.</param>
/// <param name="size">The total size of the uploaded data, in bytes.</param>


/// <summary>
/// Updates a <see cref="DeviceBuffer"/> region with new data.
/// This function must be used with a blittable value type <typeparamref name="T"/>.
/// </summary>
/// <typeparam name="T">The type of data to upload.</typeparam>
/// <param name="buffer">The resource to update.</param>
/// <param name="bufferOffset">An offset, in bytes, from the beginning of the <see cref="DeviceBuffer"/>'s storage, at
/// which new data will be uploaded.</param>
/// <param name="source">An array containing the data to upload.</param>


/// <summary>
/// Updates a <see cref="DeviceBuffer"/> region with new data.
/// This function must be used with a blittable value type <typeparamref name="T"/>.
/// </summary>
/// <typeparam name="T">The type of data to upload.</typeparam>
/// <param name="buffer">The resource to update.</param>
/// <param name="bufferOffset">An offset, in bytes, from the beginning of the <see cref="DeviceBuffer"/>'s storage, at
/// which new data will be uploaded.</param>
/// <param name="source">A reference to the first of a series of values to upload.</param>
/// <param name="size">The total size of the uploaded data, in bytes.</param>


/// <summary>
/// Updates a <see cref="DeviceBuffer"/> region with new data.
/// This function must be used with a blittable value type <typeparamref name="T"/>.
/// </summary>
/// <typeparam name="T">The type of data to upload.</typeparam>
/// <param name="buffer">The resource to update.</param>
/// <param name="bufferOffset">An offset, in bytes, from the beginning of the <see cref="DeviceBuffer"/>'s storage, at
/// which new data will be uploaded.</param>
/// <param name="source">A reference to the single value to upload.</param>


/// <summary>
/// Updates a <see cref="DeviceBuffer"/> region with new data.
/// This function must be used with a blittable value type <typeparamref name="T"/>.
/// </summary>
/// <typeparam name="T">The type of data to upload.</typeparam>
/// <param name="buffer">The resource to update.</param>
/// <param name="bufferOffset">An offset, in bytes, from the beginning of the <see cref="DeviceBuffer"/> storage, at
/// which new data will be uploaded.</param>
/// <param name="source">The value to upload.</param>


/// <summary>
/// Maps a <see cref="DeviceBuffer"/> or <see cref="Texture"/> into a CPU-accessible data region, and returns a structured
/// view over that region.
/// </summary>
/// <param name="resource">The <see cref="DeviceBuffer"/> or <see cref="Texture"/> resource to map.</param>
/// <param name="mode">The <see cref="MapMode"/> to use.</param>
/// <param name="subresource">The subresource to map. Subresources are indexed first by mip slice, then by array layer.</param>
/// <typeparam name="T">The blittable value type which mapped data is viewed as.</typeparam>
/// <returns>A <see cref="MappedResource"/> structure describing the mapped data region.</returns>


/// <summary>
/// Maps a <see cref="DeviceBuffer"/> or <see cref="Texture"/> into a CPU-accessible data region, and returns a structured
/// view over that region. For Texture resources, this overload maps the first subresource.
/// </summary>
/// <param name="resource">The <see cref="DeviceBuffer"/> or <see cref="Texture"/> resource to map.</param>
/// <param name="mode">The <see cref="MapMode"/> to use.</param>
/// <typeparam name="T">The blittable value type which mapped data is viewed as.</typeparam>
/// <returns>A <see cref="MappedResource"/> structure describing the mapped data region.</returns>

template <typename T>
RHIMappedResourceView<T> RHIGraphicsDevice::Map(RHIDeviceResource* resource, ezEnum<RHIMapMode> mode)
{
  Map<T>(resource, mode, 0);
}

template <typename T>
RHIMappedResourceView<T> RHIGraphicsDevice::Map(RHIDeviceResource* resource, ezEnum<RHIMapMode> mode, ezUInt32 subresource)
{
  RHIMappedResource* mappedResource = Map(resource, mode, subresource);
  return RHIMappedResourceView<T>(mappedResource);
}

template <typename T>
void RHIGraphicsDevice::UpdateBuffer(RHIDeviceBuffer* buffer, ezUInt32 bufferOffset, T source)
{
  ezUInt8* ptr = reinterpret_cast<ezUInt8*>(&source);
  UpdateBuffer(buffer, bufferOffset, ptr, (ezUInt32)sizeof(T));
}

template <typename T>
void RHIGraphicsDevice::UpdateBuffer(RHIDeviceBuffer* buffer, ezUInt32 bufferOffset, const T& source)
{
  ezUInt8* ptr = reinterpret_cast<ezUInt8*>(&source);
  UpdateBuffer(buffer, bufferOffset, ptr, (ezUInt32)sizeof(T));
}

template <typename T>
void RHIGraphicsDevice::UpdateBuffer(RHIDeviceBuffer* buffer, ezUInt32 bufferOffset, const T& source, ezUInt32 size)
{
  //ezUInt8* ptr = reinterpret_cast<ezUInt8*>(&source);
  //UpdateBuffer(buffer, bufferOffset, ptr, size);
}

template <typename T>
void RHIGraphicsDevice::UpdateBuffer(RHIDeviceBuffer* buffer, ezUInt32 bufferOffset, ezDynamicArray<T> source)
{
  UpdateBuffer(buffer, bufferOffset, source.GetPtr(), source.GetCount());
}

template <typename T>
void RHIGraphicsDevice::UpdateBuffer(RHIDeviceBuffer* buffer, ezUInt32 bufferOffset, ezUInt8* source, ezUInt32 size)
{
  if (bufferOffset + size > buffer.SizeInBytes)
  {
    EZ_REPORT_FAILURE("The data size given to UpdateBuffer is too large. The given buffer can only hold {} total bytes. The requested update would require {} bytes.", buffer->GetSize(), (bufferOffset + size));
  }
  UpdateBufferCore(buffer, bufferOffset, source, size);
}