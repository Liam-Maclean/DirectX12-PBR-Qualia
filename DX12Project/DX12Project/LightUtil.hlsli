//Reinhard tonemapping
float3 Reinhard(float3 color)
{
	return color / (1.0 + color);
}

//Gamma correction
float3 GammaCorrection(float3 color)
{
	//Gamma = 2.2
	return pow(color, (float3(1.0, 1.0, 1.0) / 2.2));
}

float3 getNormal(float3 inNormal, Texture2D txNormal, SamplerState s1, float3 worldPos, float2 inUV)
{
	float3 bumpedNormal = txNormal.Sample(s1, inUV).xyz * 2.0 - float3(1.0, 1.0, 1.0);
	float3 q1 = ddx(worldPos);
	float3 q2 = ddy(worldPos);
	float2 st1 = ddx(inUV);
	float2 st2 = ddy(inUV);

	float3 N = normalize(inNormal);
	float3 T = normalize(q1 * st2.y - q2 * st1.y);
	float3 B = normalize(cross(N, T));
	float3x3 TBN = float3x3(T, B, N);

	float3 output = mul(bumpedNormal, TBN);
	return output;
}

// ----------------------------------------------------------------------------
float DistributionGGX(float3 N, float3 H, float roughness)
{
	float PI = 3.141592653589793;
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
float3 fresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}