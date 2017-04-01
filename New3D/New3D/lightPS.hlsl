SamplerState SampleType;


cbuffer LightBuffer
{
	float4 diffuseColor;
	float4 ambientColor;
	float3 lightDirection;
	float padding;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInput
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float3 color : COLOR;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInput input) : SV_TARGET
{

float3 lightDir;
float lightIntensity;
float4 color;
float4 finalColor;



// Invert the light direction for calculations.
lightDir = -lightDirection;

// Calculate the amount of light on this pixel.
lightIntensity = saturate(dot(input.normal, lightDir));
color = ambientColor;
// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.

color += saturate(diffuseColor * lightIntensity);
color = saturate(color);
// Multiply the texture pixel and the final diffuse color to get the final pixel color result.

finalColor = color;

return finalColor;
}