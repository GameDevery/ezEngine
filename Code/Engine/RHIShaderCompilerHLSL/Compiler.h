#pragma once

#include <RHIShaderCompilerHLSL/RHIShaderCompilerHLSLDLL.h>
#include <RHI/Instance/BaseTypes.h>

EZ_RHISHADERCOMPILERHLSL_DLL ezDynamicArray<ezUInt8> Compile(const ShaderDesc& shader, ShaderBlobType blob_type);
