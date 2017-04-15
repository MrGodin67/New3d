#pragma once

#include "ShaderFactory.h"

class TerrainShader : public ShaderFactory::Shader
{
private:
	ID3D11Buffer* m_matrixBuffer = nullptr;
	const UINT numElements = sizeof(ShaderFactory::InputLayoutDesc::PosTexNormTanBiClr32) /
		sizeof(ShaderFactory::InputLayoutDesc::PosTexNormTanBiClr32[0]);
public:
	TerrainShader(ShaderFactory::InitShaderDesc desc);
	~TerrainShader();
	HRESULT Render(ID3D11DeviceContext * context, ID3D11Buffer* VBuffer, ID3D11Buffer* IBuffer, const UINT vertStructInBytes,
		const int indexCount, const std::vector<ID3D11ShaderResourceView*> textures)override;

};