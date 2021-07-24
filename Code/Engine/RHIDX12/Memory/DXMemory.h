#pragma once
#include <RHI/Memory/Memory.h>
#include <RHI/Instance/BaseTypes.h>
#include <directx/d3d12.h>
#include <wrl.h>
using namespace Microsoft::WRL;

class DXDevice;

class DXMemory : public Memory
{
public:
    DXMemory(DXDevice& device, ezUInt64 size, MemoryType memory_type, ezUInt32 memory_type_bits);
    MemoryType GetMemoryType() const override;
    ComPtr<ID3D12Heap> GetHeap() const;

private:
    MemoryType m_memory_type;
    ComPtr<ID3D12Heap> m_heap;
};
