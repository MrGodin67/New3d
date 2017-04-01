
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

struct VertexInput
{
	float4 position : POSITION;
	float3 normal : NORMAL;
	float3 color : COLOR;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float3 color : COLOR;
};

PixelInput LightVertexShader(VertexInput input)
{
	PixelInput output;
	input.position.w = 1.0f;
	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);
	output.color = input.color;
	return output;
}