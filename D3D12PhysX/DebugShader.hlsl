///////////////��� ����////////////////
cbuffer cbBuffer : register(b0)
{
    float4x4 view;
    float4x4 proj;
};

///////////���ؽ� ������ �̷� ���� ����/////
struct VertexInputType
{
    float4 position : POSITION;
    float4 color    : COLOR;
};

//////////�ȼ� ������ �Է� ���� ����////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
};

//////////////////////////////////////////////////////////
//���ؽ� ���̴�...ó�� ���� �Լ�
//////////////////////////////////////////////////////////
PixelInputType vs_main(VertexInputType input)
{
    PixelInputType output;

    ///////////////���ؽ� ���� ��ġ ���//////////////////////////
    output.position = mul(input.position,  view);
    output.position = mul(output.position, proj);

    //////////////////////��»��� ����////////////////////////
    output.color = input.color;

    //////////////�ȼ� ���̴� ���� ����Ÿ////////////////////////
    return output;
}

////////////////////////////////////////////////
// �ȼ� ���̴�..���� ó�� �Լ�
///////////////////////////////////////////////
float4 ps_main(PixelInputType input) : SV_TARGET
{
    return input.color;
}