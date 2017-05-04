
//cbuffer MatrixType
//{
//	matrix projection;
//	matrix view;
//	matrix world;
//};

cbuffer constMatrix :register(b0)
{
	matrix projection;
};
cbuffer perFrame :register(b1)
{
	matrix view;
};
cbuffer perObject :register(b2)
{
	matrix world;
};
cbuffer FogBuffer 
{
	float fogStart;
	float fogRange;
	float pabbing1, padding2;
};
struct VertexInput
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float fogFactor : FOG;
};
PixelInput FogLightVertexShader(VertexInput input)
{
	PixelInput output;
	float4 cameraPosition;
	input.position.w = 1.0f;
	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);
	output.tex = input.tex;
	output.normal = mul(input.normal, (float3x3)world);
	output.normal = normalize(output.normal);
	cameraPosition = mul(input.position, world);
	cameraPosition = mul(cameraPosition, view);
	float dist = distance(output.position, cameraPosition);
	output.fogFactor = saturate((dist - fogStart) / fogRange);
	return output;
}