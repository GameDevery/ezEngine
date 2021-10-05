#pragma once

#include <RHI/Base/Def.h>
#include <glslang/Public/ShaderLang.h>

class SPIRVUtils
{
public:
  static SPIRVUtils* getInstance() { return &instance; }

  void initialize(int vulkanMinorVersion);
  void destroy();

  void compileGLSL(ShaderStageFlagBit type, const ezString& source);
  void compressInputLocations(AttributeList& attributes);

  inline uint32_t* getOutputData()
  {
    _shader.Clear();
    _program.Clear();
    return _output.GetData();
  }

  inline ezUInt32 getOutputSize()
  {
    return _output.GetCount() * sizeof(uint32_t);
  }

private:
  int _clientInputSemanticsVersion{0};
  glslang::EShTargetClientVersion _clientVersion{glslang::EShTargetClientVersion::EShTargetVulkan_1_0};
  glslang::EShTargetLanguageVersion _targetVersion{glslang::EShTargetLanguageVersion::EShTargetSpv_1_0};

  ezUniquePtr<glslang::TShader> _shader{nullptr};
  ezUniquePtr<glslang::TProgram> _program{nullptr};
  ezDynamicArray<uint32_t> _output;

  static SPIRVUtils instance;
};
