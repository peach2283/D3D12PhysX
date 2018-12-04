#pragma once

#include "Device.h"
#include <DirectXMath.h>

struct ConstBuffer
{
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX proj;
};

class DebugRender
{
private:

    ID3DBlob * vsBuffer;
    ID3DBlob * psBuffer;

    ////////버텍스 세이더 상수 버퍼//////
    ID3D12DescriptorHeap *cbdescHeap;
    ID3D12Resource * cbUPBuffer;
    BYTE* cbData;

    ID3D12RootSignature * rootSignature;
    ID3D12PipelineState * pipelineState;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeLineStateDesc;

public:
    DebugRender();
    virtual ~DebugRender();

    ID3DBlob * compileShader(LPCWSTR fileName, LPCSTR entry, LPCSTR target);
    bool createConstBuffer(ID3D12DescriptorHeap **descHeap, BYTE ** Data, int size, ID3D12Resource **UPBuffer);

    virtual bool init();
    void draw(D3D12_VERTEX_BUFFER_VIEW * lineBuffer, int lineSize, DirectX::XMMATRIX  * view, DirectX::XMMATRIX  * proj);
};