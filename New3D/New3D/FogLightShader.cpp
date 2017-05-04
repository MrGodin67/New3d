#include "PercompIncludes.h"
#include "FogLightShader.h"
using namespace DirectX;
FogLightShader::FogLightShader(ShaderFactory::InitShaderDesc desc)
{
	
	
	ID3DBlob* vblob;
	ID3DBlob* pblob;
	m_vertexShader = ShaderFactory::LoadShader<ID3D11VertexShader>(&vblob, desc.first);
	assert(m_vertexShader);// check pointer
	m_pixelShader = ShaderFactory::LoadShader<ID3D11PixelShader>(&pblob, desc.second);
	assert(m_pixelShader);
	ShaderFactory::SamplerStates::CreateSamplerState(ShaderFactory::SamplerStates::BasicTexture, &m_sampler);
	assert(m_sampler);
	ShaderFactory::InputLayoutDesc::CreateLayout(ShaderFactory::InputLayoutDesc::PosTexNorm32, numElements,vblob, &m_inputLayout);
	assert(m_inputLayout);
	
	
	// clean up
	SafeRelease(vblob);
	SafeRelease(pblob);
}

FogLightShader::~FogLightShader()
{
	
	
}

HRESULT FogLightShader::Render(ID3D11DeviceContext * context,ID3D11Buffer* VBuffer,ID3D11Buffer* IBuffer,
	const UINT vertStructInBytes,const int indexCount,
	const std::vector<ID3D11ShaderResourceView*> textures)
{ 
	
	// load vertices and index buffers
	
	context->PSSetConstantBuffers(0, 1, &ShaderFactory::ConstFogLightBuffers[ShaderFactory::CB_light]);

	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &VBuffer, &vertStructInBytes, &offset);
	context->IASetIndexBuffer(IBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	int bufferNumber = 0;
	context->VSSetConstantBuffers(bufferNumber, 3, ShaderFactory::ConstMatrixBuffers);
	
	bufferNumber = 3;
	for(UINT i = 0; i < (UINT)textures.size();i++)
	   context->PSSetShaderResources(i, 1, &textures[i]);
	context->VSSetConstantBuffers(bufferNumber, 1, &ShaderFactory::ConstFogLightBuffers[ShaderFactory::CB_fog]);
	context->IASetInputLayout(m_inputLayout);
	context->VSSetShader(m_vertexShader, NULL, 0);
	context->PSSetShader(m_pixelShader, NULL, 0);
	context->PSSetSamplers(0, 1, &m_sampler);
	context->DrawIndexed(indexCount, 0, 0);
	return S_OK;
}
