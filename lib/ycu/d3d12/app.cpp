#include "app.h"
#include "d3dx12.h"

#include "../math.h"


#include <combaseapi.h>
#include <DirectXColors.h>
#include <d3dcompiler.h>


#include <array>
#include <iostream>

using Microsoft::WRL::ComPtr;
using ycu::math::float3;
using ycu::math::float4;
using ycu::math::mat4f;

struct Vertex
{
    float3 Pos;
    float4 Color;
};

float3 camPos;
float tt = 0.f;

struct TransformData
{
    mat4f MVP = mat4f::identity();
} g_TransformData;

D3D12App::D3D12App()
{
    WindowSpec spec;
    spec.width = 800;
    spec.height = 800;
    spec.title = "DX12 - ZYC";
    m_Window = std::make_shared<Window>(spec);

    Initialize();
}

void D3D12App::Initialize()
{
#if defined(DEBUG) || defined(_DEBUG) 
    // Enable the D3D12 debug layer.
    {
        ComPtr<ID3D12Debug> debugController;
        CHECK_HR(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
        debugController->EnableDebugLayer();
    }
#endif
    // Create factory, device, and fence
    CHECK_HR(CreateDXGIFactory1(IID_PPV_ARGS(m_DXGIFactory.GetAddressOf())));
    CHECK_HR(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device)));
    CHECK_HR(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));

    CreateCommandObjects();
    CreateSwapChain();
    CreateDescriptorHeaps();

    CreateAssets();

    OnResize();
}

void D3D12App::CreateCommandObjects()
{
    D3D12_COMMAND_QUEUE_DESC qd = {};
    qd.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    qd.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    CHECK_HR(m_Device->CreateCommandQueue(&qd, IID_PPV_ARGS(m_CmdQueue.GetAddressOf())));
    CHECK_HR(m_Device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CmdListAllocator.GetAddressOf())));
    CHECK_HR(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_CmdListAllocator.Get(), nullptr, IID_PPV_ARGS(m_CmdList.GetAddressOf())));
    m_CmdList->Close();
}

void D3D12App::CreateSwapChain()
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = m_Window->GetWidth();
    sd.BufferDesc.Height = m_Window->GetHeight();
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 2;
    sd.OutputWindow = m_Window->GetHandle();
    sd.Windowed = true;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    CHECK_HR(m_DXGIFactory->CreateSwapChain(m_CmdQueue.Get(), &sd, m_SwapChain.GetAddressOf()));

}

void D3D12App::CreateDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvhd = {};
    rtvhd.NumDescriptors = s_SwapChainBufferCount;
    rtvhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvhd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvhd.NodeMask = 0;
    m_RTVHeap.Create(m_Device, &rtvhd);

    D3D12_DESCRIPTOR_HEAP_DESC dsvhd = {};
    dsvhd.NumDescriptors = 1;
    dsvhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvhd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvhd.NodeMask = 0;
    m_DSVHeap.Create(m_Device, &dsvhd);

    D3D12_DESCRIPTOR_HEAP_DESC cbvhd = {};
    cbvhd.NumDescriptors = 1;
    cbvhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvhd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvhd.NodeMask = 0;
    m_CBVHeap.Create(m_Device, &cbvhd);

    // create constant buffer
    m_ConstantBufferByteSize = (sizeof(TransformData) + 255) & ~255;
    CHECK_HR(m_Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(m_ConstantBufferByteSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_ConstantBuffer)));

    // create a constant buffer view
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = m_ConstantBuffer->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = m_ConstantBufferByteSize;
    m_Device->CreateConstantBufferView(&cbvDesc, m_CBVHeap.GetCPUDescriptorHandle(0));

    // create constant buffer CPU mapping
    CHECK_HR(m_ConstantBuffer->Map(0, nullptr, reinterpret_cast<void **>(&m_ConstantBufferBegin)));
}

void D3D12App::OnResize()
{
    // Prepare for recording commands
    FlushCommandQueue();
    m_CmdListAllocator->Reset();
    CHECK_HR(m_CmdList->Reset(m_CmdListAllocator.Get(), m_PipelineState.Get()));

    // release resources
    for (int i = 0; i < s_SwapChainBufferCount; ++i)
        m_SwapChainBuffers[i].Reset();
    m_DepthStencilBuffer.Reset();

    // Resize the swap chain.
    CHECK_HR(m_SwapChain->ResizeBuffers(
        s_SwapChainBufferCount,
        m_Window->GetWidth(), m_Window->GetHeight(),
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
    m_CurrentBackBuffer = 0;

    // update render target views to reference current render target resources
    for (int i = 0; i < s_SwapChainBufferCount; i++)
    {
        CHECK_HR(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffers[i])));
        m_Device->CreateRenderTargetView(m_SwapChainBuffers[i].Get(), nullptr, m_RTVHeap.GetCPUDescriptorHandle(i));
    }

    // create new depth stencil resource 
    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = m_Window->GetWidth();
    depthStencilDesc.Height = m_Window->GetHeight();
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    D3D12_CLEAR_VALUE optClear = {};
    optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;
    CHECK_HR(m_Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_COMMON,
        &optClear,
        IID_PPV_ARGS(m_DepthStencilBuffer.GetAddressOf())));

    // update depth stencil view
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.Texture2D.MipSlice = 0;
    m_Device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &dsvDesc, m_DSVHeap.GetCPUDescriptorHandle(0));

    // prepare state of depth stencil view
    m_CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilBuffer.Get(),
        D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

    // Execute the above commands
    CHECK_HR(m_CmdList->Close());
    ID3D12CommandList *cmdsLists[] = { m_CmdList.Get() };
    m_CmdQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
    FlushCommandQueue();

    // update viewport and scissor rect
    m_Viewport.TopLeftX = 0.f;
    m_Viewport.TopLeftY = 0.f;
    m_Viewport.Width = static_cast<float>(m_Window->GetWidth());
    m_Viewport.Height = static_cast<float>(m_Window->GetHeight());
    m_Viewport.MinDepth = 0.f;
    m_Viewport.MaxDepth = 1.f;
    m_ScissorRect = { 0, 0, m_Window->GetWidth(), m_Window->GetHeight() };
}

void D3D12App::OnDraw()
{
    auto currentBackBuffer = m_SwapChainBuffers[m_CurrentBackBuffer].Get();
    auto currentBackBufferView = m_RTVHeap.GetCPUDescriptorHandle(m_CurrentBackBuffer);
    auto depthStencilView = m_DSVHeap.GetCPUDescriptorHandle(0);

    // prepare for command recording
    CHECK_HR(m_CmdListAllocator->Reset());
    CHECK_HR(m_CmdList->Reset(m_CmdListAllocator.Get(), m_PipelineState.Get()));

    // necessary state updates for command list
    m_CmdList->SetGraphicsRootSignature(m_RootSignature.Get());
    ID3D12DescriptorHeap *descriptorHeaps[] = { m_CBVHeap.GetD3D12DescriptorHeap() };
    m_CmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    m_CmdList->SetGraphicsRootDescriptorTable(0, m_CBVHeap.GetGPUDescriptorHandle(0));
    m_CmdList->RSSetViewports(1, &m_Viewport);
    m_CmdList->RSSetScissorRects(1, &m_ScissorRect);

    // indicate that the back buffer will be used as render target
    m_CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(currentBackBuffer,
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    // bind render target
    m_CmdList->OMSetRenderTargets(1, &currentBackBufferView, true, &depthStencilView);

    // record commands
    m_CmdList->ClearRenderTargetView(currentBackBufferView, DirectX::Colors::LightSteelBlue, 0, nullptr);
    m_CmdList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
    m_CmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_CmdList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
    m_CmdList->IASetIndexBuffer(&m_IndexBufferView);
    m_CmdList->DrawIndexedInstanced(36, 1, 0, 0, 0);

    // indicate that the back buffer will be use to present
    m_CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(currentBackBuffer,
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

    // add commands to queue
    CHECK_HR(m_CmdList->Close());
    ID3D12CommandList *cmdsLists[] = { m_CmdList.Get() };
    m_CmdQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // swap the back and front buffers
    CHECK_HR(m_SwapChain->Present(0, 0));
    m_CurrentBackBuffer = (m_CurrentBackBuffer + 1) % s_SwapChainBufferCount;

    // wait for this frame to finish
    FlushCommandQueue();
}

void D3D12App::Run()
{
    while (!m_Window->ShouldClose())
    {
        m_Window->DoEvents();
        OnUpdate();
        OnDraw();
    }
}

void D3D12App::OnUpdate()
{
    tt += 0.01f;
    camPos.x = std::cos(tt);
    camPos.z = std::sin(tt);
    camPos.y = camPos.z;
    auto asp = m_Window->GetAspectRatio();
    g_TransformData.MVP =
        mat4f::right_transform::lookat(camPos, { 0,0,0 }, { 0,1,0 }) *
        mat4f::right_transform::perspective(3.14 * 0.5, asp, 0.1f, 100.f);
    memcpy(m_ConstantBufferBegin, &g_TransformData, sizeof(TransformData));
}

void D3D12App::FlushCommandQueue()
{
    m_CurrentFence ++;
    CHECK_HR(m_CmdQueue->Signal(m_Fence.Get(), m_CurrentFence));
    if (m_Fence->GetCompletedValue() < m_CurrentFence)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
        CHECK_HR(m_Fence->SetEventOnCompletion(m_CurrentFence, eventHandle));
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

void D3D12App::CreateAssets()
{
    float scale = 0.5f;
    std::array<Vertex, 8> vertices =
    {
        Vertex({ {-scale, -scale, -scale}, {0, 0, 0, 1} }),
        Vertex({ {-scale, +scale, -scale}, {0, 0, 1, 1} }),
        Vertex({ {+scale, +scale, -scale}, {0, 1, 0, 1} }),
        Vertex({ {+scale, -scale, -scale}, {0, 1, 1, 1} }),
        Vertex({ {-scale, -scale, +scale}, {1, 0, 0, 1} }),
        Vertex({ {-scale, +scale, +scale}, {1, 0, 1, 1} }),
        Vertex({ {+scale, +scale, +scale}, {1, 1, 0, 1} }),
        Vertex({ {+scale, -scale, +scale}, {1, 1, 1, 1} })
    };

    std::array<std::uint16_t, 36> indices =
    {
        // front face
        0, 1, 2,
        0, 2, 3,
        // back face
        4, 6, 5,
        4, 7, 6,
        // left face
        4, 5, 1,
        4, 1, 0,
        // right face
        3, 2, 6,
        3, 6, 7,
        // top face
        1, 5, 6,
        1, 6, 2,
        // bottom face
        4, 0, 3,
        4, 3, 7
    };
    const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

    // vertex buffer 
    CHECK_HR(m_Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(vbByteSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_VertexBufferGPU.GetAddressOf())));
    CD3DX12_RANGE readRange(0, 0);
    void *pVertexDataBegin;
    m_VertexBufferGPU->Map(NULL, &readRange, reinterpret_cast<void **>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, vertices.data(), vbByteSize);
    m_VertexBufferGPU->Unmap(0, nullptr);

    // index buffer
    CHECK_HR(m_Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(vbByteSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_IndexBufferGPU.GetAddressOf())));
    void *pIndexDataBegin;
    m_IndexBufferGPU->Map(NULL, &readRange, reinterpret_cast<void **>(&pIndexDataBegin));
    memcpy(pIndexDataBegin, indices.data(), ibByteSize);
    m_IndexBufferGPU->Unmap(0, nullptr);

    {
        CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
        CD3DX12_ROOT_PARAMETER1 rootParameters[1];

        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);

        // Allow input layout and deny unecessary access to certain pipeline stages.
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        CHECK_HR(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
        CHECK_HR(m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));
    }

    // Create the pipeline state, which includes compiling and loading shaders.
    {
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
        // Enable better shader debugging with the graphics debugging tools.
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif
        const wchar_t file[] = L"../src/shader/shader.hlsl";
        CHECK_HR(D3DCompileFromFile(file, nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
        CHECK_HR(D3DCompileFromFile(file, nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = m_RootSignature.Get();
        psoDesc.VS = { reinterpret_cast<UINT8 *>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
        psoDesc.PS = { reinterpret_cast<UINT8 *>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        psoDesc.SampleDesc.Count = 1;
        CHECK_HR(m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));

        m_VertexBufferView.BufferLocation = m_VertexBufferGPU->GetGPUVirtualAddress();
        m_VertexBufferView.StrideInBytes = sizeof(Vertex);
        m_VertexBufferView.SizeInBytes = vbByteSize;
        m_IndexBufferView.BufferLocation = m_IndexBufferGPU->GetGPUVirtualAddress();
        m_IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
        m_IndexBufferView.SizeInBytes = ibByteSize;
    }
}
