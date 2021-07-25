#include <RHIDX12/BindingSet/DXBindingSet.h>
#include <RHIDX12/Device/DXDevice.h>
#include <RHIDX12/Program/DXProgram.h>
#include <RHIDX12/View/DXView.h>
#include <deque>
#include <directx/d3dx12.h>
#include <set>
#include <stdexcept>

DXProgram::DXProgram(DXDevice& device, const ezDynamicArray<ezSharedPtr<Shader>>& shaders)
  : ProgramBase(shaders)
{
}
