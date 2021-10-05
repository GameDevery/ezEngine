#pragma once

#include <RHI/Base/Object.h>

class EZ_RHI_DLL TextureBarrier : public RHIObject
{
public:
  TextureBarrier(const TextureBarrierInfo& info, ezUInt32 hash);

  static ezUInt32 computeHash(const TextureBarrierInfo& info);

  inline const TextureBarrierInfo& getInfo() const { return _info; }
  inline const ezUInt32& getHash() const { return _hash; }

protected:
  TextureBarrierInfo _info;
  ezUInt32 _hash{0U};
};
