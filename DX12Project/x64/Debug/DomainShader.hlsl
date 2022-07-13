//Generates surface geometry

cbuffer ConstantBuffer : register(b0)
{
	float4x4 mvpMat;
};


struct DS_OUTPUT
{
	float4 vPosition : SV_POSITION;
	float4 vNorm : NORMAL;
	float2 vTexCoord : TEXCOORD;
};

//Output control point
struct HS_CONTROL_POINT_OUTPUT
{
	float4 vPosition : SV_POSITION;
	float4 vNorm : NORMAL;
	float2 vTexCoord : TEXCOORD;
};

//Output Patch Constant data
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor; //[4] for a quad, [3] for a triangle
	float InsideTessFactor			: SV_InsideTessFactor; //[2] for quad
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;

	Output.vPosition = float4(patch[0].vPosition.xyz * domain.x + patch[1].vPosition.xyz * domain.y + patch[2].vPosition.xyz * domain.z, 1);
	Output.vPosition = mul(Output.vPosition, mvpMat);
	Output.vNorm = float4(patch[0].vNorm.xyz * domain.x + patch[1].vNorm.xyz * domain.y + patch[2].vNorm.xyz * domain.z, 1);
	Output.vTexCoord = float2(0.0f, 0.0f);
	return Output;
}
