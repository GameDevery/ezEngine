#include <RHI/Base/States/Sampler.h>

Sampler::Sampler(const SamplerInfo& info, ezUInt32 hash)
  : RHIObject(ObjectType::SAMPLER)
{
  _info = info;
  _hash = hash;
}

ezUInt32 Sampler::computeHash(const SamplerInfo& info)
{
  ezUInt32 hash = ToNumber(info.minFilter);
  hash |= (ToNumber(info.magFilter) << 2);
  hash |= (ToNumber(info.mipFilter) << 4);
  hash |= (ToNumber(info.addressU) << 6);
  hash |= (ToNumber(info.addressV) << 8);
  hash |= (ToNumber(info.addressW) << 10);
  hash |= (info.maxAnisotropy << 12);
  hash |= (ToNumber(info.cmpFunc) << 16);
  return hash;
}
