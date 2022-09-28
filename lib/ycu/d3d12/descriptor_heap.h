#pragma once

#include <d3d12.h>
//#include <d3dx12.h>
#include <wrl/client.h>

class DescriptorHeap
{
public:
    void Create(Microsoft::WRL::ComPtr<ID3D12Device> &device, const D3D12_DESCRIPTOR_HEAP_DESC *desc);
    
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(size_t index);
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(size_t index);
    auto GetD3D12DescriptorHeap()
    {
        return m_Ptr.Get();
    }
private:
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_Ptr;
    size_t m_DescriptorSize;
};