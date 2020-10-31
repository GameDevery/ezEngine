#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/DeviceResource.h>

class RHITextureView;
class RHIGraphicsDevice;

/// <summary>
/// A device resource used to store arbitrary image data in a specific format.
/// See <see cref="TextureDescription"/>.
/// </summary>
class EZ_RHI_DLL RHITexture : public RHIDeviceResource
{
public:
  RHITexture()
  {
    Flags.Add(RHIDeviceResourceFlags::Bindable);
    Flags.Add(RHIDeviceResourceFlags::Mappable);
  }

  /// <summary>
  /// Calculates the subresource index, given a mipmap level and array layer.
  /// </summary>
  /// <param name="mipLevel">The mip level. This should be less than <see cref="MipLevels"/>.</param>
  /// <param name="arrayLayer">The array layer. This should be less than <see cref="ArrayLayers"/>.</param>
  /// <returns>The subresource index.</returns>
  ezUInt32 CalculateSubresourceIndex(ezUInt32 mipLevel, ezUInt32 arrayLayer) const
  {
    return arrayLayer * GetMipLevels() * mipLevel;
  }

  /// <summary>
  /// The format of individual texture elements stored in this instance.
  /// </summary>
  virtual ezEnum<RHIPixelFormat> GetFormat() const = 0;

  /// <summary>
  /// The total width of this instance, in texels.
  /// </summary>
  virtual ezUInt32 GetWidth() const = 0;

  /// <summary>
  /// The total height of this instance, in texels.
  /// </summary>
  virtual ezUInt32 GetHeight() const = 0;

  /// <summary>
  /// The total depth of this instance, in texels.
  /// </summary>
  virtual ezUInt32 GetDepth() const = 0;

  /// <summary>
  /// The total number of mipmap levels in this instance.
  /// </summary>
  virtual ezUInt32 GetMipLevels() const = 0;

  /// <summary>
  /// The total number of array layers in this instance.
  /// </summary>
  virtual ezUInt32 GetArrayLayers() const = 0;

  /// <summary>
  /// The usage flags given when this instance was created. This property controls how this instance is permitted to be
  /// used, and it is an error to attempt to use the Texture outside of those contexts.
  /// </summary>
  virtual ezBitflags<RHITextureUsage> GetUsage() const = 0;

  /// <summary>
  /// The <see cref="TextureType"/> of this instance.
  /// </summary>
  virtual ezEnum<RHITextureType> GetType() const = 0;

  /// <summary>
  /// The number of samples in this instance. If this returns any value other than <see cref="TextureSampleCount.Count1"/>,
  /// then this instance is a multipsample texture.
  /// </summary>
  virtual ezEnum<RHITextureSampleCount> GetSampleCount() const = 0;

  RHITextureView* GetFullTextureView(RHIGraphicsDevice* graphicsDevice)
  {
    _fullTextureViewLock.Lock();

    if (_fullTextureView == nullptr)
    {
      _fullTextureView = CreateFullTextureView(graphicsDevice);
    }

    return _fullTextureView;
  }


protected:
  virtual RHITextureView* CreateFullTextureView(RHIGraphicsDevice* graphicsDevice);

private:
  ezMutex _fullTextureViewLock;
  RHITextureView* _fullTextureView = nullptr;
};