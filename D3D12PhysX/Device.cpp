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
    //���̷�Ʈ �׷��� �������̽� ���丮 �����ϱ�
    if (CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory) != S_OK)
    {
        printf("���丮 ����..����\n");
        return false;
    }

    return true;
}

bool Device::createDevice()
{
    //Direct3D 12 ��ġ �����ϱ�
    if (D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&device) != S_OK)
    {
        printf("Direct3D12 ��ġ ���� ����\n");
        return false;
    }
  
    //Fence �����ϱ�
    if (device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&fence) != S_OK)
    {
        printf("Fence ���� ����\n");
        return false;
    }
 
    /////////////////Fence �̺�Ʈ �ڵ鷯..�����ϱ�/////////////////////
    fenceEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);

    return true;
}

bool Device::ceateCommandQueue()
{
    //Command Queue����
    D3D12_COMMAND_QUEUE_DESC queueDesc;
    ZeroMemory(&queueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));

    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

    if (device->CreateCommandQueue(&queueDesc, __uuidof(ID3D12CommandQueue), (void**)&cmdQueue) != S_OK)
    {
        printf("Command Queue ���� ����\n");
        return false;
    }

    //Command Allocator ����
    if (device->CreateCommandAllocator(queueDesc.Type, __uuidof(ID3D12CommandAllocator), (void**)&cmdAlloc) != S_OK)
    {
        printf("Command Allocator ����\n");
        return false;
    }

    //Graphics Command List ����
    if (device->CreateCommandList(0, queueDesc.Type, cmdAlloc, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&cmdList) != S_OK)
    {
        printf("Command List ���� ����\n");
        return false;
    }

    cmdList->Close();
    return true;
}

bool Device::ceateSwapChain(HWND hWnd, int width, int height)
{
    //[6]����ü�� �����ϱ�
    DXGI_SWAP_CHAIN_DESC scDesc;  //����ü���� ����� ���� ���� ����ü
    ZeroMemory(&scDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

    scDesc.BufferDesc.Width = width;    //���� ���� ����
    scDesc.BufferDesc.Height = height;   //���� ���� ����
    scDesc.BufferDesc.RefreshRate.Numerator = 60;  //�������� �и�
    scDesc.BufferDesc.RefreshRate.Denominator = 1; // �������� ����

    scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.SampleDesc.Count = 1; //��Ƽ ���ø� ����
    scDesc.SampleDesc.Quality = 0; //��Ƽ ���ø� ����

    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;       //ȭ�� ������� ���
    scDesc.BufferCount = NUM_SWAPCHAIN_BUFF;                    //�� ���� ����
    scDesc.OutputWindow = hWnd;  //������ �ڵ�
    scDesc.Windowed = true;      //������ �Ǵ� ��ü ȭ�� 
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    if (factory->CreateSwapChain(cmdQueue, &scDesc, &swapChain) != S_OK)
    {
        printf("����ü��  ���� ����\n");
        return false;
    }
   
    return true;
}

bool Device::createRTView()
{
    //Render Target View ��ũ���� �� ����
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    ZeroMemory(&rtvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));

    rtvHeapDesc.NumDescriptors = NUM_SWAPCHAIN_BUFF;  //������� ���� ��ŭ
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    if (device->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&rtvHeap) != S_OK)
    {
        printf("���� Ÿ�� �� ��ũ���� �� ���� ����\n");
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
    //[8]Depath/Stencil Target View ��ũ���� �� ����
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    ZeroMemory(&dsvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));

    dsvHeapDesc.NumDescriptors = 1;  //������� ���� ��ŭ
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    if (device->CreateDescriptorHeap(&dsvHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&dsvHeap) != S_OK)
    {
        printf("Depth/Stencil ��ũ���� �� ����\n");
        return false;
    }
   
    //[10]Depth/Stencil ���ۿ� �� ����
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
        printf("Depth/Stencil ���� ���� ����\n");
        return false;
    }
   

    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
    device->CreateDepthStencilView(dsBuffer, NULL, dsvHandle);  //Depth / Stencil View ����

    ///////Depth/Stencil ������ ���¸� WRITE ���·�..�����Ѵ�////////
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
    //[1]Command List ����
    reset(NULL, true);

    //[2]�� ���� ������ ����[PRESENT->TARGET]
    ///////Depth/Stencil ������ ���¸� WRITE ���·�..�����Ѵ�////////
    D3D12_RESOURCE_BARRIER barrier;
    
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(swapChainBuff[bufferIndex],
                                                   D3D12_RESOURCE_STATE_PRESENT, 
                                                   D3D12_RESOURCE_STATE_RENDER_TARGET);
    cmdList->ResourceBarrier(1, &barrier);

    //[5]�ĸ� ���ۿ� ����-���ٽ� ���� �����
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

    //[4]��ɾ� ����
    execute(true);
}

void  Device::render()
{
    //[1]Command List ����
    reset(NULL, true);

    //[2]�� ���� ������ ����[TARGET->PRESENT]
    D3D12_RESOURCE_BARRIER barrier=CD3DX12_RESOURCE_BARRIER::Transition(swapChainBuff[bufferIndex],
                                                                        D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                                        D3D12_RESOURCE_STATE_PRESENT);

    cmdList->ResourceBarrier(1, &barrier);

    //[3]��ɾ� ���� �� �÷���
    execute(true);
   
    //[4]����� �ĸ� ���� ��ü�ϱ�
    swapChain->Present(0, 0);
    bufferIndex = (bufferIndex + 1) % NUM_SWAPCHAIN_BUFF;
}

void Device::reset(ID3D12PipelineState * pipelineState, bool alloc, bool view)
{
    if (alloc == true)
    {
        //[1]Command Allocator ����
        if (cmdAlloc->Reset() != S_OK)
        {
            printf("Command Allocator ���� ����\n");
        }
    }

    //[2]Command List ����
    if (cmdList->Reset(cmdAlloc, pipelineState) != S_OK)
    {
        printf("Command List ���� ����\n");
    }

    //[3]RenderTarget�� View ����
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
    //[1]��ɾ� ����
    cmdList->Close();

    //[2]��ɾ ��⿭�� �߰�
    ID3D12CommandList * cmdLists[1] = { cmdList };
    cmdQueue->ExecuteCommandLists(1, cmdLists);

    if (flush == true)
    {
        //[3]CommandList ��� ����
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
         printf("�⺻ ���� ���� ����\n");
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
         printf("���ε� ���� ���� ����\n");
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