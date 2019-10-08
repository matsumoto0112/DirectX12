//#include "GPUParticle.h"
//#include "Framework/Define/Config.h"
//#include "Framework/Graphics/DX12/Desc/BlendState.h"
//#include "Framework/Graphics/DX12/Helper.h"
//#include "Framework/Utility/Time.h"
//#include "Framework/ImGUI/ImGUI.h"
//
//// Assign a name to the object to aid with debugging.
//#if defined(_DEBUG) || defined(DBG)
//inline void SetName(ID3D12Object* pObject, LPCWSTR name) {
//    pObject->SetName(name);
//}
//inline void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index) {
//    WCHAR fullName[50];
//    if (swprintf_s(fullName, L"%s[%u]", name, index) > 0) {
//        pObject->SetName(fullName);
//    }
//}
//#else
//inline void SetName(ID3D12Object*, LPCWSTR) { }
//inline void SetNameIndexed(ID3D12Object*, LPCWSTR, UINT) { }
//#endif
//
//#define NAME_D3D12_OBJECT(x) SetName((x).Get(),L#x)
//#define NAME_D3D12_OBJECT_INDEXED(x,n) SetNameIndexed((x)[n].Get(),L#x,n)
//
//namespace {
//using Framework::Graphics::throwIfFailed;
//using namespace Framework;
//using namespace DirectX;
//
//_Use_decl_annotations_
//void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter) {
//    ComPtr<IDXGIAdapter1> adapter;
//    *ppAdapter = nullptr;
//
//    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex) {
//        DXGI_ADAPTER_DESC1 desc;
//        adapter->GetDesc1(&desc);
//
//        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
//            // Don't select the Basic Render Driver adapter.
//            // If you want a software adapter, pass in "/warp" on the command line.
//            continue;
//        }
//
//        // Check to see if the adapter supports Direct3D 12, but don't create the
//        // actual device yet.
//        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
//            break;
//        }
//    }
//
//    *ppAdapter = adapter.Detach();
//}
//}
//
//GPUParticle::GPUParticle(HWND hWnd)
//    :mFrameIndex(0),
//    mCullingScissorRect(),
//    mRTVDescriptorSize(0),
//    mFenceValues{},
//    mHWnd(hWnd){
//
//    mWidth = Framework::Define::Config::getInstance().screenWidth;
//    mHeight = Framework::Define::Config::getInstance().screenHeight;
//
//    mViewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(mWidth), static_cast<float>(mHeight));
//    mScissorRect = CD3DX12_RECT(0.0f, 0.0f, static_cast<LONG>(mWidth), static_cast<LONG>(mHeight));
//}
//
//GPUParticle::~GPUParticle() {
//    end();
//    waitForGPU();
//    CloseHandle(mFenceEvent);
//}
//
//void GPUParticle::load(Framework::Scene::Collecter& collecter) {
//    loadPipeline();
//    loadAssets();
//}
//
//void GPUParticle::update() {
//    ImGui_ImplDX12_NewFrame();
//    ImGui_ImplWin32_NewFrame();
//    ImGui::NewFrame();
//}
//
//bool GPUParticle::isEndScene() const {
//    return false;
//}
//
//void GPUParticle::draw() {
//    populateCommandList();
//    {
//        ID3D12CommandList* list[] = { mComputeCommandList.Get() };
//        mComputeCommandQueue->ExecuteCommandLists(_countof(list), list);
//
//        mComputeCommandQueue->Signal(mComputeFence.Get(), mFenceValues[mFrameIndex]);
//        mCommandQueue->Wait(mComputeFence.Get(), mFenceValues[mFrameIndex]);
//    }
//
//    ID3D12CommandList* list[] = { mCommandList.Get() };
//    mCommandQueue->ExecuteCommandLists(_countof(list), list);
//
//    throwIfFailed(mSwapChain->Present(1, 0));
//    moveToNextFrame();
//}
//
//void GPUParticle::end() {
//    //ImGui_ImplDX12_Shutdown();
//    //ImGui_ImplWin32_Shutdown();
//    //ImGui::DestroyContext();
//}
//
//Framework::Define::SceneType GPUParticle::next() {
//    return Framework::Define::SceneType();
//}
//
//void GPUParticle::loadPipeline() {
//    //デバッグ用インターフェースを先に作成する
//    UINT dxgiFactoryFlags = 0;
//
//#if defined(_DEBUG)
//    {
//        ComPtr<ID3D12Debug> debug;
//        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)))) {
//            debug->EnableDebugLayer();
//            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
//        }
//    }
//#endif
//
//    ComPtr<IDXGIFactory4> factory;
//    throwIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));
//    static const bool useWarpDevice = true;
//    if (useWarpDevice) {
//        ComPtr<IDXGIAdapter> warpAdapter;
//        throwIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
//
//        throwIfFailed(D3D12CreateDevice(
//            warpAdapter.Get(),
//            D3D_FEATURE_LEVEL_11_0,
//            IID_PPV_ARGS(&mDevice)
//        ));
//    }
//    else {
//        ComPtr<IDXGIAdapter1> hardwareAdapter;
//        GetHardwareAdapter(factory.Get(), &hardwareAdapter);
//
//        throwIfFailed(D3D12CreateDevice(
//            hardwareAdapter.Get(),
//            D3D_FEATURE_LEVEL_11_0,
//            IID_PPV_ARGS(&mDevice)
//        ));
//    }
//
//    {
//        //グラフィック用キュー作成
//        D3D12_COMMAND_QUEUE_DESC queueDesc{};
//        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
//        throwIfFailed(mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));
//        NAME_D3D12_OBJECT(mCommandQueue);
//    }
//    {
//        //CP用キュー作成
//        D3D12_COMMAND_QUEUE_DESC computeQueueDesc{};
//        computeQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//        computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
//        throwIfFailed(mDevice->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&mComputeCommandQueue)));
//        NAME_D3D12_OBJECT(mComputeCommandQueue);
//    }
//    {
//        //スワップチェイン作成
//        DXGI_SWAP_CHAIN_DESC1 desc{};
//        desc.BufferCount = FRAME_COUNT;
//        desc.Width = mWidth;
//        desc.Height = mHeight;
//        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
//        desc.SampleDesc.Count = 1;
//
//        ComPtr<IDXGISwapChain1> sc;
//        throwIfFailed(factory->CreateSwapChainForHwnd(
//            mCommandQueue.Get(),
//            mHWnd,
//            &desc,
//            nullptr,
//            nullptr,
//            &sc));
//
//        //フルスクリーン禁止
//        throwIfFailed(factory->MakeWindowAssociation(mHWnd, DXGI_MWA_NO_ALT_ENTER));
//
//        throwIfFailed(sc.As(&mSwapChain));
//        mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
//    }
//
//    {
//        //RTV
//        D3D12_DESCRIPTOR_HEAP_DESC rtv{};
//        rtv.NumDescriptors = FRAME_COUNT;
//        rtv.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
//        rtv.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//        throwIfFailed(mDevice->CreateDescriptorHeap(&rtv, IID_PPV_ARGS(&mRTVHeap)));
//
//        //DSV
//        D3D12_DESCRIPTOR_HEAP_DESC dsv{};
//        dsv.NumDescriptors = 1;
//        dsv.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
//        dsv.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//        throwIfFailed(mDevice->CreateDescriptorHeap(&dsv, IID_PPV_ARGS(&mDSVHeap)));
//
//        //UAV
//        D3D12_DESCRIPTOR_HEAP_DESC desc{};
//        desc.NumDescriptors = 1;
//        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//        throwIfFailed(mDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mParticleUAVHeap)));
//        NAME_D3D12_OBJECT(mParticleUAVHeap);
//
//        mRTVDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//        mParticleUAVDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
//    }
//
//    {
//        //フレームリソース作成
//        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart());
//
//        for (UINT n = 0; n < FRAME_COUNT; n++) {
//            throwIfFailed(mSwapChain->GetBuffer(n, IID_PPV_ARGS(&mRenderTargets[n])));
//            mDevice->CreateRenderTargetView(mRenderTargets[n].Get(), nullptr, rtvHandle);
//            rtvHandle.Offset(1, mRTVDescriptorSize);
//
//            NAME_D3D12_OBJECT_INDEXED(mRenderTargets, n);
//
//            throwIfFailed(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocators[n])));
//        }
//        throwIfFailed(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&mComputeCommandAllocators)));
//    }
//}
//
//void GPUParticle::loadAssets() {
//    {
//        //ルートシグネチャ作成
//        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{};
//        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
//        if (FAILED(mDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)))) {
//            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
//        }
//
//        CD3DX12_ROOT_PARAMETER1 rootParameters[GraphicsRootParametersCount];
//        rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL);
//
//        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
//        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
//
//        ComPtr<ID3DBlob> signature;
//        ComPtr<ID3DBlob> error;
//        throwIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
//        throwIfFailed(mDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&mRootSignature)));
//        NAME_D3D12_OBJECT(mRootSignature);
//
//        //コンピュートシグネチャ作成
//        CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
//        //0番目はUAVのパーティクルデータ
//        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);
//
//        CD3DX12_ROOT_PARAMETER1 computeRootParameters[ComputeRootParametersCount];
//        //UAVテーブルの初期化
//        computeRootParameters[UavTable].InitAsDescriptorTable(1, ranges);
//
//        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc;
//        desc.Init_1_1(_countof(computeRootParameters), computeRootParameters);
//
//        throwIfFailed(D3DX12SerializeVersionedRootSignature(&desc, featureData.HighestVersion, &signature, &error));
//        throwIfFailed(mDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&mComputeRootSignature)));
//        NAME_D3D12_OBJECT(mComputeRootSignature);
//    }
//
//    {
//        ComPtr<ID3DBlob> vertexShader;
//        ComPtr<ID3DBlob> pixelShader;
//        ComPtr<ID3DBlob> computeShader;
//        ComPtr<ID3DBlob> geometoryShader;
//        ComPtr<ID3DBlob> error;
//
//    #if defined(_DEBUG)
//        // Enable better shader debugging with the graphics debugging tools.
//        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
//    #else
//        UINT compileFlags = 0;
//    #endif
//        throwIfFailed(D3DCompileFromFile(L"Resources/Shader/Particle/Particle_VS.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &error));
//        throwIfFailed(D3DCompileFromFile(L"Resources/Shader/Particle/Particle_GS.hlsl", nullptr, nullptr, "GSMain", "gs_5_0", compileFlags, 0, &geometoryShader, &error));
//        throwIfFailed(D3DCompileFromFile(L"Resources/Shader/Particle/Particle_PS.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &error));
//        HRESULT hr = (D3DCompileFromFile(L"Resources/Shader/Particle/Particle.hlsl", nullptr, nullptr, "CSMain", "cs_5_0", compileFlags, 0, &computeShader, &error));
//
//        // エラーチェック.
//        if (FAILED(hr)) {
//            // エラーメッセージを出力.
//            if (error != NULL) {
//                OutputDebugStringA((char*)error->GetBufferPointer());
//            }
//        }
//        D3D12_INPUT_ELEMENT_DESC elem[] =
//        {
//                { "IN_LIFETIME", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//                { "IN_SPEED", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//                { "IN_VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//                { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//        };
//
//        {
//            D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
//            pso.pRootSignature = mRootSignature.Get();
//            pso.InputLayout = { elem,_countof(elem) };
//            pso.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
//            pso.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
//            pso.GS = CD3DX12_SHADER_BYTECODE(geometoryShader.Get());
//            pso.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
//
//            D3D12_BLEND_DESC bd{};
//            bd.AlphaToCoverageEnable = FALSE;
//            bd.IndependentBlendEnable = FALSE;
//            for (int i = 0; i < 8; i++) {
//                bd.RenderTarget[i] = Framework::Graphics::BlendState::addBlendDesc();
//            }
//
//            pso.BlendState = bd;
//            pso.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
//            pso.SampleMask = UINT_MAX;
//            pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
//            pso.NumRenderTargets = 1;
//            pso.RTVFormats[0] = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
//            pso.DSVFormat = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
//            pso.SampleDesc.Count = 1;
//
//            throwIfFailed(mDevice->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&mPipelineState)));
//            NAME_D3D12_OBJECT(mPipelineState);
//        }
//        {
//            D3D12_COMPUTE_PIPELINE_STATE_DESC pso{};
//            pso.pRootSignature = mComputeRootSignature.Get();
//            pso.CS = CD3DX12_SHADER_BYTECODE(computeShader.Get());
//
//            throwIfFailed(mDevice->CreateComputePipelineState(&pso, IID_PPV_ARGS(&mComputePipelineState)));
//            NAME_D3D12_OBJECT(mComputePipelineState);
//        }
//    }
//    //コマンドリスト作成
//    throwIfFailed(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocators[mFrameIndex].Get(), mPipelineState.Get(), IID_PPV_ARGS(&mCommandList)));
//    throwIfFailed(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, mComputeCommandAllocators.Get(), mComputePipelineState.Get(), IID_PPV_ARGS(&mComputeCommandList)));
//    throwIfFailed(mComputeCommandList->Close());
//
//    NAME_D3D12_OBJECT(mCommandList);
//    NAME_D3D12_OBJECT(mComputeCommandList);
//
//    //GPU用データ作成
//    ComPtr<ID3D12Resource> vertexBufferUpload;
//    ComPtr<ID3D12Resource> commandBufferUpload;
//    {
//    }
//    //DSV作成
//    {
//        D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
//        desc.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
//        desc.ViewDimension = D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2D;
//        desc.Flags = D3D12_DSV_FLAGS::D3D12_DSV_FLAG_NONE;
//
//        D3D12_CLEAR_VALUE val{};
//        val.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
//        val.DepthStencil.Depth = 1.0f;
//        val.DepthStencil.Stencil = 0;
//
//        throwIfFailed(mDevice->CreateCommittedResource(
//            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT),
//            D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
//            &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT, mWidth, mHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
//            D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE,
//            &val,
//            IID_PPV_ARGS(&mDepthStencil)));
//
//        NAME_D3D12_OBJECT(mDepthStencil);
//
//        mDevice->CreateDepthStencilView(mDepthStencil.Get(), &desc, mDSVHeap->GetCPUDescriptorHandleForHeapStart());
//    }
//
//    //描画シェーダー用コンスタントバッファ作成
//    {
//        UINT size = sizeof(MVP);
//        throwIfFailed(mDevice->CreateCommittedResource(
//            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
//            D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
//            &CD3DX12_RESOURCE_DESC::Buffer(size),
//            D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
//            nullptr,
//            IID_PPV_ARGS(&mConstantBuffer)));
//        NAME_D3D12_OBJECT(mConstantBuffer);
//
//        Framework::Math::Matrix4x4 world = Framework::Math::Matrix4x4::IDENTITY;
//        Framework::Math::Vector3 eye(0, 0, -10);
//        Framework::Math::Vector3 at(0, 0, 0);
//        Framework::Math::Vector3 up = Framework::Math::Vector3::UP;
//        Framework::Math::Matrix4x4 view = Framework::Math::Matrix4x4::transposition(Framework::Math::Matrix4x4::createView({ eye,at,up }));
//        const float ratio = static_cast<float>(mWidth) / mHeight;
//        Framework::Math::Matrix4x4 proj = Framework::Math::Matrix4x4::transposition(Framework::Math::Matrix4x4::createProjection({ 45.0f, ratio,0.01,20.0f }));
//        mMVPCBufferData.world = Framework::Math::Matrix4x4::transposition(world);
//        mMVPCBufferData.view = Framework::Math::Matrix4x4::transposition(view);
//        mMVPCBufferData.proj = Framework::Math::Matrix4x4::transposition(proj);
//
//        CD3DX12_RANGE range{ 0,0 };
//        UINT8* cbvDataBegin = nullptr;
//        mConstantBuffer->Map(0, &range, reinterpret_cast<void**>(&cbvDataBegin));
//        memcpy(cbvDataBegin, &mMVPCBufferData, sizeof(MVP));
//        mConstantBuffer->Unmap(0, nullptr);
//    }
//
//    ComPtr<ID3D12Resource> mParticleUAVUpload;
//    {
//        std::vector<Particle> particles(PARTICLE_NUM);
//        const UINT size = sizeof(Particle) * PARTICLE_NUM;
//        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);
//
//        throwIfFailed(mDevice->CreateCommittedResource(
//            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT),
//            D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
//            &desc,
//            D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
//            nullptr,
//            IID_PPV_ARGS(&mParticleUAVBuffer)));
//
//        throwIfFailed(mDevice->CreateCommittedResource(
//            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
//            D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
//            &CD3DX12_RESOURCE_DESC::Buffer(size),
//            D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
//            nullptr,
//            IID_PPV_ARGS(&mParticleUAVUpload)));
//        NAME_D3D12_OBJECT(mParticleUAVBuffer);
//
//        D3D12_CPU_DESCRIPTOR_HANDLE  ptr = mParticleUAVHeap->GetCPUDescriptorHandleForHeapStart();
//        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
//        uavDesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
//        uavDesc.ViewDimension = D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_BUFFER;
//        uavDesc.Buffer.FirstElement = 0;
//        uavDesc.Buffer.NumElements = PARTICLE_NUM;
//        uavDesc.Buffer.CounterOffsetInBytes = alignForUavCounter(sizeof(Particle) * PARTICLE_NUM);
//        uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAGS::D3D12_BUFFER_UAV_FLAG_NONE;
//        mDevice->CreateUnorderedAccessView(
//            mParticleUAVBuffer.Get(),
//            mParticleUAVBuffer.Get(),
//            &uavDesc, ptr);
//
//    }
//
//    //コマンドの実行
//    throwIfFailed(mCommandList->Close());
//    ID3D12CommandList* list[] = { mCommandList.Get() };
//    mCommandQueue->ExecuteCommandLists(_countof(list), list);
//
//    {
//        //フェンス作成
//        throwIfFailed(mDevice->CreateFence(mFenceValues[mFrameIndex], D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));
//        throwIfFailed(mDevice->CreateFence(mFenceValues[mFrameIndex], D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mComputeFence)));
//        mFenceValues[mFrameIndex]++;
//
//        mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
//        if (mFenceEvent == nullptr) {
//            throwIfFailed(HRESULT_FROM_WIN32(GetLastError()));
//        }
//        waitForGPU();
//    }
//
//    IMGUI_CHECKVERSION();
//    ImGui::CreateContext();
//    ImGuiIO& io = ImGui::GetIO(); (void)io;
//    ImGui::StyleColorsDark();
//    ImGui_ImplWin32_Init(mHWnd);
//
//    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
//    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//    desc.NumDescriptors = 1;
//    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//    throwIfFailed(mDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mImGUIDescriptorSrvHeap)));
//
//    ImGui_ImplDX12_Init(mDevice.Get(), 2, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
//        mImGUIDescriptorSrvHeap->GetCPUDescriptorHandleForHeapStart(),
//        mImGUIDescriptorSrvHeap->GetGPUDescriptorHandleForHeapStart());
//
//}
//
//float GPUParticle::getRandomFloat(float min, float max) {
//    const float scale = static_cast<float>(rand()) / RAND_MAX;
//    const float range = max - min;
//    return scale * range + min;
//}
//
//void GPUParticle::populateCommandList() {
//    //リセット
//    throwIfFailed(mComputeCommandAllocators->Reset());
//    throwIfFailed(mCommandAllocators[mFrameIndex]->Reset());
//
//    throwIfFailed(mComputeCommandList->Reset(mComputeCommandAllocators.Get(), mComputePipelineState.Get()));
//    throwIfFailed(mCommandList->Reset(mCommandAllocators[mFrameIndex].Get(), mPipelineState.Get()));
//
//
//    //UINT frameOffset = mFrameIndex * CbvSrvUavDescriptorCountPerFrame;
//    //D3D12_GPU_DESCRIPTOR_HANDLE handle = mCBV_SRV_UAV_Heap->GetGPUDescriptorHandleForHeapStart();
//
//    //mComputeCommandList->SetComputeRootSignature(mComputeRootSignature.Get());
//    //ID3D12DescriptorHeap* heap[] = { mCBV_SRV_UAV_Heap.Get() };
//    //mComputeCommandList->SetDescriptorHeaps(_countof(heap), heap);
//
//    //mComputeCommandList->SetComputeRootDescriptorTable(
//    //    SrvUavTable,
//    //    CD3DX12_GPU_DESCRIPTOR_HANDLE(handle, CbvSrvOffset + frameOffset, mCBV_SRV_UAV_DescriptorSize));
//
//    //mComputeCommandList->SetComputeRoot32BitConstants(RootConstants, 4, reinterpret_cast<void*>(&mCSRootConstants), 0);
//
//    //mComputeCommandList->CopyBufferRegion(mProcessedCommandBuffers[mFrameIndex].Get(), COMMAND_BUFFER_COUNTER_OFFSET, mProcessedCommandBufferCounterReset.Get(), 0, sizeof(UINT));
//
//    //D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(mProcessedCommandBuffers[mFrameIndex].Get(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
//    //mComputeCommandList->ResourceBarrier(1, &barrier);
//
//    //mComputeCommandList->Dispatch(static_cast<UINT>(ceil(TRIANGLE_COUNT / float(COMPUTE_THREAD_BLOCK_SIZE))), 1, 1);
//
//    //throwIfFailed(mComputeCommandList->Close());
//
//    //{
//    //    mCommandList->SetGraphicsRootSignature(mRootSignature.Get());
//
//    //    ID3D12DescriptorHeap* heap[] = { mCBV_SRV_UAV_Heap.Get() };
//    //    mCommandList->SetDescriptorHeaps(_countof(heap), heap);
//
//    //    mCommandList->RSSetViewports(1, &mViewport);
//    //    mCommandList->RSSetScissorRects(1, &mCullingScissorRect);
//
//        //D3D12_RESOURCE_BARRIER barriers[2]{
//        //    CD3DX12_RESOURCE_BARRIER::Transition(
//        //       mProcessedCommandBuffers[mFrameIndex].Get() ,
//        //     D3D12_RESOURCE_STATE_UNORDERED_ACCESS ,
//        //        D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT),
//        //    CD3DX12_RESOURCE_BARRIER::Transition(
//        //        mRenderTargets[mFrameIndex].Get(),
//        //        D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT,
//        //        D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET),
//        //};
//
//        //mCommandList->ResourceBarrier(_countof(barriers), barriers);
//
//        //CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), mFrameIndex, mRTVDescriptorSize);
//        //CD3DX12_CPU_DESCRIPTOR_HANDLE dsv(mDSVHeap->GetCPUDescriptorHandleForHeapStart());
//        //mCommandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
//
//        //const float clear[] = { 0.0f,0.0f,0.0f,1.0f };
//        //mCommandList->ClearRenderTargetView(rtv, clear, 0, nullptr);
//        //mCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAGS::D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
//
//        //mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//        //mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
//
//        ////mCommandList->ExecuteIndirect(
//        ////    mCommandSignature.Get(),
//        ////    TRIANGLE_COUNT,
//        ////    mProcessedCommandBuffers[mFrameIndex].Get(),
//        ////    0,
//        ////    mProcessedCommandBuffers[mFrameIndex].Get(), COMMAND_BUFFER_COUNTER_OFFSET);
//
//        //ImGui::Begin("FPS");
//        //ImGui::Text("FPS %.3f", (float)Framework::Utility::Time::getInstance().currentFPS);
//        //ImGui::End();
//
//        //mCommandList->SetDescriptorHeaps(1, mImGUIDescriptorSrvHeap.GetAddressOf());
//        //ImGui::Render();
//        //ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mCommandList.Get());
//
//
//        //barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
//        //barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST;
//
//        //barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET;
//        //barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT;
//
//        //mCommandList->ResourceBarrier(_countof(barriers), barriers);
//
//
//
//
//        //throwIfFailed(mCommandList->Close());
//}
//
//void GPUParticle::waitForGPU() {
//    throwIfFailed(mCommandQueue->Signal(mFence.Get(), mFenceValues[mFrameIndex]));
//
//    throwIfFailed(mFence->SetEventOnCompletion(mFenceValues[mFrameIndex], mFenceEvent));
//    WaitForSingleObjectEx(mFenceEvent, INFINITE, FALSE);
//
//    mFenceValues[mFrameIndex]++;
//}
//
//void GPUParticle::moveToNextFrame() {
//    const UINT64 currentFenceValue = mFenceValues[mFrameIndex];
//    throwIfFailed(mCommandQueue->Signal(mFence.Get(), currentFenceValue));
//
//    mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
//
//    if (mFence->GetCompletedValue() < mFenceValues[mFrameIndex]) {
//        throwIfFailed(mFence->SetEventOnCompletion(mFenceValues[mFrameIndex], mFenceEvent));
//        WaitForSingleObjectEx(mFenceEvent, INFINITE, FALSE);
//    }
//
//    mFenceValues[mFrameIndex] = currentFenceValue + 1;
//}
