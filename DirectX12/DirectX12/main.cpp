//メモリリーク検出
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h> 
#include <crtdbg.h>  

#include <Windows.h>
#include <tchar.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3d12shader.h>

#pragma comment(linker,"/SUBSYSTEM:WINDOWS")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")

#include "Framework/Define/Config.h"
#include "Framework/Define/Path.h"
#include "Framework/Device/GameDevice.h"
#include "Framework/Game.h"
#include "Framework/Graphics/Color4.h"
#include "Framework/Math/Vector3.h"
#include "Framework/Math/Vector4.h"
#include "Framework/Utility/IO/ByteReader.h"
#include "Framework/Window/Window.h"
#include "Framework/Window/Procedure/DestroyProc.h"
#include "Framework/Window/Procedure/CloseProc.h"
#include "Framework/Utility/IO/TextureLoader.h"
#include "Framework/Graphics/DX12/Helper.h"
#include "Framework/Math/Matrix4x4.h"

namespace {
using namespace Framework::Graphics;
struct Vertex {
    Framework::Math::Vector4 pos;
    Framework::Math::Vector2 uv;
};

struct ColorBuffer {
    Framework::Graphics::Color4 color;
};

struct MVP {
    Framework::Math::Matrix4x4 world;
    Framework::Math::Matrix4x4 view;
    Framework::Math::Matrix4x4 proj;
};

void getHardwareAdapter(IDXGIFactory2* factory, IDXGIAdapter1** ppAdapter) {
    ComPtr<IDXGIAdapter1> adapter = nullptr;
    *ppAdapter = nullptr;

    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapterIndex, &adapter); adapterIndex++) {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG::DXGI_ADAPTER_FLAG_SOFTWARE) {
            continue;
        }

        if (SUCCEEDED(D3D12CreateDevice(
            adapter.Get(),
            D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0,
            __uuidof(ID3D12Device),
            nullptr))) {
            break;
        }
    }

    *ppAdapter = adapter.Detach();
}

D3D12_HEAP_PROPERTIES PROPERTY(D3D12_HEAP_TYPE type) {
    D3D12_HEAP_PROPERTIES prop{};
    prop.Type = type;
    prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    prop.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
    prop.CreationNodeMask = 1;
    prop.VisibleNodeMask = 1;
    return prop;
}

D3D12_RESOURCE_DESC RESOURCE(UINT size) {
    D3D12_RESOURCE_DESC res{};
    res.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
    res.Alignment = 0;
    res.Width = size;
    res.Height = 1;
    res.DepthOrArraySize = 1;
    res.MipLevels = 1;
    res.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
    res.SampleDesc.Count = 1;
    res.SampleDesc.Quality = 0;
    res.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    res.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
    return res;
}

D3D12_RESOURCE_BARRIER BARRIER(ID3D12Resource* resource,
    D3D12_RESOURCE_STATES before,
    D3D12_RESOURCE_STATES after,
    UINT subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
    D3D12_RESOURCE_BARRIER_FLAGS flag = D3D12_RESOURCE_BARRIER_FLAGS::D3D12_RESOURCE_BARRIER_FLAG_NONE) {
    D3D12_RESOURCE_BARRIER result{};
    result.Type = D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    result.Flags = flag;
    result.Transition.pResource = resource;
    result.Transition.StateBefore = before;
    result.Transition.StateAfter = after;
    result.Transition.Subresource = subResource;
    return result;
}

UINT64 getRequiredIntermediateSize(ID3D12Resource* resource, UINT firstSubresource, UINT numSubresource) {
    D3D12_RESOURCE_DESC desc = resource->GetDesc();
    UINT64 requiredSize = 0;

    ID3D12Device* device = nullptr;
    resource->GetDevice(__uuidof(*device), reinterpret_cast<void**>(&device));
    device->GetCopyableFootprints(&desc, firstSubresource, numSubresource, 0, nullptr, nullptr, nullptr, &requiredSize);
    device->Release();

    return requiredSize;
}

std::vector<UINT8> generate(UINT width, UINT height, UINT texPixelSize) {
    const UINT textureSize = width * height * texPixelSize;
    std::vector<UINT8> data(textureSize);
    for (UINT n = 0; n < textureSize; n += texPixelSize) {
        data[n + 0] = 0.0f;
        data[n + 1] = Framework::Math::MathUtil::lerp(0, 0xff, static_cast<float>(n) / static_cast<float>(textureSize));
        data[n + 2] = 0x00;
        data[n + 3] = 0xff;
    }
    return data;
}

void memcpySubResource(
    const D3D12_MEMCPY_DEST* dest,
    const D3D12_SUBRESOURCE_DATA* src,
    SIZE_T rowSizeInBytes,
    UINT numRows,
    UINT numSlices) {
    for (UINT z = 0; z < numSlices; z++) {
        BYTE* destSlice = reinterpret_cast<BYTE*>(dest->pData) + dest->SlicePitch * z;
        const BYTE* srcSlice = reinterpret_cast<const BYTE*>(src->pData) + src->SlicePitch * z;
        for (UINT y = 0; y < numRows; y++) {
            memcpy(destSlice + dest->RowPitch * y,
                srcSlice + src->RowPitch * y,
                rowSizeInBytes);
        }
    }
}

UINT64 updateSubresource(ID3D12GraphicsCommandList* list,
    ID3D12Resource* resource,
    ID3D12Resource* intermediate,
    UINT firstSubresource,
    UINT numSubresource,
    UINT requiredSize,
    const D3D12_PLACED_SUBRESOURCE_FOOTPRINT* layouts,
    const UINT* numRows,
    const UINT64* rowSizesInBytes,
    const D3D12_SUBRESOURCE_DATA* srcData) {
    D3D12_RESOURCE_DESC imdDesc = intermediate->GetDesc();
    D3D12_RESOURCE_DESC dstDesc = resource->GetDesc();
    if (imdDesc.Dimension != D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER ||
        imdDesc.Width < requiredSize + layouts[0].Offset ||
        requiredSize > SIZE_T(-1) ||
        (dstDesc.Dimension == D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER &&
        (firstSubresource != 0 || numSubresource != 1))) {
        return 0;
    }

    BYTE* data;
    HRESULT hr = intermediate->Map(0, nullptr, reinterpret_cast<void**>(&data));
    if (FAILED(hr))return 0;

    for (UINT i = 0; i < numSubresource; i++) {
        if (rowSizesInBytes[i] > SIZE_T(-1))return 0;
        D3D12_MEMCPY_DEST destData = { data + layouts[i].Offset ,layouts[i].Footprint.RowPitch,SIZE_T(layouts[i].Footprint.RowPitch) * SIZE_T(numRows[i]) };
        memcpySubResource(&destData, &srcData[i], static_cast<SIZE_T>(rowSizesInBytes[i]), numRows[i], layouts[i].Footprint.Depth);
    }

    intermediate->Unmap(0, nullptr);

    if (dstDesc.Dimension == D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER) {
        list->CopyBufferRegion(
            resource, 0, intermediate, layouts[0].Offset, layouts[0].Footprint.Width);
    }
    else {
        for (UINT i = 0; i < numSubresource; i++) {
            D3D12_TEXTURE_COPY_LOCATION dst{};
            dst.pResource = resource;
            dst.Type = D3D12_TEXTURE_COPY_TYPE::D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            dst.SubresourceIndex = i + firstSubresource;
            D3D12_TEXTURE_COPY_LOCATION src{};
            src.pResource = intermediate;
            src.Type = D3D12_TEXTURE_COPY_TYPE::D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            src.PlacedFootprint = layouts[i];
            list->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
        }
    }
    return requiredSize;
}

UINT64 updateSubresource(ID3D12GraphicsCommandList* list,
    ID3D12Resource* resource,
    ID3D12Resource* intermediate,
    UINT intermediateOffset,
    UINT firstSubresource,
    UINT numSubresources,
    D3D12_SUBRESOURCE_DATA* srcData) {
    UINT64 requiredSize = 0;
    UINT64 memToAlloc = static_cast<UINT64>(sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(UINT) + sizeof(UINT64))* numSubresources;
    if (memToAlloc > SIZE_MAX) {
        return 0;
    }

    void* mem = HeapAlloc(GetProcessHeap(), 0, static_cast<SIZE_T>(memToAlloc));
    if (mem == nullptr)return 0;

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT* layouts = reinterpret_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(mem);
    UINT64* rowSizesInBytes = reinterpret_cast<UINT64*>(layouts + numSubresources);
    UINT* numRows = reinterpret_cast<UINT*>(rowSizesInBytes + numSubresources);

    D3D12_RESOURCE_DESC desc = resource->GetDesc();
    ID3D12Device* device;
    resource->GetDevice(__uuidof(*device), reinterpret_cast<void**>(&device));
    device->GetCopyableFootprints(&desc, firstSubresource, numSubresources, intermediateOffset, layouts, numRows, rowSizesInBytes, &requiredSize);
    device->Release();
    UINT64 res = updateSubresource(list, resource, intermediate, firstSubresource, numSubresources, requiredSize, layouts, numRows, rowSizesInBytes, srcData);
    HeapFree(GetProcessHeap(), 0, mem);
    return res;
}
}

/**
* @class MyGame
*/
class MyGame :public Framework::Game {
public:
    /**
    * @brief コンストラクタ
    */
    MyGame(const std::string& title, HINSTANCE hInstance) :Framework::Game(title, hInstance) { }
    /**
    * @brief デストラクタ
    */
    ~MyGame() { }
    virtual bool init() override {
        using namespace Framework::Graphics;
        if (!Game::init())return false;
        using namespace Framework::Window;
        Window* window = Framework::Device::GameDevice::getInstance().getWindow();
        window->addProcedureEvent(new DestroyProc());
        window->addProcedureEvent(new CloseProc());

        mColorBuffer.color = Framework::Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f);

#pragma region INIT_PIPELINE
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

        //ファクトリの生成
        ComPtr<IDXGIFactory4> factory;
        throwIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));
        static constexpr bool useWrapDevice = false;
        if (useWrapDevice) {
            ComPtr<IDXGIAdapter> adapter = nullptr;
            throwIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)));
            throwIfFailed(D3D12CreateDevice(
                adapter.Get(),
                D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0,
                IID_PPV_ARGS(&mDevice)));
        }
        else {
            ComPtr<IDXGIAdapter1> adapter;
            getHardwareAdapter(factory.Get(), &adapter);
            throwIfFailed(D3D12CreateDevice(
                adapter.Get(),
                D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0,
                IID_PPV_ARGS(&mDevice)));
        }

        //キュー作成
        D3D12_COMMAND_QUEUE_DESC queueDesc{};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;

        throwIfFailed(mDevice->CreateCommandQueue(
            &queueDesc,
            IID_PPV_ARGS(&mCommandQueue)));

        const int width = Framework::Define::Config::getInstance().screenWidth;
        const int height = Framework::Define::Config::getInstance().screenHeight;
        //スワップチェイン作成
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
        swapChainDesc.BufferCount = FRAME_COUNT;
        swapChainDesc.Width = width;
        swapChainDesc.Height = height;
        swapChainDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SampleDesc.Count = 1;

        ComPtr<IDXGISwapChain1> swapChain;
        throwIfFailed(factory->CreateSwapChainForHwnd(
            mCommandQueue.Get(),
            window->getHWND(),
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain));
        throwIfFailed(factory->MakeWindowAssociation(window->getHWND(), DXGI_MWA_NO_ALT_ENTER));
        throwIfFailed(swapChain.As(&mSwapChain));
        mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

        {
            //RTV
            D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
            rtvHeapDesc.NumDescriptors = FRAME_COUNT;
            rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            throwIfFailed(mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRTVHeap)));

            //SRV
            D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
            srvHeapDesc.NumDescriptors = 1;
            srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            throwIfFailed(mDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSRVHeap)));

            mRTVDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

            //CBV
            D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc{};
            cbvHeapDesc.NumDescriptors = 1;
            cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            throwIfFailed(mDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCBVHeap)));

            throwIfFailed(mDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mMatrixCBVHeap)));
        }

        {
            //RTV作成
            D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
            rtvHandle = mRTVHeap->GetCPUDescriptorHandleForHeapStart();

            //それぞれのRTVを作成する
            for (UINT n = 0; n < FRAME_COUNT; n++) {
                throwIfFailed(mSwapChain->GetBuffer(n, IID_PPV_ARGS(&mRenderTargets[n])));
                mDevice->CreateRenderTargetView(mRenderTargets[n].Get(), nullptr, rtvHandle);
                rtvHandle.ptr += INT64(1) * UINT64(mRTVDescriptorSize);
            }
        }

        throwIfFailed(mDevice->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(&mCommandAllocator)));
#pragma endregion

#pragma region LOAD_ASSET
        //ルートシグネチャ作成
        {
            D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{};
            //使用可能なバージョンの中で上位のものを使用する
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1_1;
            if (FAILED(mDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)))) {
                featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1_0;
            }

            auto initRange = [](D3D12_DESCRIPTOR_RANGE_TYPE type, UINT num, UINT baseRegisterNumber, UINT registerSpace, D3D12_DESCRIPTOR_RANGE_FLAGS flag, UINT offset = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND) {
                D3D12_DESCRIPTOR_RANGE1 range{};
                range.RangeType = type;
                range.NumDescriptors = num;
                range.BaseShaderRegister = baseRegisterNumber;
                range.RegisterSpace = registerSpace;
                range.Flags = flag;
                range.OffsetInDescriptorsFromTableStart = offset;
                return range;
            };

            auto initParam = [](UINT num, const D3D12_DESCRIPTOR_RANGE1* ranges, D3D12_SHADER_VISIBILITY visibility) {
                D3D12_ROOT_PARAMETER1 rootParam{};
                rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                rootParam.ShaderVisibility = visibility;
                rootParam.DescriptorTable.NumDescriptorRanges = num;
                rootParam.DescriptorTable.pDescriptorRanges = ranges;
                return rootParam;
            };

            auto initRoot = [](UINT num,
                const D3D12_ROOT_PARAMETER1* param,
                UINT numStaticSampler,
                const D3D12_STATIC_SAMPLER_DESC* sampler,
                D3D12_ROOT_SIGNATURE_FLAGS flag) {
                    D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
                    rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1;
                    rootSignatureDesc.Desc_1_1.NumParameters = num;
                    rootSignatureDesc.Desc_1_1.pParameters = param;
                    rootSignatureDesc.Desc_1_1.NumStaticSamplers = numStaticSampler;
                    rootSignatureDesc.Desc_1_1.pStaticSamplers = sampler;
                    rootSignatureDesc.Desc_1_1.Flags = flag;
                    return rootSignatureDesc;
            };

            auto serializeVersionedRootSignature = [](
                const D3D12_VERSIONED_ROOT_SIGNATURE_DESC* rootSignatureDesc,
                D3D_ROOT_SIGNATURE_VERSION version,
                ID3DBlob** ppBlob,
                ID3DBlob** ppError) {
                    return D3D12SerializeVersionedRootSignature(rootSignatureDesc, ppBlob, ppError);
            };

            D3D12_DESCRIPTOR_RANGE1 range[1];
            range[0] = initRange(D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
                1,
                0,
                0,
                D3D12_DESCRIPTOR_RANGE_FLAGS::D3D12_DESCRIPTOR_RANGE_FLAG_NONE);

            D3D12_ROOT_PARAMETER1 rootParameter[3];
            rootParameter[0] = initParam(1, &range[0], D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL);
            rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL;
            rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_CBV;
            rootParameter[1].Descriptor.ShaderRegister = 0;
            rootParameter[1].Descriptor.RegisterSpace = 0;
            rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_VERTEX;
            rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_CBV;
            rootParameter[2].Descriptor.ShaderRegister = 1;
            rootParameter[2].Descriptor.RegisterSpace = 0;


            D3D12_ROOT_SIGNATURE_FLAGS  rootSignatureFlags =
                D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

            D3D12_STATIC_SAMPLER_DESC sampler{};
            sampler.Filter = D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_POINT;
            sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_BORDER;
            sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_BORDER;
            sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_BORDER;
            sampler.MipLODBias = 0;
            sampler.MaxAnisotropy = 0;
            sampler.MinLOD = 0.0f;
            sampler.MaxLOD = D3D12_FLOAT32_MAX;
            sampler.ShaderRegister = 0;
            sampler.RegisterSpace = 0;
            sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL;

            D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = initRoot(
                _countof(rootParameter),
                rootParameter,
                1,
                &sampler,
                rootSignatureFlags);

            ComPtr<ID3DBlob> sigunature, error;
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1;
            throwIfFailed(serializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &sigunature, &error));

            HRESULT hr = (mDevice->CreateRootSignature(0,
                sigunature->GetBufferPointer(),
                sigunature->GetBufferSize(),
                IID_PPV_ARGS(&mRootSignature)));
        }
        {
            auto createRasterizerState = []() {
                D3D12_RASTERIZER_DESC rasDesc{};
                rasDesc.FillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID;
                rasDesc.CullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK;
                rasDesc.FrontCounterClockwise = FALSE;
                rasDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
                rasDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
                rasDesc.DepthClipEnable = TRUE;
                rasDesc.MultisampleEnable = FALSE;
                rasDesc.AntialiasedLineEnable = FALSE;
                rasDesc.ForcedSampleCount = 0;
                rasDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE::D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
                return rasDesc;
            };

            auto createBlendState = []() {
                D3D12_BLEND_DESC blendDesc{};
                blendDesc.AlphaToCoverageEnable = FALSE;
                blendDesc.IndependentBlendEnable = FALSE;
                const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc = {
                    FALSE,FALSE,
                    D3D12_BLEND::D3D12_BLEND_ONE,D3D12_BLEND::D3D12_BLEND_ZERO,D3D12_BLEND_OP::D3D12_BLEND_OP_ADD,
                    D3D12_BLEND::D3D12_BLEND_ONE,D3D12_BLEND::D3D12_BLEND_ZERO,D3D12_BLEND_OP::D3D12_BLEND_OP_ADD,
                    D3D12_LOGIC_OP::D3D12_LOGIC_OP_NOOP,
                    D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_ALL
                };
                for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++) {
                    blendDesc.RenderTarget[i] = defaultRenderTargetBlendDesc;
                }

                return blendDesc;
            };

            //シェーダーの読み込み
            Framework::Utility::ByteReader reader((std::string)Framework::Define::Path::getInstance().shader + "VertexShader.cso");
            mVertexShader.code = reader.get();
            reader = Framework::Utility::ByteReader((std::string)Framework::Define::Path::getInstance().shader + "PixelShader.cso");
            mPixelShader.code = reader.get();

            D3D12_INPUT_ELEMENT_DESC elementDescs[] = {
                { "POSITION",   0,DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,  0,  D3D12_APPEND_ALIGNED_ELEMENT,   D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "TEXCOORD",  0,DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,        0,  D3D12_APPEND_ALIGNED_ELEMENT,   D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            };

            //パイプライン生成
            D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
            psoDesc.InputLayout = { elementDescs,_countof(elementDescs) };
            psoDesc.pRootSignature = mRootSignature.Get();
            psoDesc.VS.pShaderBytecode = mVertexShader.code.data();
            psoDesc.VS.BytecodeLength = mVertexShader.code.size();
            psoDesc.PS.pShaderBytecode = mPixelShader.code.data();
            psoDesc.PS.BytecodeLength = mPixelShader.code.size();
            psoDesc.RasterizerState = createRasterizerState();
            psoDesc.BlendState = createBlendState();
            psoDesc.DepthStencilState.DepthEnable = FALSE;
            psoDesc.DepthStencilState.StencilEnable = FALSE;
            psoDesc.SampleMask = UINT_MAX;
            psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            psoDesc.NumRenderTargets = 1;
            psoDesc.RTVFormats[0] = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
            psoDesc.SampleDesc.Count = 1;
            throwIfFailed(mDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPipelineState)));

            reader = Framework::Utility::ByteReader((std::string)Framework::Define::Path::getInstance().shader + "PixelShader2.cso");
            std::vector<BYTE> code = reader.get();
            psoDesc.PS.pShaderBytecode = code.data();
            psoDesc.PS.BytecodeLength = code.size();
            throwIfFailed(mDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPipelineState2)));
        }

        //コマンドリスト作成
        throwIfFailed(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), mPipelineState.Get(), IID_PPV_ARGS(&mCommandList)));

        //頂点バッファ作成
        {
            const float ratio = static_cast<float>(width) / static_cast<float>(height);

            Vertex vertices[]{
                {{-1.0f, 1.0f,0.0f,1.0f},{0.0f,0.0f}},
                {{ 1.0f, 1.0f,0.0f,1.0f},{1.0f,0.0f}},
                {{ 1.0f,-1.0f,0.0f,1.0f},{1.0f,1.0f}},
                {{-1.0f,-1.0f,0.0f,1.0f},{0.0f,1.0f}}
            };

            const UINT vertexBufferSize = sizeof(vertices);

            //TODO:後でUploadからstaticに変える
            throwIfFailed(mDevice->CreateCommittedResource(
                &PROPERTY(D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
                &RESOURCE(vertexBufferSize),
                D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&mVertexBuffer)));

            //頂点データをコピーする
            UINT8* vertexDataBegin;
            D3D12_RANGE range{ 0,0 };
            throwIfFailed(mVertexBuffer->Map(0, &range, reinterpret_cast<void**>(&vertexDataBegin)));
            memcpy(vertexDataBegin, vertices, sizeof(vertices));
            mVertexBuffer->Unmap(0, nullptr);

            mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
            mVertexBufferView.StrideInBytes = sizeof(Vertex);
            mVertexBufferView.SizeInBytes = vertexBufferSize;
        }

        {
            UINT indices[]{ 0,1,2,0,2,3 };
            const UINT indexBufferSize = sizeof(indices);

            throwIfFailed(mDevice->CreateCommittedResource(
                &PROPERTY(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
                &RESOURCE(indexBufferSize),
                D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&mIndexBuffer)));

            UINT8* indexDataBegin;
            D3D12_RANGE range{ 0,0 };
            throwIfFailed(mIndexBuffer->Map(0, &range, reinterpret_cast<void**>(&indexDataBegin)));
            memcpy(indexDataBegin, indices, sizeof(indices));
            mIndexBuffer->Unmap(0, nullptr);

            mIndexBufferView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
            mIndexBufferView.Format = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
            mIndexBufferView.SizeInBytes = indexBufferSize;

            mNumIndices = _countof(indices);
        }

        //テクスチャ読み込み
        ComPtr<ID3D12Resource> textureUploadHeap;
        {
            constexpr UINT TEXTURE_PIXEL_SIZE = 4;
            static const std::string TEXTURE_NAME("bg.png");
            UINT WIDTH = 256;
            UINT HEIGHT = 256;
            //テクスチャの生成
            Framework::Utility::TextureLoader loader;
            std::vector<BYTE> data = loader.load((std::string)Framework::Define::Path::getInstance().texture + TEXTURE_NAME, &WIDTH, &HEIGHT);
            //std::vector<BYTE> data = generate(WIDTH, HEIGHT, TEXTURE_PIXEL_SIZE);

            D3D12_RESOURCE_DESC desc{};
            desc.MipLevels = 1;
            desc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.Width = WIDTH;
            desc.Height = HEIGHT;
            desc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
            desc.DepthOrArraySize = 1;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;

            throwIfFailed(mDevice->CreateCommittedResource(
                &PROPERTY(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
                &desc,
                D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST,
                nullptr,
                IID_PPV_ARGS(&mTexture)));

            const UINT64 uploadBufferSize = getRequiredIntermediateSize(mTexture.Get(), 0, 1);

            //GPUアップロードバッファ作成
            throwIfFailed(mDevice->CreateCommittedResource(
                &PROPERTY(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
                &RESOURCE(uploadBufferSize),
                D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&textureUploadHeap)));

            std::vector<UINT8> texture = data;

            D3D12_SUBRESOURCE_DATA sub{};
            sub.pData = &texture[0];
            sub.RowPitch = WIDTH * TEXTURE_PIXEL_SIZE;
            sub.SlicePitch = sub.RowPitch * HEIGHT;
            updateSubresource(mCommandList.Get(), mTexture.Get(), textureUploadHeap.Get(), 0, 0, 1, &sub);
            mCommandList->ResourceBarrier(1, &BARRIER(mTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format = desc.Format;
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;
            mDevice->CreateShaderResourceView(mTexture.Get(), &srvDesc, mSRVHeap->GetCPUDescriptorHandleForHeapStart());
        }

        //コンスタントバッファ作成
        ComPtr<ID3D12Resource> constantBufferHeap;
        {
            throwIfFailed(mDevice->CreateCommittedResource(
                &PROPERTY(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
                &RESOURCE(256),
                D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&mConstantBuffer)));

            D3D12_RANGE range{ 0,0 };
            throwIfFailed(mConstantBuffer->Map(0, &range, reinterpret_cast<void**>(&mCBVDataBegin)));
            memcpy(mCBVDataBegin, &mColorBuffer, sizeof(mColorBuffer));

            throwIfFailed(mDevice->CreateCommittedResource(
                &PROPERTY(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
                &RESOURCE(256),
                D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&mMatrixConstantBuffer)));

            throwIfFailed(mMatrixConstantBuffer->Map(0, &range, reinterpret_cast<void**>(&mMatrixCBVDataBegin)));
            memcpy(mMatrixCBVDataBegin, &mMVP, sizeof(mMVP));
        }

        //コマンドリストを閉じて実行処理
        throwIfFailed(mCommandList->Close());
        ID3D12CommandList* lists[] = { mCommandList.Get() };
        mCommandQueue->ExecuteCommandLists(_countof(lists), lists);

        //フェンス作成
        {
            throwIfFailed(mDevice->CreateFence(0, D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));
            mFenceValue = 1;
            mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            if (!mFenceEvent) {
                throwIfFailed(HRESULT_FROM_WIN32(GetLastError()));
            }

            waitForPreviousFrame();
        }

#pragma endregion
        mViewport.TopLeftX = 0;
        mViewport.TopLeftY = 0;
        mViewport.Width = static_cast<float>(width);
        mViewport.Height = static_cast<float>(height);
        mViewport.MinDepth = 0.0f;
        mViewport.MaxDepth = 1.0f;

        mScissorRect.left = 0;
        mScissorRect.top = 0;
        mScissorRect.right = static_cast<LONG>(width);
        mScissorRect.bottom = static_cast<LONG>(height);

        return true;
    }
protected:
    virtual void update() override {
        Game::update();
        mColorBuffer.color.r += 0.01f;
        if (mColorBuffer.color.r >= 1.0f) mColorBuffer.color.r -= 1.0f;
        mColorBuffer.color.g += 0.005f;
        if (mColorBuffer.color.g >= 1.0f) mColorBuffer.color.g -= 1.0f;

        memcpy(mCBVDataBegin, &mColorBuffer, sizeof(mColorBuffer));

        using Framework::Math::Vector3;
        using Framework::Math::Matrix4x4;
        mMVP.world = Matrix4x4::transposition(Matrix4x4::createScale(Vector3(mScale, mScale, mScale)));
        mMVP.view = Matrix4x4::transposition(Matrix4x4::createView({ Vector3(0,0,-10),Vector3(0,0,0),Vector3(0,1,0) }));
        float ratio = static_cast<float>(Framework::Define::Config::getInstance().screenWidth) / static_cast<float>(Framework::Define::Config::getInstance().screenHeight);
        mMVP.proj = Matrix4x4::transposition(Matrix4x4::createProjection({ 45.0f,ratio,0.1f,1000.0f }));
        memcpy(mMatrixCBVDataBegin, &mMVP, sizeof(mMVP));
        mScale += 0.01f;

        if (Framework::Device::GameDevice::getInstance().getInputManager()->getKeyboard().getKeyDown(Framework::Input::KeyCode::A)) {
            mMode = !mMode;
        }
    }
    virtual void draw() override {
        throwIfFailed(mCommandAllocator->Reset());
        throwIfFailed(mCommandList->Reset(mCommandAllocator.Get(), mPipelineState.Get()));
        mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

        ID3D12DescriptorHeap* heaps[] = { mSRVHeap.Get() };
        mCommandList->SetDescriptorHeaps(_countof(heaps), heaps);

        if (mMode) {
            mCommandList->SetPipelineState(mPipelineState.Get());
        }
        else {
            mCommandList->SetPipelineState(mPipelineState2.Get());
        }

        mCommandList->SetGraphicsRootDescriptorTable(0, mSRVHeap->GetGPUDescriptorHandleForHeapStart());
        mCommandList->RSSetViewports(1, &mViewport);
        mCommandList->RSSetScissorRects(1, &mScissorRect);

        mCommandList->ResourceBarrier(1, &BARRIER(mRenderTargets[mFrameIndex].Get(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET));

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{};
        rtvHandle.ptr = static_cast<SIZE_T>(mRTVHeap->GetCPUDescriptorHandleForHeapStart().ptr + INT64(mFrameIndex) * UINT64(mRTVDescriptorSize));
        mCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

        const float clear[] = { 0.65f,0.2f,0.48f,0.0f };
        mCommandList->ClearRenderTargetView(rtvHandle, clear, 0, nullptr);
        mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        mCommandList->SetGraphicsRootConstantBufferView(1, mConstantBuffer->GetGPUVirtualAddress());
        mCommandList->SetGraphicsRootConstantBufferView(2, mMatrixConstantBuffer->GetGPUVirtualAddress());
        mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
        mCommandList->IASetIndexBuffer(&mIndexBufferView);
        mCommandList->DrawInstanced(3, 1, 0, 0);
        mCommandList->DrawIndexedInstanced(mNumIndices, 1, 0, 0, 0);

        mCommandList->ResourceBarrier(1, &BARRIER(mRenderTargets[mFrameIndex].Get(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT));

        throwIfFailed(mCommandList->Close());

        ID3D12CommandList* lists[] = { mCommandList.Get() };
        mCommandQueue->ExecuteCommandLists(_countof(lists), lists);

        throwIfFailed(mSwapChain->Present(1, 0));
        waitForPreviousFrame();
    }
    void finalize() override {
        waitForPreviousFrame();
        CloseHandle(mFenceEvent);
        Game::finalize();
    }

private:
    void waitForPreviousFrame() {
        const UINT64 fence = mFenceValue;
        throwIfFailed(mCommandQueue->Signal(mFence.Get(), fence));
        mFenceValue++;
        if (mFence->GetCompletedValue() < fence) {
            throwIfFailed(mFence->SetEventOnCompletion(fence, mFenceEvent));
            WaitForSingleObject(mFenceEvent, INFINITE);
        }

        mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
    }
private:
    struct ShaderObject {
        std::vector<BYTE> code;
    };
    static constexpr int FRAME_COUNT = 2; //!< バックバッファの枚数
    UINT mFrameIndex; //!< 現在のバックバッファフレーム番号
    UINT mRTVDescriptorSize; //!< RTVディスクリプタヒープの大きさ
    ComPtr<ID3D12Device> mDevice; //!< デバイス
    ComPtr<ID3D12CommandQueue> mCommandQueue; //!< コマンドキュー
    ComPtr<IDXGISwapChain3> mSwapChain; //!< スワップチェイン
    ComPtr<ID3D12DescriptorHeap> mRTVHeap; //!< RTV用
    ComPtr<ID3D12DescriptorHeap> mSRVHeap; //!< テクスチャSRV用
    ComPtr<ID3D12Resource> mRenderTargets[FRAME_COUNT]; //!< レンダーターゲット
    ComPtr<ID3D12CommandAllocator> mCommandAllocator; //!< コマンドアロケータ
    ComPtr<ID3D12RootSignature> mRootSignature; //!< ルートシグネチャ
    ShaderObject mVertexShader;
    ShaderObject mPixelShader;
    ComPtr<ID3D12PipelineState> mPipelineState; //!< パイプラインステート
    ComPtr<ID3D12PipelineState> mPipelineState2; //!< パイプラインステート
    ComPtr<ID3D12GraphicsCommandList> mCommandList; //!< コマンドリスト
    ComPtr<ID3D12Resource> mVertexBuffer; //!< 頂点バッファ
    D3D12_VERTEX_BUFFER_VIEW mVertexBufferView; //!< 頂点バッファビュー
    ComPtr<ID3D12Resource> mIndexBuffer; //!< インデックスバッファ
    D3D12_INDEX_BUFFER_VIEW mIndexBufferView; //!< インデックスバッファビュー
    ComPtr<ID3D12Resource> mTexture; //!< テクスチャ
    ComPtr<ID3D12Fence> mFence; //!< フェンス
    UINT64 mFenceValue;
    HANDLE mFenceEvent;
    D3D12_VIEWPORT mViewport;
    D3D12_RECT mScissorRect;
    ComPtr<ID3D12DescriptorHeap> mCBVHeap; //!< コンスタントバッファヒープ
    ComPtr<ID3D12Resource> mConstantBuffer; //!< コンスタントバッファ
    UINT* mCBVDataBegin;
    ComPtr<ID3D12DescriptorHeap> mMatrixCBVHeap; //!< コンスタントバッファヒープ
    ComPtr<ID3D12Resource> mMatrixConstantBuffer; //!< コンスタントバッファ
    UINT* mMatrixCBVDataBegin;
    ColorBuffer mColorBuffer;
    MVP mMVP;
    float mScale;
    bool mMode;
    UINT mNumIndices;
};

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPTSTR, _In_ int) {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    MyGame game("Game", hInstance);
    return game.run();
}

