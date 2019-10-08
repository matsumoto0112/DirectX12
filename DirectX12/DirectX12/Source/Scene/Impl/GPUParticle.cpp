#include "GPUParticle.h"
#include "Framework/Define/Config.h"
#include "Framework/Graphics/DX12/Desc/BlendState.h"
#include "Framework/Graphics/DX12/Helper.h"
#include "Framework/Utility/Time.h"
#include "Framework/ImGUI/ImGUI.h"

// Assign a name to the object to aid with debugging.
#if defined(_DEBUG) || defined(DBG)
inline void SetName(ID3D12Object* pObject, LPCWSTR name) {
    pObject->SetName(name);
}
inline void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index) {
    WCHAR fullName[50];
    if (swprintf_s(fullName, L"%s[%u]", name, index) > 0) {
        pObject->SetName(fullName);
    }
}
#else
inline void SetName(ID3D12Object*, LPCWSTR) { }
inline void SetNameIndexed(ID3D12Object*, LPCWSTR, UINT) { }
#endif

#define NAME_D3D12_OBJECT(x) SetName((x).Get(),L#x)
#define NAME_D3D12_OBJECT_INDEXED(x,n) SetNameIndexed((x)[n].Get(),L#x,n)


const UINT GPUParticle::COMMAND_SIZE_PER_FRAME = GPUParticle::TRIANGLE_COUNT * sizeof(IndirectCommand);
const UINT GPUParticle::COMMAND_BUFFER_COUNTER_OFFSET = alignForUavCounter(GPUParticle::COMMAND_SIZE_PER_FRAME);
const float GPUParticle::TRIANGLE_HALF_WIDTH = 0.05f;
const float GPUParticle::TRIANGLE_DEPTH = 1.0f;
const float GPUParticle::CULLING_CUT_OFF = 0.5f;

namespace {
using Framework::Graphics::throwIfFailed;
using namespace Framework;
using namespace DirectX;

_Use_decl_annotations_
void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter) {
    ComPtr<IDXGIAdapter1> adapter;
    *ppAdapter = nullptr;

    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex) {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
            // Don't select the Basic Render Driver adapter.
            // If you want a software adapter, pass in "/warp" on the command line.
            continue;
        }

        // Check to see if the adapter supports Direct3D 12, but don't create the
        // actual device yet.
        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
            break;
        }
    }

    *ppAdapter = adapter.Detach();
}
}

GPUParticle::GPUParticle(HWND hWnd)
    :mFrameIndex(0),
    mCullingScissorRect(),
    mRTVDescriptorSize(0),
    mCBV_SRV_UAV_DescriptorSize(0),
    mCSRootConstants(),
    mEnableCulling(false),
    mFenceValues{},
    mHWnd(hWnd){

    mWidth = Framework::Define::Config::getInstance().screenWidth;
    mHeight = Framework::Define::Config::getInstance().screenHeight;

    mViewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(mWidth), static_cast<float>(mHeight));
    mScissorRect = CD3DX12_RECT(0.0f, 0.0f, static_cast<LONG>(mWidth), static_cast<LONG>(mHeight));

    mConstantBufferData.resize(TRIANGLE_COUNT);

    mCSRootConstants.xOffset = TRIANGLE_HALF_WIDTH;
    mCSRootConstants.zOffset = TRIANGLE_DEPTH;
    mCSRootConstants.cullOffset = CULLING_CUT_OFF;
    mCSRootConstants.commandCount = TRIANGLE_COUNT;

    float center = mWidth * 0.5f;
    mCullingScissorRect.left = static_cast<LONG>(center - (center * CULLING_CUT_OFF));
    mCullingScissorRect.right = static_cast<LONG>(center + (center * CULLING_CUT_OFF));
    mCullingScissorRect.bottom = static_cast<LONG>(mHeight);
}

GPUParticle::~GPUParticle() {
    end();
    waitForGPU();
    CloseHandle(mFenceEvent);
}

void GPUParticle::load(Framework::Scene::Collecter& collecter) {
    loadPipeline();
    loadAssets();
}

void GPUParticle::update() {
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    for (UINT n = 0; n < TRIANGLE_COUNT; n++) {
        const float offset = 2.5f;

        mConstantBufferData[n].offset.x += mConstantBufferData[n].velocity.x;
        if (mConstantBufferData[n].offset.x > offset) {
            mConstantBufferData[n].velocity.x = 0.01f;
            mConstantBufferData[n].offset.x = -offset;
        }
    }

    UINT8* dest = mCbvDataBegin + (TRIANGLE_COUNT * mFrameIndex * sizeof(SceneConstantBuffer));
    memcpy(dest, &mConstantBufferData[0], TRIANGLE_COUNT * sizeof(SceneConstantBuffer));
}

bool GPUParticle::isEndScene() const {
    return false;
}

void GPUParticle::draw() {
    populateCommandList();

    if (mEnableCulling) {
        ID3D12CommandList* list[] = { mComputeCommandList.Get() };
        mComputeCommandQueue->ExecuteCommandLists(_countof(list), list);

        mComputeCommandQueue->Signal(mComputeFence.Get(), mFenceValues[mFrameIndex]);
        mCommandQueue->Wait(mComputeFence.Get(), mFenceValues[mFrameIndex]);
    }

    ID3D12CommandList* list[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(list), list);

    throwIfFailed(mSwapChain->Present(1, 0));
    moveToNextFrame();
}

void GPUParticle::end() {
    //ImGui_ImplDX12_Shutdown();
    //ImGui_ImplWin32_Shutdown();
    //ImGui::DestroyContext();
}

Framework::Define::SceneType GPUParticle::next() {
    return Framework::Define::SceneType();
}

void GPUParticle::loadPipeline() {
    //デバッグ用インターフェースを先に作成する
    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    {
        ComPtr<ID3D12Debug> debug;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)))) {
            debug->EnableDebugLayer();
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    ComPtr<IDXGIFactory4> factory;
    throwIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));
    static const bool useWarpDevice = true;
    if (useWarpDevice) {
        ComPtr<IDXGIAdapter> warpAdapter;
        throwIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

        throwIfFailed(D3D12CreateDevice(
            warpAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&mDevice)
        ));
    }
    else {
        ComPtr<IDXGIAdapter1> hardwareAdapter;
        GetHardwareAdapter(factory.Get(), &hardwareAdapter);

        throwIfFailed(D3D12CreateDevice(
            hardwareAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&mDevice)
        ));
    }

    {
        //グラフィック用キュー作成
        D3D12_COMMAND_QUEUE_DESC queueDesc{};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        throwIfFailed(mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));
        NAME_D3D12_OBJECT(mCommandQueue);
    }
    {
        //CP用キュー作成
        D3D12_COMMAND_QUEUE_DESC computeQueueDesc{};
        computeQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        throwIfFailed(mDevice->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&mComputeCommandQueue)));
        NAME_D3D12_OBJECT(mComputeCommandQueue);
    }
    {
        //スワップチェイン作成
        DXGI_SWAP_CHAIN_DESC1 desc{};
        desc.BufferCount = FRAME_COUNT;
        desc.Width = mWidth;
        desc.Height = mHeight;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.SampleDesc.Count = 1;

        ComPtr<IDXGISwapChain1> sc;
        throwIfFailed(factory->CreateSwapChainForHwnd(
            mCommandQueue.Get(),
            mHWnd,
            &desc,
            nullptr,
            nullptr,
            &sc));

        //フルスクリーン禁止
        throwIfFailed(factory->MakeWindowAssociation(mHWnd, DXGI_MWA_NO_ALT_ENTER));

        throwIfFailed(sc.As(&mSwapChain));
        mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
    }

    {
        //RTV
        D3D12_DESCRIPTOR_HEAP_DESC rtv{};
        rtv.NumDescriptors = FRAME_COUNT;
        rtv.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtv.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        throwIfFailed(mDevice->CreateDescriptorHeap(&rtv, IID_PPV_ARGS(&mRTVHeap)));

        //DSV
        D3D12_DESCRIPTOR_HEAP_DESC dsv{};
        dsv.NumDescriptors = 1;
        dsv.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsv.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        throwIfFailed(mDevice->CreateDescriptorHeap(&dsv, IID_PPV_ARGS(&mDSVHeap)));

        //CBV,SRV,UAV
        D3D12_DESCRIPTOR_HEAP_DESC desc{};
        desc.NumDescriptors = CbvSrvUavDescriptorCountPerFrame * FRAME_COUNT;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        throwIfFailed(mDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mCBV_SRV_UAV_Heap)));
        NAME_D3D12_OBJECT(mCBV_SRV_UAV_Heap);

        mRTVDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        mCBV_SRV_UAV_DescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    {
        //フレームリソース作成
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart());

        for (UINT n = 0; n < FRAME_COUNT; n++) {
            throwIfFailed(mSwapChain->GetBuffer(n, IID_PPV_ARGS(&mRenderTargets[n])));
            mDevice->CreateRenderTargetView(mRenderTargets[n].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, mRTVDescriptorSize);

            NAME_D3D12_OBJECT_INDEXED(mRenderTargets, n);

            throwIfFailed(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocators[n])));
            throwIfFailed(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&mComputeCommandAllocators[n])));
        }
    }
}

void GPUParticle::loadAssets() {
    {
        //ルートシグネチャ作成
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{};
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
        if (FAILED(mDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)))) {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        CD3DX12_ROOT_PARAMETER1 rootParameters[GraphicsRootParametersCount];
        rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_VERTEX);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        throwIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
        throwIfFailed(mDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&mRootSignature)));
        NAME_D3D12_OBJECT(mRootSignature);

        //コンピュートシグネチャ作成
        CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

        CD3DX12_ROOT_PARAMETER1 computeRootParameters[ComputeRootParametersCount];
        computeRootParameters[SrvUavTable].InitAsDescriptorTable(2, ranges);
        computeRootParameters[RootConstants].InitAsConstants(4, 0);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc;
        desc.Init_1_1(_countof(computeRootParameters), computeRootParameters);

        throwIfFailed(D3DX12SerializeVersionedRootSignature(&desc, featureData.HighestVersion, &signature, &error));
        throwIfFailed(mDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&mComputeRootSignature)));
        NAME_D3D12_OBJECT(mComputeRootSignature);
    }

    {
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;
        ComPtr<ID3DBlob> computeShader;
        ComPtr<ID3DBlob> error;

    #if defined(_DEBUG)
        // Enable better shader debugging with the graphics debugging tools.
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
    #else
        UINT compileFlags = 0;
    #endif
        throwIfFailed(D3DCompileFromFile(L"Resources/Shader/shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &error));
        throwIfFailed(D3DCompileFromFile(L"Resources/Shader/shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &error));
        throwIfFailed(D3DCompileFromFile(L"Resources/Shader/compute.hlsl", nullptr, nullptr, "CSMain", "cs_5_0", compileFlags, 0, &computeShader, &error));

        D3D12_INPUT_ELEMENT_DESC elem[] =
        {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };

        {
            D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
            pso.pRootSignature = mRootSignature.Get();
            pso.InputLayout = { elem,_countof(elem) };
            pso.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
            pso.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
            pso.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

            D3D12_BLEND_DESC bd{};
            bd.AlphaToCoverageEnable = FALSE;
            bd.IndependentBlendEnable = FALSE;
            for (int i = 0; i < 8; i++) {
                bd.RenderTarget[i] = Framework::Graphics::BlendState::addBlendDesc();
            }

            pso.BlendState = bd;
            pso.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
            pso.SampleMask = UINT_MAX;
            pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            pso.NumRenderTargets = 1;
            pso.RTVFormats[0] = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
            pso.DSVFormat = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
            pso.SampleDesc.Count = 1;

            throwIfFailed(mDevice->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&mPipelineState)));
            NAME_D3D12_OBJECT(mPipelineState);
        }
        {
            D3D12_COMPUTE_PIPELINE_STATE_DESC pso{};
            pso.pRootSignature = mComputeRootSignature.Get();
            pso.CS = CD3DX12_SHADER_BYTECODE(computeShader.Get());

            throwIfFailed(mDevice->CreateComputePipelineState(&pso, IID_PPV_ARGS(&mComputeState)));
            NAME_D3D12_OBJECT(mComputeState);
        }
    }
    //コマンドリスト作成
    throwIfFailed(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocators[mFrameIndex].Get(), mPipelineState.Get(), IID_PPV_ARGS(&mCommandList)));
    throwIfFailed(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, mComputeCommandAllocators[mFrameIndex].Get(), mComputeState.Get(), IID_PPV_ARGS(&mComputeCommandList)));
    throwIfFailed(mComputeCommandList->Close());

    NAME_D3D12_OBJECT(mCommandList);
    NAME_D3D12_OBJECT(mComputeCommandList);

    //GPU用データ作成
    ComPtr<ID3D12Resource> vertexBufferUpload;
    ComPtr<ID3D12Resource> commandBufferUpload;
    {
        //頂点データ作成
        Vertex triangleVertices[] =
        {
            { { 0.0f, TRIANGLE_HALF_WIDTH, TRIANGLE_DEPTH } },
            { { TRIANGLE_HALF_WIDTH, -TRIANGLE_HALF_WIDTH, TRIANGLE_DEPTH } },
            { { -TRIANGLE_HALF_WIDTH, -TRIANGLE_HALF_WIDTH, TRIANGLE_DEPTH } }
        };

        const UINT vertexBufferSize = sizeof(triangleVertices);

        throwIfFailed(mDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
            D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&mVertexBuffer)));

        throwIfFailed(mDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
            D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&vertexBufferUpload)));

        NAME_D3D12_OBJECT(mVertexBuffer);

        //データのコピー
        D3D12_SUBRESOURCE_DATA data{};
        data.pData = reinterpret_cast<UINT8*>(triangleVertices);
        data.RowPitch = vertexBufferSize;
        data.SlicePitch = data.RowPitch;

        UpdateSubresources<1>(mCommandList.Get(), mVertexBuffer.Get(), vertexBufferUpload.Get(), 0, 0, 1, &data);
        mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mVertexBuffer.Get(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

        //バッファビュー初期化
        mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
        mVertexBufferView.StrideInBytes = sizeof(Vertex);
        mVertexBufferView.SizeInBytes = sizeof(triangleVertices);
    }
    {
        //DSV作成
        D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
        desc.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
        desc.ViewDimension = D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2D;
        desc.Flags = D3D12_DSV_FLAGS::D3D12_DSV_FLAG_NONE;

        D3D12_CLEAR_VALUE val{};
        val.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
        val.DepthStencil.Depth = 1.0f;
        val.DepthStencil.Stencil = 0;

        throwIfFailed(mDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT, mWidth, mHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
            D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &val,
            IID_PPV_ARGS(&mDepthStencil)));

        NAME_D3D12_OBJECT(mDepthStencil);

        mDevice->CreateDepthStencilView(mDepthStencil.Get(), &desc, mDSVHeap->GetCPUDescriptorHandleForHeapStart());
    }

    {
        //コンスタントバッファ作成
        const UINT size = TRIANGLE_RESOURCE_COUNT * sizeof(SceneConstantBuffer);

        throwIfFailed(mDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(size),
            D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&mConstantBuffer)));

        NAME_D3D12_OBJECT(mConstantBuffer);

        Framework::Math::Vector3 eye(0, 0, -10);
        Framework::Math::Vector3 at(0, 0, 0);
        Framework::Math::Vector3 up = Framework::Math::Vector3::UP;
        Framework::Math::Matrix4x4 view = Framework::Math::Matrix4x4::transposition(Framework::Math::Matrix4x4::createView({ eye,at,up }));
        const float ratio = static_cast<float>(mWidth) / mHeight;
        Framework::Math::Matrix4x4 proj = Framework::Math::Matrix4x4::transposition(Framework::Math::Matrix4x4::createProjection({ 45.0f, ratio,0.01,20.0f }));

        //データ初期化
        for (UINT n = 0; n < TRIANGLE_COUNT; n++) {
            mConstantBufferData[n].velocity = Framework::Math::Vector4(getRandomFloat(0.01f, 0.02f), 0.0f, 0.0f);
            mConstantBufferData[n].offset = Framework::Math::Vector4(getRandomFloat(-5.0f, -1.5f), getRandomFloat(-1.0f, 1.0f), getRandomFloat(0.0f, 2.0f), 0.0f);
            mConstantBufferData[n].color = Framework::Graphics::Color4(0.0f, 1.0f, 1.0f, 0.5f);
            mConstantBufferData[n].view = view;
            mConstantBufferData[n].projection = proj;
        }

        //コンスタントバッファデータ初期化
        CD3DX12_RANGE range(0, 0);
        throwIfFailed(mConstantBuffer->Map(0, &range, reinterpret_cast<void**>(&mCbvDataBegin)));
        memcpy(mCbvDataBegin, &mConstantBufferData[0], TRIANGLE_COUNT * sizeof(SceneConstantBuffer));

        //SRV作成
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Buffer.NumElements = TRIANGLE_COUNT;
        srvDesc.Buffer.StructureByteStride = sizeof(SceneConstantBuffer);
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAGS::D3D12_BUFFER_SRV_FLAG_NONE;

        CD3DX12_CPU_DESCRIPTOR_HANDLE ptr(mCBV_SRV_UAV_Heap->GetCPUDescriptorHandleForHeapStart(), static_cast<UINT>(HeapOffsets::CbvSrvOffset), mCBV_SRV_UAV_DescriptorSize);
        for (UINT frame = 0; frame < FRAME_COUNT; frame++) {
            srvDesc.Buffer.FirstElement = frame * TRIANGLE_COUNT;
            mDevice->CreateShaderResourceView(mConstantBuffer.Get(), &srvDesc, ptr);
            ptr.Offset(static_cast<UINT>(HeapOffsets::CbvSrvUavDescriptorCountPerFrame), mCBV_SRV_UAV_DescriptorSize);
        }
    }

    {
        //コンピュートシグネチャ作成
        D3D12_INDIRECT_ARGUMENT_DESC argumentDescs[2] = {};
        argumentDescs[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE::D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW;
        argumentDescs[0].ConstantBufferView.RootParameterIndex = GraphicsRootParameter::Cbv;
        argumentDescs[1].Type = D3D12_INDIRECT_ARGUMENT_TYPE::D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;

        D3D12_COMMAND_SIGNATURE_DESC comDesc{};
        comDesc.pArgumentDescs = argumentDescs;
        comDesc.NumArgumentDescs = _countof(argumentDescs);
        comDesc.ByteStride = sizeof(IndirectCommand);

        throwIfFailed(mDevice->CreateCommandSignature(&comDesc, mRootSignature.Get(), IID_PPV_ARGS(&mCommandSignature)));
        NAME_D3D12_OBJECT(mCommandSignature);
    }

    {
        //UAV作成
        std::vector<IndirectCommand> commands(TRIANGLE_RESOURCE_COUNT);
        const UINT size = COMMAND_SIZE_PER_FRAME * FRAME_COUNT;
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);

        throwIfFailed(mDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&mCommandBuffer)));

        throwIfFailed(mDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(size),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&commandBufferUpload)));

        NAME_D3D12_OBJECT(mCommandBuffer);

        D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = mConstantBuffer->GetGPUVirtualAddress();
        UINT commandIndex = 0;
        for (UINT frame = 0; frame < FRAME_COUNT; frame++) {
            for (UINT n = 0; n < TRIANGLE_COUNT; n++) {
                commands[commandIndex].cbv = gpuAddress;
                commands[commandIndex].drawArguments.VertexCountPerInstance = 3;
                commands[commandIndex].drawArguments.InstanceCount = 1;
                commands[commandIndex].drawArguments.StartVertexLocation = 0;
                commands[commandIndex].drawArguments.StartInstanceLocation = 0;

                commandIndex++;
                gpuAddress += sizeof(SceneConstantBuffer);
            }
        }

        D3D12_SUBRESOURCE_DATA commandData{};
        commandData.pData = reinterpret_cast<UINT8*>(&commands[0]);
        commandData.RowPitch = size;
        commandData.SlicePitch = commandData.RowPitch;

        UpdateSubresources<1>(mCommandList.Get(), mCommandBuffer.Get(), commandBufferUpload.Get(), 0, 0, 1, &commandData);
        mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mCommandBuffer.Get(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));

        //コンピュートシェーダー用SRV
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Buffer.NumElements = TRIANGLE_COUNT;
        srvDesc.Buffer.StructureByteStride = sizeof(IndirectCommand);
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

        CD3DX12_CPU_DESCRIPTOR_HANDLE handle(mCBV_SRV_UAV_Heap->GetCPUDescriptorHandleForHeapStart(), CommandsOffset, mCBV_SRV_UAV_DescriptorSize);
        for (UINT frame = 0; frame < FRAME_COUNT; frame++) {
            srvDesc.Buffer.FirstElement = frame * TRIANGLE_COUNT;
            mDevice->CreateShaderResourceView(mCommandBuffer.Get(), &srvDesc, handle);
            handle.Offset(static_cast<UINT>(HeapOffsets::CbvSrvUavDescriptorCountPerFrame), mCBV_SRV_UAV_DescriptorSize);
        }

        //コンピュートシェーダー用UAV
        CD3DX12_CPU_DESCRIPTOR_HANDLE uavPtr(mCBV_SRV_UAV_Heap->GetCPUDescriptorHandleForHeapStart(), ProcessedCommandsOffset, mCBV_SRV_UAV_DescriptorSize);
        for (UINT f = 0; f < FRAME_COUNT; f++) {
            desc = CD3DX12_RESOURCE_DESC::Buffer(COMMAND_BUFFER_COUNTER_OFFSET + sizeof(UINT), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
            throwIfFailed(mDevice->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
                &desc,
                D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST,
                nullptr,
                IID_PPV_ARGS(&mProcessedCommandBuffers[f])));
            NAME_D3D12_OBJECT_INDEXED(mProcessedCommandBuffers, f);

            D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
            uavDesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_BUFFER;
            uavDesc.Buffer.FirstElement = 0;
            uavDesc.Buffer.NumElements = TRIANGLE_COUNT;
            uavDesc.Buffer.StructureByteStride = sizeof(IndirectCommand);
            uavDesc.Buffer.CounterOffsetInBytes = COMMAND_BUFFER_COUNTER_OFFSET;
            uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAGS::D3D12_BUFFER_UAV_FLAG_NONE;

            mDevice->CreateUnorderedAccessView(
                mProcessedCommandBuffers[f].Get(),
                mProcessedCommandBuffers[f].Get(),
                &uavDesc,
                uavPtr);

            uavPtr.Offset(static_cast<UINT>(HeapOffsets::CbvSrvUavDescriptorCountPerFrame), mCBV_SRV_UAV_DescriptorSize);
        }

        //リソースコピー
        throwIfFailed(mDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT)),
            D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&mProcessedCommandBufferCounterReset)));

        UINT8* map = nullptr;
        CD3DX12_RANGE range(0, 0);
        throwIfFailed(mProcessedCommandBufferCounterReset->Map(0, &range, reinterpret_cast<void**>(&map)));
        ZeroMemory(map, sizeof(UINT));
        mProcessedCommandBufferCounterReset->Unmap(0, nullptr);
    }

    //コマンドの実行
    throwIfFailed(mCommandList->Close());
    ID3D12CommandList* list[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(list), list);

    {
        //フェンス作成
        throwIfFailed(mDevice->CreateFence(mFenceValues[mFrameIndex], D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));
        throwIfFailed(mDevice->CreateFence(mFenceValues[mFrameIndex], D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mComputeFence)));
        mFenceValues[mFrameIndex]++;

        mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (mFenceEvent == nullptr) {
            throwIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
        waitForGPU();
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(mHWnd);

    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.NumDescriptors = 1;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    throwIfFailed(mDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mImGUIDescriptorSrvHeap)));

    ImGui_ImplDX12_Init(mDevice.Get(), 2, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
        mImGUIDescriptorSrvHeap->GetCPUDescriptorHandleForHeapStart(),
        mImGUIDescriptorSrvHeap->GetGPUDescriptorHandleForHeapStart());

}

float GPUParticle::getRandomFloat(float min, float max) {
    const float scale = static_cast<float>(rand()) / RAND_MAX;
    const float range = max - min;
    return scale * range + min;
}

void GPUParticle::populateCommandList() {
    //リセット
    throwIfFailed(mComputeCommandAllocators[mFrameIndex]->Reset());
    throwIfFailed(mCommandAllocators[mFrameIndex]->Reset());

    throwIfFailed(mComputeCommandList->Reset(mComputeCommandAllocators[mFrameIndex].Get(), mComputeState.Get()));
    throwIfFailed(mCommandList->Reset(mCommandAllocators[mFrameIndex].Get(), mPipelineState.Get()));


    if (mEnableCulling) {
        UINT frameOffset = mFrameIndex * CbvSrvUavDescriptorCountPerFrame;
        D3D12_GPU_DESCRIPTOR_HANDLE handle = mCBV_SRV_UAV_Heap->GetGPUDescriptorHandleForHeapStart();

        mComputeCommandList->SetComputeRootSignature(mComputeRootSignature.Get());
        ID3D12DescriptorHeap* heap[] = { mCBV_SRV_UAV_Heap.Get() };
        mComputeCommandList->SetDescriptorHeaps(_countof(heap), heap);

        mComputeCommandList->SetComputeRootDescriptorTable(
            SrvUavTable,
            CD3DX12_GPU_DESCRIPTOR_HANDLE(handle, CbvSrvOffset + frameOffset, mCBV_SRV_UAV_DescriptorSize));

        mComputeCommandList->SetComputeRoot32BitConstants(RootConstants, 4, reinterpret_cast<void*>(&mCSRootConstants), 0);

        mComputeCommandList->CopyBufferRegion(mProcessedCommandBuffers[mFrameIndex].Get(), COMMAND_BUFFER_COUNTER_OFFSET, mProcessedCommandBufferCounterReset.Get(), 0, sizeof(UINT));

        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(mProcessedCommandBuffers[mFrameIndex].Get(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        mComputeCommandList->ResourceBarrier(1, &barrier);

        mComputeCommandList->Dispatch(static_cast<UINT>(ceil(TRIANGLE_COUNT / float(COMPUTE_THREAD_BLOCK_SIZE))), 1, 1);
    }

    throwIfFailed(mComputeCommandList->Close());

    {
        mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

        ID3D12DescriptorHeap* heap[] = { mCBV_SRV_UAV_Heap.Get() };
        mCommandList->SetDescriptorHeaps(_countof(heap), heap);

        mCommandList->RSSetViewports(1, &mViewport);
        mCommandList->RSSetScissorRects(1, mEnableCulling ? &mCullingScissorRect : &mScissorRect);

        D3D12_RESOURCE_BARRIER barriers[2]{
            CD3DX12_RESOURCE_BARRIER::Transition(
                mEnableCulling ? mProcessedCommandBuffers[mFrameIndex].Get() : mCommandBuffer.Get(),
                mEnableCulling ? D3D12_RESOURCE_STATE_UNORDERED_ACCESS : D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
                D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT),
            CD3DX12_RESOURCE_BARRIER::Transition(
                mRenderTargets[mFrameIndex].Get(),
                D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT,
                D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET),
        };

        mCommandList->ResourceBarrier(_countof(barriers), barriers);

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), mFrameIndex, mRTVDescriptorSize);
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsv(mDSVHeap->GetCPUDescriptorHandleForHeapStart());
        mCommandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

        const float clear[] = { 0.0f,0.0f,0.0f,1.0f };
        mCommandList->ClearRenderTargetView(rtv, clear, 0, nullptr);
        mCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAGS::D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferView);

        if (mEnableCulling) {
            mCommandList->ExecuteIndirect(
                mCommandSignature.Get(),
                TRIANGLE_COUNT,
                mProcessedCommandBuffers[mFrameIndex].Get(),
                0,
                mProcessedCommandBuffers[mFrameIndex].Get(),
                COMMAND_BUFFER_COUNTER_OFFSET);
        }
        else {
            mCommandList->ExecuteIndirect(
                mCommandSignature.Get(),
                TRIANGLE_COUNT,
                mCommandBuffer.Get(),
                COMMAND_SIZE_PER_FRAME * mFrameIndex,
                nullptr,
                0);
        }
        ImGui::Begin("FPS");
        ImGui::Text("FPS %.3f", (float)Framework::Utility::Time::getInstance().currentFPS);
        ImGui::End();

        mCommandList->SetDescriptorHeaps(1, mImGUIDescriptorSrvHeap.GetAddressOf());
        ImGui::Render();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mCommandList.Get());


        barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
        barriers[0].Transition.StateAfter = mEnableCulling ? D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST : D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

        barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET;
        barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT;

        mCommandList->ResourceBarrier(_countof(barriers), barriers);




        throwIfFailed(mCommandList->Close());
    }
}

void GPUParticle::waitForGPU() {
    throwIfFailed(mCommandQueue->Signal(mFence.Get(), mFenceValues[mFrameIndex]));

    throwIfFailed(mFence->SetEventOnCompletion(mFenceValues[mFrameIndex], mFenceEvent));
    WaitForSingleObjectEx(mFenceEvent, INFINITE, FALSE);

    mFenceValues[mFrameIndex]++;
}

void GPUParticle::moveToNextFrame() {
    const UINT64 currentFenceValue = mFenceValues[mFrameIndex];
    throwIfFailed(mCommandQueue->Signal(mFence.Get(), currentFenceValue));

    mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

    if (mFence->GetCompletedValue() < mFenceValues[mFrameIndex]) {
        throwIfFailed(mFence->SetEventOnCompletion(mFenceValues[mFrameIndex], mFenceEvent));
        WaitForSingleObjectEx(mFenceEvent, INFINITE, FALSE);
    }

    mFenceValues[mFrameIndex] = currentFenceValue + 1;
}
