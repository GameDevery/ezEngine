#pragma once

#include <RHI/Base/Def.h>

class EZ_RHI_DLL RHIObject
{
public:
  explicit RHIObject(ObjectType type);
  virtual ~RHIObject() = default;

  inline ObjectType GetObjectType() const { return m_ObjectType; }
  inline ezUInt32 GetObjectID() const { return m_ObjectID; }
  inline ezUInt32 GetTypedID() const { return m_TypedID; }

protected:
  template <typename T>
  static ezUInt32 GenerateObjectID() noexcept
  {
    static ezUInt32 generator = 1 << 16;
    return ++generator;
  }

  ObjectType m_ObjectType = ObjectType::UNKNOWN;
  ezUInt32 m_ObjectID = 0U;

  ezUInt32 m_TypedID = 0U; // inited by sub-classes
};
