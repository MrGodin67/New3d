#pragma once
#include "ShaderFactory.h"

class ColorLightShader : public ShaderFactory::Shader
{
private:
	const UINT numElements = sizeof(ShaderFactory::InputLayoutDesc::PositionNormalColor32) /
		sizeof(ShaderFactory::InputLayoutDesc::PositionNormalColor32[0]);
public:
	ColorLightShader(ShaderFactory::InitShaderDesc desc);
	~ColorLightShader();
	HRESULT Render(ID3D11DeviceContext * context, ID3D11Buffer* VBuffer, ID3D11Buffer* IBuffer, const UINT vertStructInBytes,
		const int indexCount, const std::vector<ID3D11ShaderResourceView*> textures)override;

};