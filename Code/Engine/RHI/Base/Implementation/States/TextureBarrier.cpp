#include <RHI/Base/States/TextureBarrier.h>
#include <RHI/Base/Queue.h>

TextureBarrier::TextureBarrier(const TextureBarrierInfo& info, ezUInt32 hash)
  : RHIObject(ObjectType::TEXTURE_BARRIER)
{
  _info = info;
  _hash = hash;
}

ezUInt32 TextureBarrier::computeHash(const TextureBarrierInfo& info)
{
  auto seed = (info.prevAccesses.GetCount() + info.nextAccesses.GetCount() + 3);

  for (const AccessType type : info.prevAccesses)
  {
    seed ^= ToNumber(type) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }
  for (const AccessType type : info.nextAccesses)
  {
    seed ^= ToNumber(type) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  seed ^= info.discardContents + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= (info.srcQueue ? ToNumber(info.srcQueue->getType()) : 0U) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= (info.dstQueue ? ToNumber(info.dstQueue->getType()) : 0U) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

  return seed;
}
