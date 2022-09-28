#include "descriptor_heap.h"

#include "d3dx12.h"
#include <combaseapi.h>
#include <iostream>
#include <string>

#define CHECK_HR(X)                                                  \
    do {                                                                       \
        const HRESULT hr = X;                                         \
        if(!SUCCEEDED(hr))                                            \
        {                                                                      \
            std::cout <<                                           \
                    std::string("errcode = ")                                  \
                    + std::to_string(hr) + "."                        \
                    + std::system_category().message(hr);            \
        }                                                                      \
    } while(false)



void DescriptorHeap::Create(Microsoft::WRL::ComPtr<ID3D12Device> &device, const D3D12_DESCRIPTOR_HEAP_DESC *desc)
{
    CHECK_HR(device->CreateDescriptorHeap(desc, IID_PPV_ARGS(m_Ptr.GetAddressOf())));
    m_DescriptorSize = device->GetDescriptorHandleIncrementSize(desc->Type);
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUDescriptorHandle(size_t index)
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE ret(m_Ptr->GetCPUDescriptorHandleForHeapStart());
    return ret.Offset(index, m_DescriptorSize);
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUDescriptorHandle(size_t index)
{
    CD3DX12_GPU_DESCRIPTOR_HANDLE ret(m_Ptr->GetGPUDescriptorHandleForHeapStart());
    return ret.Offset(index, m_DescriptorSize);
}
