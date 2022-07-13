Texture2D gRTVTexture : register(t0);
SamplerState s1 : register(s0);

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 norm : NORMAL;
	float2 texCoord : TEXCOORD;
};

float3 FXAA(float2 inUV)
{
	float RDC_MIN = 0.015625f;// 1.0 / 64.0;
	float RDC = 0.25f;// 1.0 / 4.0
	float RANGE = 4.0f;
	uint width;
	uint height;

	gRTVTexture.GetDimensions(width, height);
	float2 texelSize = 1.0 / float2(width, height);

	
	//Get the 4 closest pixels nearest to the current pixel being sampled
	float3 rgbNW = gRTVTexture.Sample(s1, inUV, float2(-1, -1)).xyz;
	float3 rgbNE = gRTVTexture.Sample(s1, inUV, float2(+1, -1)).xyz;
	float3 rgbSW = gRTVTexture.Sample(s1, inUV, float2(-1, +1)).xyz;
	float3 rgbSE = gRTVTexture.Sample(s1, inUV, float2(+1, +1)).xyz;

	//Grab the current pixel that we'll blur
	float3 texColor = gRTVTexture.Sample(s1, inUV).xyz;

	//Store and multiply the lumanosity value by the nearest corner values
	float3 luma = float3(0.2627, 0.678, 0.0593);
	float lumaNW = dot(rgbNW, luma);
	float lumaNE = dot(rgbNE, luma);
	float lumaSW = dot(rgbSW, luma);
	float lumaSE = dot(rgbSE, luma);
	float lumaM = dot(texColor, luma);

	//Get the highest and lowest luminosity value
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

	float2 dir;
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

	float2 dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * RDC), RDC_MIN);
	float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
	dir = clamp(dir * rcpDirMin, -RANGE, RANGE) * texelSize;


	float3 rgbA = 0.5 * (
		gRTVTexture.Sample(s1, inUV + dir * (1.0 / 3.0 - 0.5)).rgb +
		gRTVTexture.Sample(s1, inUV + dir * (2.0 / 3.0 - 0.5)).xyz);
	float3 rgbB = rgbA * 0.5 + 0.25 * (
		gRTVTexture.Sample(s1, inUV + dir * -0.5).xyz +
		gRTVTexture.Sample(s1, inUV + dir * 0.5).xyz);


	float lumaB = dot(rgbB, luma);
	float3 color;
	if ((lumaB < lumaMin) || (lumaB > lumaMax))
		color = rgbA;
	else
		color = rgbB;

	return color;

}

float4 main(VS_OUTPUT input) : SV_TARGET
{
	float2 inUv = input.texCoord;
	float3 aaValue = FXAA(inUv);
	
	return float4 (aaValue.xyz, 1);
	//float4 albedo = gRTVTexture.Sample(s1, input.texCoord);
	//return albedo;
}