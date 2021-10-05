#pragma once

#include <RHI/Base/Object.h>

class EZ_RHI_DLL GlobalBarrier : public RHIObject
{
public:
  GlobalBarrier(const GlobalBarrierInfo& info, ezUInt32 hash);

  static ezUInt32 computeHash(const GlobalBarrierInfo& info);

  inline const GlobalBarrierInfo& getInfo() const { return _info; }
  inline const ezUInt32& getHash() const { return _hash; }

protected:
  GlobalBarrierInfo _info;
  ezUInt32 _hash{0U};
};
