struct VS_INPUT
{
	float4 pos : POSITION0;
	float4 norm : NORMAL;
	float2 texCoord : TEXCOORD;
	float2 texcoord2 : WORLD0;
	float4 joint : WORLD1;
	float4 weight : WORLD2;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 norm : NORMAL;
	float2 texCoord : TEXCOORD;
	float2 texcoord2 : WORLD0;
};

cbuffer constantBuffer : register(b0)
{
	matrix  viewMat;
	matrix  projMat;
	matrix  modelMat;	
	matrix  inverseViewMat;
	matrix  inverseProjMat;
	matrix  invertedTransposeModelMat;
	float4 cameraPos;
};

#define MAX_NUM_JOINTS 128
cbuffer UBONodeBuffer : register (b1)
{
	matrix  mat;
	matrix  jointMatrix[MAX_NUM_JOINTS];
	float jointCount;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.pos.w = 1.0f;
	output.pos.w = 1.0f;
	if (jointCount > 0.0)
	{
		//Mesh is skinned
		float4x4 skinMat = input.weight.x * jointMatrix[int(input.joint.x)] +
			input.weight.y * jointMatrix[int(input.joint.y)] +
			input.weight.z * jointMatrix[int(input.joint.z)] +
			input.weight.w * jointMatrix[int(input.joint.w)];

		output.pos = mul(input.pos, mat);
		output.pos = mul(output.pos, skinMat);
		output.pos = mul(output.pos, modelMat);
		output.pos = mul(output.pos, viewMat);
		output.pos = mul(output.pos, projMat);
	}
	else 
	{
		//No skins
		output.pos = mul(input.pos, mat);
		output.pos = mul(output.pos, modelMat);
		output.pos = mul(output.pos, viewMat);
		output.pos = mul(output.pos, projMat);
	}
	output.norm = mul(input.norm, invertedTransposeModelMat);
	output.texCoord = input.texCoord;
	output.texcoord2 = input.texcoord2;
	return output;
}