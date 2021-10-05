#pragma once

#include <RHI/Base/Object.h>

class EZ_RHI_DLL Sampler : public RHIObject
{
public:
  Sampler(const SamplerInfo& info, ezUInt32 hash);

  static ezUInt32 computeHash(const SamplerInfo& info);

  inline const SamplerInfo& getInfo() const { return _info; }
  inline const ezUInt32& getHash() const { return _hash; }

protected:
  SamplerInfo _info;
  ezUInt32 _hash{0U};
};
