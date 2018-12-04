///////////////상수 버퍼////////////////
cbuffer cbBuffer : register(b0)
{
    float4x4 view;
    float4x4 proj;
};

///////////버텍스 세이터 이력 형식 지정/////
struct VertexInputType
{
    float4 position : POSITION;
    float4 color    : COLOR;
};

//////////픽셀 세이터 입력 형식 지정////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
};

//////////////////////////////////////////////////////////
//버텍스 세이더...처리 메인 함수
//////////////////////////////////////////////////////////
PixelInputType vs_main(VertexInputType input)
{
    PixelInputType output;

    ///////////////버텍스 월드 위치 계산//////////////////////////
    output.position = mul(input.position,  view);
    output.position = mul(output.position, proj);

    //////////////////////출력색상 지정////////////////////////
    output.color = input.color;

    //////////////픽셀 세이더 전달 데이타////////////////////////
    return output;
}

////////////////////////////////////////////////
// 픽셀 세이더..메인 처리 함수
///////////////////////////////////////////////
float4 ps_main(PixelInputType input) : SV_TARGET
{
    return input.color;
}