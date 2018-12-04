#include "stdafx.h"
#include "Device.h"
#include "DebugRender.h"

#include <stdio.h>
#include "d3dx12.h"
#include <D3DCompiler.h>

const D3D12_INPUT_ELEMENT_DESC  InputLayoutDesc[] =
{
    { "POSITION",     0, DXGI_FORMAT_R32G32B32A32_FLOAT,    0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "COLOR"  ,      0, DXGI_FORMAT_R32G32B32A32_FLOAT,    0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

DebugRender::DebugRender()
{
    vsBuffer = NULL;
    psBuffer = NULL;

    cbdescHeap   = NULL;
    cbUPBuffer   = NULL;

    rootSignature = NULL;
    pipelineState = NULL;

    DirectX::XMMATRIX I = DirectX::XMMatrixIdentity();
}

DebugRender::~DebugRender()
{
    ReleaseCOM(pipelineState)
    ReleaseCOM(rootSignature)

    if (cbUPBuffer != NULL)
    {
        cbUPBuffer->Unmap(0, NULL);
    }

    ReleaseCOM(cbUPBuffer);
    ReleaseCOM(cbdescHeap);

    ReleaseCOM(vsBuffer)
    ReleaseCOM(psBuffer)
}

bool DebugRender::init()
{
    //[1] 세이더 컴파일 하기
    vsBuffer = compileShader(L"DebugShader.hlsl", "vs_main", "vs_5_0");
    psBuffer = compileShader(L"DebugShader.hlsl", "ps_main", "ps_5_0");

    //[2] 상수버퍼 Descriptor Heap 생성하기
    /*************버텍스 세이더 상수버퍼*******************************************/
    if (createConstBuffer(&cbdescHeap, &cbData, sizeof(ConstBuffer), &cbUPBuffer) != true)
    {
        printf("버텍스 세이더 상수 버퍼 만들기 실패\n");
        return false;
    }

    //[3]Root 시그니쳐 생성하기
    CD3DX12_ROOT_PARAMETER slotRootParameter[1];
    CD3DX12_DESCRIPTOR_RANGE cbvVSTable;

    cbvVSTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);  //버텍스 세이터 상수 버퍼   
    slotRootParameter[0].InitAsDescriptorTable(1, &cbvVSTable);
   
    CD3DX12_STATIC_SAMPLER_DESC linearWrapSampler(  0, // shaderRegister
                                                    D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
                                                    D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
                                                    D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
                                                    D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 1, &linearWrapSampler,
                                            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    ID3DBlob * serializedRootSig = NULL;
    ID3DBlob * errorBlob         = NULL;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
                                             &serializedRootSig, &errorBlob);

    if (errorBlob != NULL)
    {
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        return false;
    }
 
    if (Device::getDevice()->CreateRootSignature(0,
                                                 serializedRootSig->GetBufferPointer(),
                                                 serializedRootSig->GetBufferSize(),
                                                 __uuidof(ID3D12RootSignature), (void **)&rootSignature) != S_OK)
    {
        printf("루트 시그너쳐 실패\n");
        return false;
    }
       
    //[4]파이프라인 상태 및 래스터라이저값 지정하기
    ZeroMemory(&pipeLineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

    pipeLineStateDesc.InputLayout    = {InputLayoutDesc, 2};
    pipeLineStateDesc.pRootSignature = rootSignature;

    pipeLineStateDesc.VS = { vsBuffer->GetBufferPointer() , vsBuffer->GetBufferSize() };
    pipeLineStateDesc.PS = { psBuffer->GetBufferPointer() , psBuffer->GetBufferSize() };
    pipeLineStateDesc.RasterizerState   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pipeLineStateDesc.BlendState        = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pipeLineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pipeLineStateDesc.SampleMask            = UINT_MAX;
    pipeLineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipeLineStateDesc.NumRenderTargets      = 1;
    pipeLineStateDesc.RTVFormats[0]         = DXGI_FORMAT_R8G8B8A8_UNORM;
    pipeLineStateDesc.SampleDesc.Count      = 1;
    pipeLineStateDesc.SampleDesc.Quality    = 0;
    pipeLineStateDesc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;

    if (Device::getDevice()->CreateGraphicsPipelineState(&pipeLineStateDesc,
                                                            __uuidof(ID3D12PipelineState),
                                                            (void **)&pipelineState) != S_OK)
    {
        printf("파이프 라인 상태 객체 생성 실패\n");
        return false;
    }

    return true;
}

bool DebugRender::createConstBuffer(ID3D12DescriptorHeap **descHeap, BYTE ** Data, int size, ID3D12Resource **UPBuffer)
{
    //[2] 상수버퍼 Descriptor Heap 생성하기
    D3D12_DESCRIPTOR_HEAP_DESC  cbDesc;
    cbDesc.NumDescriptors = 1;
    cbDesc.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbDesc.NodeMask = 0;

    if (Device::getDevice()->CreateDescriptorHeap(&cbDesc,
                                                    __uuidof(ID3D12DescriptorHeap),
                                                     (void**)descHeap) != S_OK)
    {
        printf("상수 버퍼 디스크립터 생성 실패\n");
        return false;
    }

    //[3] 상수버퍼  생성하기
    size = (size + 255) & ~255;  //256배수 단위

    if (Device::getDevice()->CreateCommittedResource(
                                                    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                                                    D3D12_HEAP_FLAG_NONE,
                                                    &CD3DX12_RESOURCE_DESC::Buffer(size),
                                                    D3D12_RESOURCE_STATE_GENERIC_READ,
                                                    nullptr,
                                                    __uuidof(ID3D12Resource), (void**)UPBuffer) != S_OK)
    {
        printf("상수 버퍼  생성 실패\n");
        return false;
    }

    if ((*UPBuffer)->Map(0, NULL, (void**)Data) != S_OK)
    {
        printf("상수 버퍼  매핑 실패\n");
        return false;
    }

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
    cbvDesc.BufferLocation = (*UPBuffer)->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes    = size;

    Device::getDevice()->CreateConstantBufferView(&cbvDesc, (*descHeap)->GetCPUDescriptorHandleForHeapStart());

    return true;
}

void DebugRender::draw(D3D12_VERTEX_BUFFER_VIEW * lineBuffer, int lineSize, DirectX::XMMATRIX  * view, DirectX::XMMATRIX  * proj)
{
    Device::reset(pipelineState, true, true);

    ConstBuffer *cbuffer = (ConstBuffer*)cbData;

    cbuffer->view = DirectX::XMMatrixTranspose(*view);
    cbuffer->proj = DirectX::XMMatrixTranspose(*proj);

    Device::getCmdList()->SetGraphicsRootSignature(rootSignature);

    Device::getCmdList()->IASetVertexBuffers(0, 1, lineBuffer);
    Device::getCmdList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    Device::getCmdList()->SetGraphicsRootDescriptorTable(0, cbdescHeap->GetGPUDescriptorHandleForHeapStart());
    Device::getCmdList()->DrawInstanced(lineSize, 1, 0, 0);
    Device::execute(true);
}

ID3DBlob * DebugRender::compileShader(LPCWSTR fileName, LPCSTR entry, LPCSTR target)
{
    HRESULT hr;

    ID3DBlob * byteCode = nullptr;
    ID3DBlob * errorBlob;

    hr = D3DCompileFromFile(fileName,
                            NULL,
                            D3D_COMPILE_STANDARD_FILE_INCLUDE,
                            entry,
                            target,
                            0,
                            0,
                            &byteCode, &errorBlob);

    if (errorBlob != NULL)
    {
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        printf("Shader 파일 컴파일 에러\n");
        return NULL;
    }

    if (FAILED(hr))
    {
        printf("Shader 파일 컴파일 에러\n");
        return NULL;
    }

    return byteCode;
}