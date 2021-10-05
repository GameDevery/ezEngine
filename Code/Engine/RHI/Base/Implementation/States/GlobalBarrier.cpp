#include <RHI/Base/States/GlobalBarrier.h>

GlobalBarrier::GlobalBarrier(const GlobalBarrierInfo& info, ezUInt32 hash)
  : RHIObject(ObjectType::GLOBAL_BARRIER)
{
  _info = info;
  _hash = hash;
}

ezUInt32 GlobalBarrier::computeHash(const GlobalBarrierInfo& info)
{
  auto seed = (info.prevAccesses.GetCount() + info.nextAccesses.GetCount());

  for (const AccessType type : info.prevAccesses)
  {
    seed ^= ToNumber(type) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }
  for (const AccessType type : info.nextAccesses)
  {
    seed ^= ToNumber(type) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  return seed;
}
