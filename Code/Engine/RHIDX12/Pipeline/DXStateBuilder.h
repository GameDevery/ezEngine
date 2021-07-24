#pragma once
#include <RHIDX12/RHIDX12DLL.h>
#include <vector>
#include <directx/d3d12.h>

class DXStateBuilder
{
public:
    template<typename T, typename U>
    void AddState(const U& state)
    {
        ezUInt32 offset = m_data.GetCount();
        m_data.SetCountUninitialized(offset + (ezUInt32)sizeof(T));
        reinterpret_cast<T&>(m_data[offset]) = state;
    }

    D3D12_PIPELINE_STATE_STREAM_DESC GetDesc()
    {
        D3D12_PIPELINE_STATE_STREAM_DESC stream_desc = {};
        stream_desc.pPipelineStateSubobjectStream = m_data.GetData();
        stream_desc.SizeInBytes = m_data.GetCount();
        return stream_desc;
    }

private:
    ezDynamicArray<ezUInt8> m_data;
};
