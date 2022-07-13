//Material textures
 #include "LightUtil.hlsli"
//All the textures we need
Texture2D Tex2DArray[] : register(t5, space1);
SamplerState s1 : register(s0);

//Structure of the Cbuffer for materials
struct MaterialCBuffer
{
	float4 baseColorFactor;
	float4 emissiveFactor;
	float4 diffuseFactor;
	float4 specularFactor;
	int albedo_idx;
	int normal_idx;
	int metallic_idx;
	int occlusion_idx;
	int emissive_idx;
	float metallicFactor;
	float roughnessFactor;
	float alphaMask;
	float alphaMaskCutoff;
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

cbuffer MaterialBuffer : register(b2)
{
	MaterialCBuffer materialBuffer;
}

struct PS_OUTPUT
{
	float4 albedoRoughness : SV_TARGET0; //diffuse with roughness on W
	float4 normalMetallic : SV_TARGET1; //Normal with metallic on W
	float4 emissive : SV_TARGET2; //emissive value
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 norm : NORMAL;
	float2 texCoord : TEXCOORD;
	float2 texcoord2 : WORLD0;
};



PS_OUTPUT PackGBuffer(float3 baseColor, float3 normalVS, float4 emissive, float roughness, float metallic)
{
	PS_OUTPUT gBufferOut;
	gBufferOut.albedoRoughness = float4(baseColor.xyz, roughness);
	gBufferOut.normalMetallic = float4(normalVS, metallic);
	gBufferOut.emissive = float4(emissive);
	return gBufferOut;
}


PS_OUTPUT main(VS_OUTPUT input) : SV_TARGET
{
	Texture2D txNormal;
	Texture2D txEmissive;
	Texture2D txAlbedo;
	Texture2D txOcclusion;
	Texture2D txMetallic;
	float4 diffuseColor;
	float3 normal;
	float4 occlusionColor;
	float4 emissiveColor;
	float roughness;
	float metallic;

	//Metallic and Roughness
	if (materialBuffer.metallic_idx > -1)
	{
		//we have a texture, sample and store
		txMetallic = Tex2DArray[materialBuffer.metallic_idx];
		metallic = txMetallic.Sample(s1, input.texCoord).b * materialBuffer.metallicFactor;
		roughness = txMetallic.Sample(s1, input.texCoord).g * materialBuffer.roughnessFactor;
	}
	else
	{
		metallic = 0;
		roughness = 0;
	}
	

	//Albedo
	if (materialBuffer.albedo_idx > -1)
	{
		//we have a texture, sample and store
		txAlbedo = Tex2DArray[materialBuffer.albedo_idx];
		diffuseColor = txAlbedo.Sample(s1, input.texCoord) * materialBuffer.baseColorFactor;
	}
	else
	{
		//No diffuse data
		diffuseColor = materialBuffer.baseColorFactor;
	}
	
	//Normals
	if (materialBuffer.normal_idx > -1)
	{
		//We have a texture, sample and store
		txNormal = Tex2DArray[materialBuffer.normal_idx];
		normal = getNormal(input.norm, txNormal, s1, input.pos, input.texCoord);
	}
	else
	{
		//No normal map, use the models data if there is any
		normal = input.norm.xyz;
	}

	//Occlusion data (if it doesn't already belong to the roughness texture)
	if (materialBuffer.occlusion_idx > -1)
	{
		txOcclusion = Tex2DArray[materialBuffer.occlusion_idx];
		occlusionColor = txOcclusion.Sample(s1, input.texCoord);
	}
	else
	{
		occlusionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	//Emission data
	if (materialBuffer.emissive_idx > -1)
	{
		txEmissive = Tex2DArray[materialBuffer.emissive_idx];
		emissiveColor = txEmissive.Sample(s1, input.texCoord) * materialBuffer.emissiveFactor;
	}
	else
	{
		//No emissive data
		emissiveColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	//Pack the G buffer
	PS_OUTPUT gBufferOut = PackGBuffer(diffuseColor.xyz, normal, occlusionColor, roughness, metallic);

	//Output the G buffer
	return gBufferOut;

}