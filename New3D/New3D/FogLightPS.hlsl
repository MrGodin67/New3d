Texture2D shaderTexture;
SamplerState SampleType;


cbuffer LightBuffer
{
	float4 diffuseColor;
	float4 ambientColor;
	float4 fogColor;
	float3 lightDirection;
	float padding;
};

cbuffer FogBuffer
{
	float fogStart;
	float fogRange;
	float padding1, padding2;
}
//////////////
// TYPEDEFS //
//////////////
struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float fogFactor : FOG;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 FogLightPixelShader(PixelInput input) : SV_TARGET
{
float4 textureColor;
float3 lightDir;
float lightIntensity;
float4 color;
float4 finalColor;

// Sample the pixel color from the texture using the sampler at this texture coordinate location.
textureColor = shaderTexture.Sample(SampleType, input.tex);
//if (textureColor.r == 0.0f && textureColor.g == 0.0f && textureColor.b == 0.0f)
	//discard;
// Invert the light direction for calculations.
lightDir = -lightDirection;

// Calculate the amount of light on this pixel.
lightIntensity = saturate(dot(input.normal, lightDir));
color = ambientColor;
// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.

color += saturate(diffuseColor * lightIntensity);
color = saturate(color);
// Multiply the texture pixel and the final diffuse color to get the final pixel color result.
color = color * textureColor;
finalColor = lerp(color, fogColor, input.fogFactor);

return finalColor;
}