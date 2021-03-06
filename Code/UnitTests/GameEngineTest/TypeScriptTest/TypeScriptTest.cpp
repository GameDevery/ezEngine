#include <GameEngineTestPCH.h>

#include "TypeScriptTest.h"
#include <Core/Messages/EventMessage.h>
#include <Core/Scripting/DuktapeFunction.h>
#include <Core/Scripting/DuktapeHelper.h>
#include <Core/WorldSerializer/WorldReader.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <TypeScriptPlugin/Components/TypeScriptComponent.h>

static ezGameEngineTestTypeScript s_GameEngineTestTypeScript;

const char* ezGameEngineTestTypeScript::GetTestName() const
{
  return "TypeScript Tests";
}

ezGameEngineTestApplication* ezGameEngineTestTypeScript::CreateApplication()
{
  m_pOwnApplication = EZ_DEFAULT_NEW(ezGameEngineTestApplication_TypeScript);
  return m_pOwnApplication;
}

void ezGameEngineTestTypeScript::SetupSubTests()
{
  AddSubTest("Vec2", SubTests::Vec2);
  AddSubTest("Vec3", SubTests::Vec3);
  AddSubTest("Quat", SubTests::Quat);
  AddSubTest("Mat3", SubTests::Mat3);
  AddSubTest("Mat4", SubTests::Mat4);
  AddSubTest("Transform", SubTests::Transform);
}

ezResult ezGameEngineTestTypeScript::InitializeSubTest(ezInt32 iIdentifier)
{
  if (iIdentifier == SubTests::Vec2 ||
      iIdentifier == SubTests::Vec3 ||
      iIdentifier == SubTests::Quat ||
      iIdentifier == SubTests::Mat3 ||
      iIdentifier == SubTests::Mat4 ||
      iIdentifier == SubTests::Transform)
  {
    m_pOwnApplication->SubTestBasicsSetup();
    return EZ_SUCCESS;
  }

  return EZ_FAILURE;
}

ezTestAppRun ezGameEngineTestTypeScript::RunSubTest(ezInt32 iIdentifier, ezUInt32 uiInvocationCount)
{
  if (iIdentifier == SubTests::Vec2)
    return m_pOwnApplication->SubTestBasisExec(iIdentifier);

  if (iIdentifier == SubTests::Vec3)
    return m_pOwnApplication->SubTestBasisExec(iIdentifier);

  if (iIdentifier == SubTests::Quat)
    return m_pOwnApplication->SubTestBasisExec(iIdentifier);

  if (iIdentifier == SubTests::Mat3)
    return m_pOwnApplication->SubTestBasisExec(iIdentifier);

  if (iIdentifier == SubTests::Mat4)
    return m_pOwnApplication->SubTestBasisExec(iIdentifier);

  if (iIdentifier == SubTests::Transform)
    return m_pOwnApplication->SubTestBasisExec(iIdentifier);

  EZ_ASSERT_NOT_IMPLEMENTED;
  return ezTestAppRun::Quit;
}

//////////////////////////////////////////////////////////////////////////

ezGameEngineTestApplication_TypeScript::ezGameEngineTestApplication_TypeScript()
  : ezGameEngineTestApplication("TypeScript")
{
}

//////////////////////////////////////////////////////////////////////////

static int Duk_TestFailure(duk_context* pDuk)
{
  ezDuktapeFunction duk(pDuk);

  const char* szFile = duk.GetStringValue(0);
  const ezInt32 iLine = duk.GetIntValue(1);
  const char* szFunction = duk.GetStringValue(2);
  const char* szMsg = duk.GetStringValue(3);

  ezTestBool(false, "TypeScript Test Failed", szFile, iLine, szFunction, szMsg);

  return duk.ReturnVoid();
}

void ezGameEngineTestApplication_TypeScript::SubTestBasicsSetup()
{
  LoadScene("TypeScript/AssetCache/Common/Scenes/TypeScripting.ezObjectGraph");

  EZ_LOCK(m_pWorld->GetWriteMarker());
  ezTypeScriptComponentManager* pMan = m_pWorld->GetOrCreateComponentManager<ezTypeScriptComponentManager>();

  pMan->GetTsBinding().GetDukTapeContext().RegisterGlobalFunction("ezTestFailure", Duk_TestFailure, 4);
}

ezTestAppRun ezGameEngineTestApplication_TypeScript::SubTestBasisExec(ezInt32 iIdentifier)
{
  if (Run() == ezApplication::Quit)
    return ezTestAppRun::Quit;

  EZ_LOCK(m_pWorld->GetWriteMarker());

  ezGameObject* pTests = nullptr;
  if (m_pWorld->TryGetObjectWithGlobalKey("Tests", pTests) == false)
  {
    EZ_TEST_FAILURE("Failed to retrieve TypeScript Tests-Object", "");
    return ezTestAppRun::Quit;
  }

  switch (iIdentifier)
  {
    case ezGameEngineTestTypeScript::SubTests::Vec2:
    {
      ezMsgGenericEvent msg;
      msg.m_sMessage = "TestVec2";
      pTests->SendMessageRecursive(msg);

      EZ_TEST_STRING(msg.m_sMessage, "done");
      break;
    }

    case ezGameEngineTestTypeScript::SubTests::Vec3:
    {
      ezMsgGenericEvent msg;
      msg.m_sMessage = "TestVec3";
      pTests->SendMessageRecursive(msg);

      EZ_TEST_STRING(msg.m_sMessage, "done");
      break;
    }

    case ezGameEngineTestTypeScript::SubTests::Quat:
    {
      ezMsgGenericEvent msg;
      msg.m_sMessage = "TestQuat";
      pTests->SendMessageRecursive(msg);

      EZ_TEST_STRING(msg.m_sMessage, "done");
      break;
    }

    case ezGameEngineTestTypeScript::SubTests::Mat3:
    {
      ezMsgGenericEvent msg;
      msg.m_sMessage = "TestMat3";
      pTests->SendMessageRecursive(msg);

      EZ_TEST_STRING(msg.m_sMessage, "done");
      break;
    }

    case ezGameEngineTestTypeScript::SubTests::Mat4:
    {
      ezMsgGenericEvent msg;
      msg.m_sMessage = "TestMat4";
      pTests->SendMessageRecursive(msg);

      EZ_TEST_STRING(msg.m_sMessage, "done");
      break;
    }

    case ezGameEngineTestTypeScript::SubTests::Transform:
    {
      ezMsgGenericEvent msg;
      msg.m_sMessage = "TestTransform";
      pTests->SendMessageRecursive(msg);

      EZ_TEST_STRING(msg.m_sMessage, "done");
      break;
    }
  }

  return ezTestAppRun::Quit;
}
