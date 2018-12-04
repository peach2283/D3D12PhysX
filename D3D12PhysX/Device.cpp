#include "stdafx.h"
#include "Device.h"
#include "d3dx12.h"

#include <stdio.h>

#pragma comment ( lib, "d3d12.lib")
#pragma comment ( lib, "D3DCompiler.lib")
#pragma comment ( lib, "dxgi.lib")

IDXGIFactory * Device::factory = NULL;

ID3D12Device * Device::device=NULL;
ID3D12Fence  * Device::fence =NULL;

ID3D12CommandQueue              * Device::cmdQueue    = NULL;
ID3D12CommandAllocator          * Device::cmdAlloc    = NULL;
ID3D12GraphicsCommandList       * Device::cmdList     = NULL;

IDXGISwapChain  *Device::swapChain=NULL;

ID3D12DescriptorHeap * Device::rtvHeap = NULL;
ID3D12DescriptorHeap * Device::dsvHeap = NULL;

ID3D12Resource *  Device::swapChainBuff[NUM_SWAPCHAIN_BUFF] = { NULL, };
ID3D12Resource *  Device::dsBuffer = NULL;
D3D12_VIEWPORT    Device::viewRect = { 0.0f, };
D3D12_RECT        Device::sissorRect{ 0, };

int Device::bufferIndex   =  0;
UINT64 Device::fenceValue =  0;
HANDLE Device::fenceEvent = NULL;

bool Device::init(HWND hWnd, int width, int height)
{
    if (createFactory()     != true) return false;
    if (createDevice()      != true) return false;
    if (ceateCommandQueue() != true) return false;
    if (ceateSwapChain(hWnd, width, height) != true) return false;
    if (createRTView() != true) return false;
    if (createDSTView(width, height) != true) return false;
    if(setViewport(width,  height) != true) return false;

    return true;
}

bool Device::createFactory()
{
    //다이렉트 그래픽 인터페이스 팩토리 생성하기
    if (CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory) != S_OK)
    {
        printf("팩토리 생성..실패\n");
        return false;
    }

    return true;
}

bool Device::createDevice()
{
    //Direct3D 12 장치 생성하기
    if (D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&device) != S_OK)
    {
        printf("Direct3D12 장치 생성 실패\n");
        return false;
    }
  
    //Fence 생성하기
    if (device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&fence) != S_OK)
    {
        printf("Fence 생성 실패\n");
        return false;
    }
 
    /////////////////Fence 이벤트 핸들러..생성하기/////////////////////
    fenceEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);

    return true;
}

bool Device::ceateCommandQueue()
{
    //Command Queue생성
    D3D12_COMMAND_QUEUE_DESC queueDesc;
    ZeroMemory(&queueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));

    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

    if (device->CreateCommandQueue(&queueDesc, __uuidof(ID3D12CommandQueue), (void**)&cmdQueue) != S_OK)
    {
        printf("Command Queue 생성 실패\n");
        return false;
    }

    //Command Allocator 생성
    if (device->CreateCommandAllocator(queueDesc.Type, __uuidof(ID3D12CommandAllocator), (void**)&cmdAlloc) != S_OK)
    {
        printf("Command Allocator 실패\n");
        return false;
    }

    //Graphics Command List 생성
    if (device->CreateCommandList(0, queueDesc.Type, cmdAlloc, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&cmdList) != S_OK)
    {
        printf("Command List 생성 실패\n");
        return false;
    }

    cmdList->Close();
    return true;
}

bool Device::ceateSwapChain(HWND hWnd, int width, int height)
{
    //[6]스왑체인 생성하기
    DXGI_SWAP_CHAIN_DESC scDesc;  //스왑체인을 만들기 위한 정보 구조체
    ZeroMemory(&scDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

    scDesc.BufferDesc.Width = width;    //버퍼 가로 길이
    scDesc.BufferDesc.Height = height;   //버퍼 세로 길이
    scDesc.BufferDesc.RefreshRate.Numerator = 60;  //리프레시 분모
    scDesc.BufferDesc.RefreshRate.Denominator = 1; // 리프레시 분자

    scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.SampleDesc.Count = 1; //멀티 샘플링 갯수
    scDesc.SampleDesc.Quality = 0; //멀티 샘플링 레블

    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;       //화면 출력으로 사용
    scDesc.BufferCount = NUM_SWAPCHAIN_BUFF;                    //백 버퍼 갯수
    scDesc.OutputWindow = hWnd;  //윈도우 핸들
    scDesc.Windowed = true;      //윈도우 또는 전체 화면 
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    if (factory->CreateSwapChain(cmdQueue, &scDesc, &swapChain) != S_OK)
    {
        printf("스왑체인  생성 실패\n");
        return false;
    }
   
    return true;
}

bool Device::createRTView()
{
    //Render Target View 디스크립터 힙 생성
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    ZeroMemory(&rtvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));

    rtvHeapDesc.NumDescriptors = NUM_SWAPCHAIN_BUFF;  //백버퍼의 갯수 만큼
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    if (device->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&rtvHeap) != S_OK)
    {
        printf("렌더 타겟 뷰 디스크립터 힙 생성 실패\n");
        return false;
    }
   
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
    unsigned int incSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    for (int i = 0; i < NUM_SWAPCHAIN_BUFF; i++)
    {
        swapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&swapChainBuff[i]);
        device->CreateRenderTargetView(swapChainBuff[i], NULL, rtvHandle);

        rtvHandle.ptr += incSize;
    }

    return true;
}

bool Device::createDSTView(int width, int height)
{
    //[8]Depath/Stencil Target View 디스크립터 힙 생성
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    ZeroMemory(&dsvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));

    dsvHeapDesc.NumDescriptors = 1;  //백버퍼의 갯수 만큼
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    if (device->CreateDescriptorHeap(&dsvHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&dsvHeap) != S_OK)
    {
        printf("Depth/Stencil 디스크립터 힙 실패\n");
        return false;
    }
   
    //[10]Depth/Stencil 버퍼와 뷰 생성
    D3D12_RESOURCE_DESC dsDesc;
    ZeroMemory(&dsDesc, sizeof(D3D12_RESOURCE_DESC));

    dsDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    dsDesc.Width = width;
    dsDesc.Height = height;
    dsDesc.DepthOrArraySize = 1;
    dsDesc.MipLevels = 1;
    dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsDesc.SampleDesc.Count = 1;
    dsDesc.SampleDesc.Quality = 0;
    dsDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    dsDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clearValue;
    ZeroMemory(&clearValue, sizeof(D3D12_CLEAR_VALUE));

    clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    if (device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &dsDesc, D3D12_RESOURCE_STATE_COMMON, &clearValue, __uuidof(ID3D12Resource), (void**)&dsBuffer) != S_OK)
    {
        printf("Depth/Stencil 버퍼 생성 실패\n");
        return false;
    }
   

    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
    device->CreateDepthStencilView(dsBuffer, NULL, dsvHandle);  //Depth / Stencil View 생성

    ///////Depth/Stencil 버퍼의 상태를 WRITE 상태로..변경한다////////
    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(dsBuffer,
                                                                D3D12_RESOURCE_STATE_COMMON,
                                                                D3D12_RESOURCE_STATE_DEPTH_WRITE);
                                                                cmdList->ResourceBarrier(1, &barrier);
    return true;
}

bool Device::setViewport(int width, int height)
{
    viewRect   = { 0.0f, 0.0f, (float)width, float(height), 0.0f, 1.0f };
    sissorRect = { 0, 0,width, height };

    return true;
}


void  Device::clear(unsigned char r, unsigned char g, unsigned char b)
{
    //[1]Command List 리셋
    reset(NULL, true);

    //[2]백 버퍼 사용상태 전이[PRESENT->TARGET]
    ///////Depth/Stencil 버퍼의 상태를 WRITE 상태로..변경한다////////
    D3D12_RESOURCE_BARRIER barrier;
    
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(swapChainBuff[bufferIndex],
                                                   D3D12_RESOURCE_STATE_PRESENT, 
                                                   D3D12_RESOURCE_STATE_RENDER_TARGET);
    cmdList->ResourceBarrier(1, &barrier);

    //[5]후면 버퍼와 깊이-스텐실 버퍼 지우기
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();

    unsigned int incSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    rtvHandle.ptr += incSize * bufferIndex;

    float BGColor[4];

    BGColor[0] = r / 255.0f;
    BGColor[1] = g / 255.0f;
    BGColor[2] = b / 255.0f;
    BGColor[3] = 1.0f;

    cmdList->ClearRenderTargetView(rtvHandle, BGColor, 0, NULL);
    cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

    //[4]명령어 실행
    execute(true);
}

void  Device::render()
{
    //[1]Command List 리셋
    reset(NULL, true);

    //[2]백 버퍼 사용상태 전이[TARGET->PRESENT]
    D3D12_RESOURCE_BARRIER barrier=CD3DX12_RESOURCE_BARRIER::Transition(swapChainBuff[bufferIndex],
                                                                        D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                                        D3D12_RESOURCE_STATE_PRESENT);

    cmdList->ResourceBarrier(1, &barrier);

    //[3]명령어 실행 및 플러쉬
    execute(true);
   
    //[4]전면과 후면 버퍼 교체하기
    swapChain->Present(0, 0);
    bufferIndex = (bufferIndex + 1) % NUM_SWAPCHAIN_BUFF;
}

void Device::reset(ID3D12PipelineState * pipelineState, bool alloc, bool view)
{
    if (alloc == true)
    {
        //[1]Command Allocator 리셋
        if (cmdAlloc->Reset() != S_OK)
        {
            printf("Command Allocator 리셋 에러\n");
        }
    }

    //[2]Command List 리셋
    if (cmdList->Reset(cmdAlloc, pipelineState) != S_OK)
    {
        printf("Command List 리셋 에러\n");
    }

    //[3]RenderTarget과 View 레셋
    if(view == true)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();

        unsigned int incSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        rtvHandle.ptr += incSize * bufferIndex;

        cmdList->RSSetViewports(1, &viewRect);
        cmdList->RSSetScissorRects(1, &sissorRect);

        cmdList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);
    }
}

void Device::execute(bool flush)
{
    //[1]명령어 종료
    cmdList->Close();

    //[2]명령어를 대기열에 추가
    ID3D12CommandList * cmdLists[1] = { cmdList };
    cmdQueue->ExecuteCommandLists(1, cmdLists);

    if (flush == true)
    {
        //[3]CommandList 모두 비우기
        fenceValue++;

        if (cmdQueue->Signal(fence, fenceValue) == S_OK)
        {
            if (fence->GetCompletedValue() < fenceValue)
            {
                if (fence->SetEventOnCompletion(fenceValue, fenceEvent) == S_OK)
                {
                    WaitForSingleObject(fenceEvent, INFINITE);
                }
            }
        }
    }
}

void Device::exit()
{
    if (fenceEvent != NULL)
    {
        CloseHandle(fenceEvent);
    }

    ReleaseCOM(dsBuffer);

    for (int i = 0; i < NUM_SWAPCHAIN_BUFF; i++)
    {
        ReleaseCOM(swapChainBuff[i]);
    }

    ReleaseCOM(rtvHeap);
    ReleaseCOM(dsvHeap);
    ReleaseCOM(swapChain);

    ReleaseCOM(cmdList);
    ReleaseCOM(cmdAlloc);
    ReleaseCOM(cmdQueue);

    ReleaseCOM(fence);
    ReleaseCOM(device);

    ReleaseCOM(factory);

}

ID3D12Resource * Device::createBuffer(void * initData, UINT64 size, ID3D12Resource **UPBuffer)
{
     reset(NULL, true);

     ID3D12Resource * defaultBuffer=NULL;

     D3D12_RESOURCE_DESC   buffer = CD3DX12_RESOURCE_DESC::Buffer(size);

     //Create the actual default buffer resource.
     D3D12_HEAP_PROPERTIES defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
     
     if (device->CreateCommittedResource(&defaultHeap,
                                         D3D12_HEAP_FLAG_NONE,
                                         &buffer,
                                         D3D12_RESOURCE_STATE_COMMON,
                                         nullptr,
                                         __uuidof(ID3D12Resource), (void**)&defaultBuffer) != S_OK)
     {
         printf("기본 버퍼 생성 실패\n");
         return NULL;
     }

     //Create the upload buffer resource.
     D3D12_HEAP_PROPERTIES uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    
     if (device->CreateCommittedResource(&uploadHeap,
                                         D3D12_HEAP_FLAG_NONE,
                                         &buffer,
                                         D3D12_RESOURCE_STATE_GENERIC_READ,
                                         nullptr,
                                         __uuidof(ID3D12Resource), (void**)UPBuffer) != S_OK)
     {
         printf("업로드 버퍼 생성 실패\n");
         return NULL;
     }

     // Describe the data we want to copy into the default buffer.
     D3D12_SUBRESOURCE_DATA subResourceData;
     ZeroMemory(&subResourceData, sizeof(D3D12_SUBRESOURCE_DATA));

     subResourceData.pData      = initData;
     subResourceData.RowPitch   = (LONG_PTR)size;
     subResourceData.SlicePitch = subResourceData.RowPitch;

     D3D12_RESOURCE_BARRIER barrier;

     barrier = CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer,
                                                     D3D12_RESOURCE_STATE_COMMON,
                                                     D3D12_RESOURCE_STATE_COPY_DEST);

     cmdList->ResourceBarrier(1, &barrier);

     UpdateSubresources<1>(cmdList, defaultBuffer, *UPBuffer, 0, 0, 1, &subResourceData);

     barrier = CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer,
                                                    D3D12_RESOURCE_STATE_COPY_DEST, 
                                                    D3D12_RESOURCE_STATE_GENERIC_READ);

     cmdList->ResourceBarrier(1, &barrier);

     execute(true);
     return defaultBuffer;
}

ID3D12Device * Device::getDevice()
{
    return device;
}

ID3D12GraphicsCommandList * Device::getCmdList()
{
    return cmdList; 
}