#pragma once

#include "ShaderFactory.h"

class FogLightShader : public ShaderFactory::Shader
{
private:
	
	

	const UINT numElements = sizeof(ShaderFactory::InputLayoutDesc::PosTexNorm32) /
		sizeof(ShaderFactory::InputLayoutDesc::PosTexNorm32[0]);
public:
	FogLightShader(ShaderFactory::InitShaderDesc desc);
	~FogLightShader();
	HRESULT Render(ID3D11DeviceContext * context, ID3D11Buffer* VBuffer, ID3D11Buffer* IBuffer, const UINT vertStructInBytes, 
		const int indexCount,const std::vector<ID3D11ShaderResourceView*> textures)override;
	
};