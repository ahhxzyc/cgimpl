#pragma once

#include "../win32/window.h"
#include "macro.h"
#include "descriptor_heap.h"

#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

#include <memory>

class D3D12App
{
public:
    D3D12App();

    void Initialize();

    void CreateCommandObjects();

    void CreateSwapChain();

    void CreateDescriptorHeaps();

    void OnResize();

    void OnDraw();

    void Run();

    void OnUpdate();

    void FlushCommandQueue();

    void CreateAssets();

private:
    std::shared_ptr<Window> m_Window;

    Microsoft::WRL::ComPtr<IDXGIFactory4> m_DXGIFactory;
    Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
    Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CmdQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CmdListAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CmdList;
    UINT m_CurrentFence = 0;

    Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
    static const int s_SwapChainBufferCount = 2;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_SwapChainBuffers[s_SwapChainBufferCount];
    Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer;
    int m_CurrentBackBuffer = 0;

    DescriptorHeap m_RTVHeap;
    DescriptorHeap m_DSVHeap;
    DescriptorHeap m_CBVHeap;

    D3D12_VIEWPORT m_Viewport;
    D3D12_RECT m_ScissorRect;

    Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBufferGPU;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBufferGPU;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_ConstantBuffer;
    void *m_ConstantBufferBegin;
    size_t m_ConstantBufferByteSize;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;

    D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
    D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
};
