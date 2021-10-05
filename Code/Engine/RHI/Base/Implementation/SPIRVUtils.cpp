#include <Foundation/Logging/Log.h>
#include <RHI/Base/SPIRVUtils.h>
#include <RHI/Base/ThirdParty/spirv.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/StandAlone/ResourceLimits.h>

namespace
{
  EShLanguage getShaderStage(ShaderStageFlagBit type)
  {
    switch (type)
    {
      case ShaderStageFlagBit::VERTEX:
        return EShLangVertex;
      case ShaderStageFlagBit::CONTROL:
        return EShLangTessControl;
      case ShaderStageFlagBit::EVALUATION:
        return EShLangTessEvaluation;
      case ShaderStageFlagBit::GEOMETRY:
        return EShLangGeometry;
      case ShaderStageFlagBit::FRAGMENT:
        return EShLangFragment;
      case ShaderStageFlagBit::COMPUTE:
        return EShLangCompute;
      default:
      {
        EZ_ASSERT_DEV(false, "Unsupported ShaderStageFlagBit, convert to EShLanguage failed.");
        return EShLangVertex;
      }
    }
  }

  glslang::EShTargetClientVersion getClientVersion(int vulkanMinorVersion)
  {
    switch (vulkanMinorVersion)
    {
      case 0:
        return glslang::EShTargetVulkan_1_0;
      case 1:
        return glslang::EShTargetVulkan_1_1;
      case 2:
        return glslang::EShTargetVulkan_1_2;
      default:
      {
        EZ_ASSERT_DEV(false, "Unsupported vulkan version, convert to EShTargetClientVersion failed.");
        return glslang::EShTargetVulkan_1_0;
      }
    }
  }

  glslang::EShTargetLanguageVersion getTargetVersion(int vulkanMinorVersion)
  {
    switch (vulkanMinorVersion)
    {
      case 0:
        return glslang::EShTargetSpv_1_0;
      case 1:
        return glslang::EShTargetSpv_1_3;
      case 2:
        return glslang::EShTargetSpv_1_5;
      default:
      {
        EZ_ASSERT_DEV(false, "Unsupported vulkan version, convert to EShTargetLanguageVersion failed.");
        return glslang::EShTargetSpv_1_0;
      }
    }
  }

  // https://www.khronos.org/registry/spir-v/specs/1.0/SPIRV.pdf
  struct Id
  {
    uint32_t opcode{0};
    uint32_t typeId{0};
    uint32_t storageClass{0};
    uint32_t* pLocation{nullptr};
  };
} // namespace

SPIRVUtils SPIRVUtils::instance;

void SPIRVUtils::initialize(int vulkanMinorVersion)
{
  glslang::InitializeProcess();

  _clientInputSemanticsVersion = 100 + vulkanMinorVersion * 10;
  _clientVersion = getClientVersion(vulkanMinorVersion);
  _targetVersion = getTargetVersion(vulkanMinorVersion);
}

void SPIRVUtils::destroy()
{
  glslang::FinalizeProcess();
  _output.Clear();
}

void SPIRVUtils::compileGLSL(ShaderStageFlagBit type, const ezString& source)
{
  EShLanguage stage = getShaderStage(type);
  const char* string = source.GetData();

  _shader = EZ_DEFAULT_NEW(glslang::TShader, stage);
  _shader->setStrings(&string, 1);

  _shader->setEnvInput(glslang::EShSourceGlsl, stage, glslang::EShClientVulkan, _clientInputSemanticsVersion);
  _shader->setEnvClient(glslang::EShClientVulkan, _clientVersion);
  _shader->setEnvTarget(glslang::EShTargetSpv, _targetVersion);

  auto messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules);

  if (!_shader->parse(&glslang::DefaultTBuiltInResource, _clientInputSemanticsVersion, false, messages))
  {
    ezLog::Error("GLSL Parsing Failed:\n{}\n{}", _shader->getInfoLog(), _shader->getInfoDebugLog());
  }

  _program = EZ_DEFAULT_NEW(glslang::TProgram);
  _program->addShader(_shader.Borrow());

  if (!_program->link(messages))
  {
    ezLog::Error("GLSL Linking Failed:\n{}\n{}", _program->getInfoLog(), _program->getInfoDebugLog());
  }

  _output.Clear();
  spv::SpvBuildLogger logger;
  glslang::SpvOptions spvOptions;
  spvOptions.disableOptimizer = false;
#if CC_DEBUG > 0
  //spvOptions.validate = true;
#else
  spvOptions.optimizeSize = true;
  spvOptions.stripDebugInfo = true;
#endif
  std::vector<ezUInt32> out;
  out.assign(_output.GetData(), _output.GetData() + _output.GetCount()); // ez
  glslang::GlslangToSpv(*_program->getIntermediate(stage), out, &logger, &spvOptions);
}

void SPIRVUtils::compressInputLocations(AttributeList& attributes)
{
  static ezDynamicArray<Id> ids;
  static ezDynamicArray<uint32_t> activeLocations;
  static ezDynamicArray<uint32_t> newLocations;

  uint32_t* code = _output.GetData();
  uint32_t codeSize = _output.GetCount();

  EZ_ASSERT_DEV(code[0] == SpvMagicNumber, "");

  uint32_t idBound = code[3];
  //ids.assign(idBound, {});
  {
    ids.Clear();
    ids.SetCount(idBound, {});
  }

  uint32_t* insn = code + 5;
  while (insn != code + codeSize)
  {
    auto opcode = static_cast<uint16_t>(insn[0]);
    auto wordCount = static_cast<uint16_t>(insn[0] >> 16);

    switch (opcode)
    {
      case SpvOpDecorate:
      {
        EZ_ASSERT_DEV(wordCount >= 3, "");

        uint32_t id = insn[1];
        EZ_ASSERT_DEV(id < idBound, "");

        switch (insn[2])
        {
          case SpvDecorationLocation:
            EZ_ASSERT_DEV(wordCount == 4, "");
            ids[id].pLocation = &insn[3];
            break;
        }
      }
      break;
      case SpvOpVariable:
      {
        EZ_ASSERT_DEV(wordCount >= 4, "");

        uint32_t id = insn[2];
        EZ_ASSERT_DEV(id < idBound, "");

        EZ_ASSERT_DEV(ids[id].opcode == 0, "");
        ids[id].opcode = opcode;
        ids[id].typeId = insn[1];
        ids[id].storageClass = insn[3];
      }
      break;
    }

    EZ_ASSERT_DEV(insn + wordCount <= code + codeSize, "");
    insn += wordCount;
  }

  _program->buildReflection();
  activeLocations.Clear();
  int activeCount = _program->getNumPipeInputs();
  for (int i = 0; i < activeCount; ++i)
  {
    activeLocations.PushBack(_program->getPipeInput(i).getType()->getQualifier().layoutLocation);
  }

  uint32_t location = 0;
  uint32_t unusedLocation = activeCount;
  //newLocations.assign(attributes.size(), UINT_MAX);
  {
    newLocations.Clear();
    newLocations.SetCount(attributes.GetCount(), UINT_MAX);
  }

  for (auto& id : ids)
  {
    if (id.opcode == SpvOpVariable && id.storageClass == SpvStorageClassInput && id.pLocation)
    {
      uint32_t oldLocation = *id.pLocation;

      // update locations in SPIRV
      //if (std::find(activeLocations.begin(), activeLocations.end(), *id.pLocation) != activeLocations.end())
      if (activeLocations.Contains(*id.pLocation))
      {
        *id.pLocation = location++;
      }
      else
      {
        *id.pLocation = unusedLocation++;
      }

      // save record
      bool found{false};
      for (ezUInt32 i = 0; i < attributes.GetCount(); ++i)
      {
        if (attributes[i].location == oldLocation)
        {
          newLocations[i] = *id.pLocation;
          found = true;
          break;
        }
      }

      EZ_ASSERT_DEV(found, "missing attribute declarations?");
    }
  }

  // update gfx references
  for (ezUInt32 i = 0; i < attributes.GetCount(); ++i)
  {
    attributes[i].location = newLocations[i];
  }

  //attributes.erase(std::remove_if(attributes.begin(), attributes.end(), [](const auto& attr)
  //                   { return attr.location == UINT_MAX; }),
  //  attributes.end());

  AttributeList toRemove;

  for (auto& attribute : attributes)
  {
    if (attribute.location == UINT_MAX)
    {
      toRemove.PushBack(attribute);
    }
  }
  for (auto& attr : toRemove)
  {
    attributes.RemoveAndCopy(attr);
  }
}
