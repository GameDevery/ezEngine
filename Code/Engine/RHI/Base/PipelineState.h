#pragma once

#include <RHI/Base/Object.h>

class EZ_RHI_DLL PipelineState : public RHIObject
{
public:
  PipelineState();
  ~PipelineState() override;

  void initialize(const PipelineStateInfo& info);
  void destroy();

  inline Shader* getShader() const { return _shader; }
  inline PipelineBindPoint getBindPoint() const { return _bindPoint; }
  inline PrimitiveMode getPrimitive() const { return _primitive; }
  inline DynamicStateFlags getDynamicStates() const { return _dynamicStates; }
  inline const InputState& getInputState() const { return _inputState; }
  inline const RasterizerState& getRasterizerState() const { return _rasterizerState; }
  inline const DepthStencilState& getDepthStencilState() const { return _depthStencilState; }
  inline const BlendState& getBlendState() const { return _blendState; }
  inline const RenderPass* getRenderPass() const { return _renderPass; }
  inline const PipelineLayout* getPipelineLayout() const { return _pipelineLayout; }

protected:
  virtual void doInit(const PipelineStateInfo& info) = 0;
  virtual void doDestroy() = 0;

  Shader* _shader = nullptr;
  PipelineBindPoint _bindPoint = PipelineBindPoint::GRAPHICS;
  PrimitiveMode _primitive = PrimitiveMode::TRIANGLE_LIST;
  DynamicStateFlags _dynamicStates = DynamicStateFlags::NONE;
  InputState _inputState;
  RasterizerState _rasterizerState;
  DepthStencilState _depthStencilState;
  BlendState _blendState;
  RenderPass* _renderPass = nullptr;
  ezUInt32 _subpass = 0U;
  PipelineLayout* _pipelineLayout = nullptr;
};
