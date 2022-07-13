//G Buffer
#include "LightUtil.hlsli"

Texture2D gAlbedoRoughness : register(t0);
Texture2D gNormalMetallic : register(t1);
Texture2D gEmissive : register(t2);
Texture2D depthTx : register(t3);
TextureCube cubeMap : register(t4);
SamplerState s1 : register(s0);

//Structure of the Cbuffer for materials
struct DirectionalLightCBuffer
{
	float3 lightDir;
	float3 color;
};

struct ShaderDebugValues
{
	int HDREnabled;
	int GammaCorrectionEnabled;
	int viewMode;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 norm : NORMAL;
	float2 texCoord : TEXCOORD;
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

cbuffer DirectionalLightBuffer : register(b3)
{
	DirectionalLightCBuffer directionalLightBuffer;
}

cbuffer DebugShaderValues : register(b4)
{
	ShaderDebugValues debugValues;
}


float3 GetPositionVS(float2 texcoord, float depth)
{
	float4 clipSpaceLocation;
	clipSpaceLocation.x = texcoord.x * 2.0 - 1.0;
	clipSpaceLocation.y = (1 - texcoord.y) * 2.0 - 1.0;
	clipSpaceLocation.z = depth;
	clipSpaceLocation.w = 1.0f;
	float4 homogenousLocation = mul(clipSpaceLocation, inverseProjMat);

	//flip the z? why?
	homogenousLocation.z = -homogenousLocation.z;

	if (depth == 1)
	{
		return float3(0.0, 0.0, 0.0);
	}
	return homogenousLocation.xyz /= homogenousLocation.w;
}

//Return the textured output for a safe test
float4 main(VS_OUTPUT input) : SV_TARGET
{
	float PI = 3.141592653589793;
	float depth = depthTx.Sample(s1, input.texCoord);
	float3 color;
	////if we've hit the skybox
	//if (depth == 1)
	//{
	//	//Get world view position from depth
	//	float3 worldPos = GetPositionVS(input.texCoord, depth);
	//	float3 samplePos = normalize(worldPos - cameraPos);
	//	color = cubeMap.Sample(s1, samplePos).xyz;
	//
	//	return float4 (color, 1.0);
	//}

	//Get world view position from depth
	float3 worldPos = GetPositionVS(input.texCoord, depth);

	//Sample G buffer
	float3 N = gNormalMetallic.Sample(s1, input.texCoord).xyz;
	float3 V = normalize(cameraPos - worldPos);
	float4 albedo = gAlbedoRoughness.Sample(s1, input.texCoord);
	//float roughness = albedo.a;
	float roughness = gNormalMetallic.Sample(s1, input.texCoord).a;
	float metallic = albedo.a;
	//float metallic = gNormalMetallic.Sample(s1, input.texCoord).a;


	//Fresnel effect
	float3 F0 = float3(0.04, 0.04, 0.04);
	F0 = lerp(F0, albedo.xyz, metallic);

	float3 Lo = float3(0.0, 0.0, 0.0);

	//per light radiance
	float3 L = -directionalLightBuffer.lightDir;
	float3 H = normalize(V + L);
	float NdotH = max(dot(N, H), 0.001);

	//Cook-torrence
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

	float3 kS = F;
	float3 kD =  float3(1.0, 1.0, 1.0) - kS;
	kD *= 1.0 - metallic;

	float3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
	float3 specular = numerator / denominator;
	
	// add to outgoing radiance Lo
	float NdotL = max(dot(N, L), 0.0);
	Lo += (kD * albedo / PI + specular) * NdotL * directionalLightBuffer.color;

	//Ambient occlusion
	float ao = gEmissive.Sample(s1, input.texCoord).x;
	float3 ambient = float3(0.20, 0.20, 0.20) * albedo * ao;
	color = ambient + Lo;


	//Gamma correction and HDR. (Reinhard operator)
	//Without this, the scene seems incredibly dark and it looks
	//difficult to capture the high and low details of light intensities.
	//(See https://learnopengl.com/PBR/Lighting)

	if (debugValues.HDREnabled == 1)
	{
		color = Reinhard(color);
	}
	
	if (debugValues.GammaCorrectionEnabled == 1)
	{
		color = GammaCorrection(color);
	}


	if (debugValues.viewMode == 0)
	{
		return float4(color, 1.0);
	}
	else if (debugValues.viewMode == 1)
	{
		return float4(albedo); //Textures
	}
	else if (debugValues.viewMode == 2)
	{
		return float4(N, 1.0); //Normals
	}
	else if (debugValues.viewMode == 3)
	{
		//return float4(1.0, 1.0, 1.0, 1.0);
		return float4(gEmissive.Sample(s1, input.texCoord));
	}
	else if (debugValues.viewMode == 4)
	{
		return float4(worldPos, 1.0);
	}
	

	return float4(color, 1.0);
}