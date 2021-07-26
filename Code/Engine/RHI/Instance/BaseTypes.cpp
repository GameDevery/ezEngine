//#include <RHIPCH.h>
//
//#include <RHI/Instance/BaseTypes.h>
//
//#include <RHI/BindingSetLayout/BindingSetLayout.h>
//#include <RHI/Program/Program.h>
//#include <RHI/RenderPass/RenderPass.h>
//#include <RHI/Resource/Resource.h>
//
//GraphicsPipelineDesc::GraphicsPipelineDesc(ezSharedPtr<Program> program,
//  ezSharedPtr<BindingSetLayout> layout,
//  std::vector<InputLayoutDesc> input,
//  ezSharedPtr<RenderPass> render_pass,
//  DepthStencilDesc depth_stencil_desc,
//  BlendDesc blend_desc,
//  RasterizerDesc rasterizer_desc)
//  : program{program}
//  , layout{layout}
//  , input{input}
//  , render_pass{render_pass}
//  , depth_stencil_desc{depth_stencil_desc}
//  , blend_desc{blend_desc}
//  , rasterizer_desc{rasterizer_desc}
//{
//}
//
//RayTracingPipelineDesc::RayTracingPipelineDesc(ezSharedPtr<Program> program,
//  ezSharedPtr<BindingSetLayout> layout,
//  std::vector<RayTracingShaderGroup> groups)
//  : program{program}
//  , layout{layout}
//  , groups{groups}
//{
//}
//
//ComputePipelineDesc::ComputePipelineDesc(ezSharedPtr<Program> program = nullptr, ezSharedPtr<BindingSetLayout> layout = nullptr)
//  : program{program}
//  , layout{layout}
//{
//}
