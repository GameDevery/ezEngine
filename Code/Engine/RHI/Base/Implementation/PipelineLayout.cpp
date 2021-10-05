#include <RHI/Base/PipelineLayout.h>

PipelineLayout::PipelineLayout()
  : RHIObject(ObjectType::PIPELINE_LAYOUT)
{
}

PipelineLayout::~PipelineLayout() = default;

void PipelineLayout::initialize(const PipelineLayoutInfo& info)
{
  _setLayouts = info.setLayouts;

  doInit(info);
}

void PipelineLayout::destroy()
{
  doDestroy();

  _setLayouts.Clear();
}
