struct VS_INPUT
{
	float4 pos : POSITION;
	float4 norm : NORMAL;
	float2 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 norm : NORMAL;
	float2 texCoord : TEXCOORD;
};

cbuffer ConstantBuffer : register(b0)
{
	float4x4 mvpMat;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.pos = input.pos;
	output.norm = float4(0.0f, 0.0f, 0.0f, 1.0f);
	output.texCoord = input.texCoord;
	return output;
} 
