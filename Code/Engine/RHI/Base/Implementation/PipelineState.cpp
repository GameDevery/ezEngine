#include <RHI/Base/PipelineState.h>

PipelineState::PipelineState()
  : RHIObject(ObjectType::PIPELINE_STATE)
{
}

PipelineState::~PipelineState() = default;

void PipelineState::initialize(const PipelineStateInfo& info)
{
  _primitive = info.primitive;
  _shader = info.shader;
  _inputState = info.inputState;
  _rasterizerState = info.rasterizerState;
  _depthStencilState = info.depthStencilState;
  _bindPoint = info.bindPoint;
  _blendState = info.blendState;
  _dynamicStates = info.dynamicStates;
  _renderPass = info.renderPass;
  _subpass = info.subpass;
  _pipelineLayout = info.pipelineLayout;

  doInit(info);
}

void PipelineState::destroy()
{
  doDestroy();

  _shader = nullptr;
  _renderPass = nullptr;
  _pipelineLayout = nullptr;
}
