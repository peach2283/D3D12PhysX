#pragma once

#include <d3d12.h>
#include <dxgi.h>

#define NUM_SWAPCHAIN_BUFF 2

#define ReleaseCOM(x) { if(x){ x->Release(); x = NULL; } }
#define SafeDelete(x) { if(x){ delete x; x = NULL; } }

class Device
{
private:

    static IDXGIFactory * factory;

    static ID3D12Device * device;
    static ID3D12Fence  * fence;
    
    static ID3D12CommandQueue             * cmdQueue;
    static ID3D12CommandAllocator         * cmdAlloc;
    static ID3D12GraphicsCommandList      * cmdList;

    static  IDXGISwapChain                * swapChain;

    static ID3D12DescriptorHeap * rtvHeap;
    static ID3D12DescriptorHeap * dsvHeap;

    static ID3D12Resource * swapChainBuff[NUM_SWAPCHAIN_BUFF];

    static ID3D12Resource * dsBuffer;
    static D3D12_VIEWPORT   viewRect;
    static D3D12_RECT       sissorRect;

    static int    bufferIndex;
    static UINT64 fenceValue;
    static HANDLE fenceEvent;

    static bool createFactory();
    static bool createDevice();
    static bool ceateCommandQueue();
    static bool ceateSwapChain(HWND hWnd, int width, int height);
    static bool createRTView();
    static bool createDSTView(int width, int height);
    static bool setViewport(int width, int height);

public:

    static bool init(HWND hWnd, int width, int height);
    static void clear(unsigned char r, unsigned char g, unsigned char b);
    static void render();
    static void exit();

    static void reset(ID3D12PipelineState * pipelineState , bool alloc, bool view=false);
    static void execute(bool flush);

    static ID3D12Resource * createBuffer(void * initData, UINT64 size, ID3D12Resource **UPBuffer);

    static ID3D12Device  *  getDevice();
    static ID3D12GraphicsCommandList * getCmdList();
};