#include <TypeScriptPluginPCH.h>

#include <Duktape/duktape.h>
#include <GameEngine/Interfaces/PhysicsWorldModule.h>
#include <TypeScriptPlugin/TsBinding/TsBinding.h>

static int __CPP_Physics_CastRay(duk_context* pDuk);
static int __CPP_Physics_SweepTestSphere(duk_context* pDuk);
static int __CPP_Physics_SweepTestBox(duk_context* pDuk);
static int __CPP_Physics_SweepTestCapsule(duk_context* pDuk);
static int __CPP_Physics_OverlapTestSphere(duk_context* pDuk);
static int __CPP_Physics_OverlapTestCapsule(duk_context* pDuk);
static int __CPP_Physics_GetGravity(duk_context* pDuk);

#define GetPhysicsModule()                                    \
  pWorld->GetOrCreateModule<ezPhysicsWorldModuleInterface>(); \
                                                              \
  if (pModule == nullptr)                                     \
  {                                                           \
    duk.Error("No Physics World Module available.");          \
    return 0;                                                 \
  }

ezResult ezTypeScriptBinding::Init_Physics()
{
  m_Duk.RegisterGlobalFunction("__CPP_Physics_CastRay", __CPP_Physics_CastRay, 6);
  m_Duk.RegisterGlobalFunction("__CPP_Physics_SweepTestSphere", __CPP_Physics_SweepTestSphere, 6);
  m_Duk.RegisterGlobalFunction("__CPP_Physics_SweepTestBox", __CPP_Physics_SweepTestBox, 6);
  m_Duk.RegisterGlobalFunction("__CPP_Physics_SweepTestCapsule", __CPP_Physics_SweepTestCapsule, 7);
  m_Duk.RegisterGlobalFunction("__CPP_Physics_OverlapTestSphere", __CPP_Physics_OverlapTestSphere, 4);
  m_Duk.RegisterGlobalFunction("__CPP_Physics_OverlapTestCapsule", __CPP_Physics_OverlapTestCapsule, 5);
  m_Duk.RegisterGlobalFunction("__CPP_Physics_GetGravity", __CPP_Physics_GetGravity, 0);

  return EZ_SUCCESS;
}

static void PutHitResult(ezDuktapeHelper& duk, const ezPhysicsHitResult& res)
{
  ezTypeScriptBinding* pBinding = ezTypeScriptBinding::RetrieveBinding(duk);

  duk.PushGlobalObject();                                 // [ global ]
  duk.PushLocalObject("__Physics");                       // [ global __Physics ]
  duk.PushLocalObject("Physics");                         // [ global __Physics Physics ]
  duk_get_prop_string(duk, -1, "HitResult");              // [ global __Physics Physics HitResult ]
  duk_new(duk, 0);                                        // [ global __Physics Physics HitResultObj ]
  duk_remove(duk, -2);                                    // [ global __Physics HitResultObj ]
  duk_remove(duk, -2);                                    // [ global HitResultObj ]
  duk_remove(duk, -2);                                    // [ HitResultObj ]
  ;                                                       //
  duk.SetNumberProperty("distance", res.m_fDistance, -1); // [ HitResultObj ]
  duk.SetNumberProperty("shapeId", res.m_uiShapeId, -1);  // [ HitResultObj ]
  ;                                                       //
  ezTypeScriptBinding::PushVec3(duk, res.m_vPosition);    // [ HitResultObj pos ]
  duk.SetCustomProperty("position", -1);                  // [ HitResultObj ]
  ;                                                       //
  ezTypeScriptBinding::PushVec3(duk, res.m_vNormal);      // [ HitResultObj normal ]
  duk.SetCustomProperty("normal", -1);                    // [ HitResultObj ]
  ;                                                       //
  pBinding->DukPutGameObject(res.m_hShapeObject);         // [ HitResultObj GO ]
  duk.SetCustomProperty("shapeObject", -1);               // [ HitResultObj ]
  ;                                                       //
  pBinding->DukPutGameObject(res.m_hActorObject);         // [ HitResultObj GO ]
  duk.SetCustomProperty("actorObject", -1);               // [ HitResultObj ]
}

static int __CPP_Physics_CastRay(duk_context* pDuk)
{
  ezDuktapeFunction duk(pDuk);
  ezWorld* pWorld = ezTypeScriptBinding::RetrieveWorld(duk);

  ezPhysicsWorldModuleInterface* pModule = GetPhysicsModule();

  const ezVec3 vStart = ezTypeScriptBinding::GetVec3(duk, 0);
  const ezVec3 vDir = ezTypeScriptBinding::GetVec3(duk, 1);
  const float fDistance = duk.GetFloatValue(2);
  const ezUInt32 uiCollisionLayer = duk.GetUIntValue(3);
  const ezUInt32 uiShapeTypes = duk.GetUIntValue(4);
  const ezUInt32 uiIgnoreShapeID = duk.GetIntValue(5);

  ezPhysicsHitResult res;
  if (!pModule->CastRay(vStart, vDir, fDistance, uiCollisionLayer, res, static_cast<ezPhysicsShapeType::Enum>(uiShapeTypes), uiIgnoreShapeID))
  {
    return duk.ReturnNull();
  }

  PutHitResult(duk, res);

  EZ_DUK_RETURN_AND_VERIFY_STACK(duk, duk.ReturnCustom(), +1);
}

static int __CPP_Physics_SweepTestSphere(duk_context* pDuk)
{
  ezDuktapeFunction duk(pDuk);
  ezWorld* pWorld = ezTypeScriptBinding::RetrieveWorld(duk);

  ezPhysicsWorldModuleInterface* pModule = GetPhysicsModule();

  const float fRadius = duk.GetFloatValue(0);
  const ezVec3 vStart = ezTypeScriptBinding::GetVec3(duk, 1);
  const ezVec3 vDir = ezTypeScriptBinding::GetVec3(duk, 2);
  const float fDistance = duk.GetFloatValue(3);
  const ezUInt32 uiCollisionLayer = duk.GetUIntValue(4);
  const ezUInt32 uiIgnoreShapeID = duk.GetIntValue(5);

  ezPhysicsHitResult res;
  if (!pModule->SweepTestSphere(fRadius, vStart, vDir, fDistance, uiCollisionLayer, res, uiIgnoreShapeID))
  {
    return duk.ReturnNull();
  }

  PutHitResult(duk, res);

  EZ_DUK_RETURN_AND_VERIFY_STACK(duk, duk.ReturnCustom(), +1);
}

static int __CPP_Physics_SweepTestBox(duk_context* pDuk)
{
  ezDuktapeFunction duk(pDuk);
  ezWorld* pWorld = ezTypeScriptBinding::RetrieveWorld(duk);

  ezPhysicsWorldModuleInterface* pModule = GetPhysicsModule();

  const ezVec3 vExtents = ezTypeScriptBinding::GetVec3(duk, 0);
  const ezTransform transform = ezTypeScriptBinding::GetTransform(duk, 1);
  const ezVec3 vDir = ezTypeScriptBinding::GetVec3(duk, 2);
  const float fDistance = duk.GetFloatValue(3);
  const ezUInt32 uiCollisionLayer = duk.GetUIntValue(4);
  const ezUInt32 uiIgnoreShapeID = duk.GetIntValue(5);

  ezPhysicsHitResult res;
  if (!pModule->SweepTestBox(vExtents, transform, vDir, fDistance, uiCollisionLayer, res, uiIgnoreShapeID))
  {
    return duk.ReturnNull();
  }

  PutHitResult(duk, res);

  EZ_DUK_RETURN_AND_VERIFY_STACK(duk, duk.ReturnCustom(), +1);
}

static int __CPP_Physics_SweepTestCapsule(duk_context* pDuk)
{
  ezDuktapeFunction duk(pDuk);
  ezWorld* pWorld = ezTypeScriptBinding::RetrieveWorld(duk);

  ezPhysicsWorldModuleInterface* pModule = GetPhysicsModule();

  const float fCapsuleRadius = duk.GetFloatValue(0);
  const float fCapsuleHeight = duk.GetFloatValue(1);
  const ezTransform transform = ezTypeScriptBinding::GetTransform(duk, 2);
  const ezVec3 vDir = ezTypeScriptBinding::GetVec3(duk, 3);
  const float fDistance = duk.GetFloatValue(4);
  const ezUInt32 uiCollisionLayer = duk.GetUIntValue(5);
  const ezUInt32 uiIgnoreShapeID = duk.GetIntValue(6);

  ezPhysicsHitResult res;
  if (!pModule->SweepTestCapsule(fCapsuleRadius, fCapsuleHeight, transform, vDir, fDistance, uiCollisionLayer, res, uiIgnoreShapeID))
  {
    return duk.ReturnNull();
  }

  PutHitResult(duk, res);

  EZ_DUK_RETURN_AND_VERIFY_STACK(duk, duk.ReturnCustom(), +1);
}

static int __CPP_Physics_OverlapTestSphere(duk_context* pDuk)
{
  ezDuktapeFunction duk(pDuk);
  ezWorld* pWorld = ezTypeScriptBinding::RetrieveWorld(duk);

  ezPhysicsWorldModuleInterface* pModule = GetPhysicsModule();

  const float fRadius = duk.GetFloatValue(0);
  const ezVec3 vPos = ezTypeScriptBinding::GetVec3(duk, 1);
  const ezUInt32 uiCollisionLayer = duk.GetUIntValue(2);
  const ezUInt32 uiIgnoreShapeID = duk.GetIntValue(3);

  return duk.ReturnBool(pModule->OverlapTestSphere(fRadius, vPos, uiCollisionLayer, uiIgnoreShapeID));
}

static int __CPP_Physics_OverlapTestCapsule(duk_context* pDuk)
{
  ezDuktapeFunction duk(pDuk);
  ezWorld* pWorld = ezTypeScriptBinding::RetrieveWorld(duk);

  ezPhysicsWorldModuleInterface* pModule = GetPhysicsModule();

  const float fCapsuleRadius = duk.GetFloatValue(0);
  const float fCapsuleHeight = duk.GetFloatValue(1);
  const ezTransform transform = ezTypeScriptBinding::GetTransform(duk, 2);
  const ezUInt32 uiCollisionLayer = duk.GetUIntValue(3);
  const ezUInt32 uiIgnoreShapeID = duk.GetIntValue(4);

  return duk.ReturnBool(pModule->OverlapTestCapsule(fCapsuleRadius, fCapsuleHeight, transform, uiCollisionLayer, uiIgnoreShapeID));
}

static int __CPP_Physics_GetGravity(duk_context* pDuk)
{
  ezDuktapeFunction duk(pDuk);
  ezWorld* pWorld = ezTypeScriptBinding::RetrieveWorld(duk);

  ezPhysicsWorldModuleInterface* pModule = GetPhysicsModule();

  ezTypeScriptBinding::PushVec3(duk, pModule->GetGravity());
  return duk.ReturnCustom();
}