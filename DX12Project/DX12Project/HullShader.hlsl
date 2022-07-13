//Generates subdivision points for the domain shader

struct VS_CONTROL_POINT_OUTPUT
{
	float4 vPosition : SV_POSITION;
	float4 vNorm : NORMAL;
	float2 vTexCoord : TEXCOORD;
};

struct HS_CONTROL_POINT_OUTPUT
{
	float4 vPosition : SV_POSITION;
	float4 vNorm : NORMAL;
	float2 vTexCoord : TEXCOORD;
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor;
	float InsideTessFactor			: SV_InsideTessFactor;
};

cbuffer ConstantBuffer : register(b1)
{
	int tesselationFactor;
	int innerTesselationFactor;
	int padding1;
	int padding2;
};

#define NUM_CONTROL_POINTS 3

//This function tells us how much we are tesselating the current patch
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	//How much do we tesselate the edges
	Output.EdgeTessFactor[0] = tesselationFactor;
	Output.EdgeTessFactor[1] = tesselationFactor;
	Output.EdgeTessFactor[2] = tesselationFactor;

	//How much do we tesselate the inside
	Output.InsideTessFactor = innerTesselationFactor;

	return Output;
}


//For each control point in the patch and outputs any data necessary for later stages of the pipeline
[domain("tri")]
[partitioning("pow2")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_CONTROL_POINT_OUTPUT main( 
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip, 
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	HS_CONTROL_POINT_OUTPUT Output;

	Output.vPosition = ip[i].vPosition;
	Output.vNorm = ip[i].vNorm;
	Output.vTexCoord = ip[i].vTexCoord;

	return Output;
}
