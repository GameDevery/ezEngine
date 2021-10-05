#include <RHI/Base/Object.h>

RHIObject::RHIObject(ObjectType type)
  : m_ObjectType(type)
  , m_ObjectID(GenerateObjectID<RHIObject>())
{
}
