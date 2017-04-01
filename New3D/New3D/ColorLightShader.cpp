#include "PercompIncludes.h"
#include "ColorLightShader.h"

using namespace DirectX;
ColorLightShader::ColorLightShader(ShaderFactory::InitShaderDesc desc)
{
	ID3DBlob* vblob;
	ID3DBlob* pblob;
	m_vertexShader = ShaderFactory::LoadShader<ID3D11VertexShader>(&vblob, desc.first);
	assert(m_vertexShader);// check pointer
	m_pixelShader = ShaderFactory::LoadShader<ID3D11PixelShader>(&pblob, desc.second);
	assert(m_pixelShader);
#if 0
	ShaderFactory::SamplerStates::CreateSamplerState(ShaderFactory::SamplerStates::BasicTexture, &m_sampler);
	assert(m_sampler);
#endif
	ShaderFactory::InputLayoutDesc::CreateLayout(ShaderFactory::InputLayoutDesc::PositionNormalColor32, numElements, vblob, &m_inputLayout);
	assert(m_inputLayout);


	// clean up
	SafeRelease(vblob);
	SafeRelease(pblob);
}

ColorLightShader ::~ColorLightShader()
{


}

HRESULT ColorLightShader::Render(ID3D11DeviceContext * context, ID3D11Buffer* VBuffer, ID3D11Buffer* IBuffer,
	const UINT vertStructInBytes, const int indexCount,
	const std::vector<ID3D11ShaderResourceView*> textures)
{
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &VBuffer, &vertStructInBytes, &offset);
	context->IASetIndexBuffer(IBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	int bufferNumber = 0;
	context->VSSetConstantBuffers(bufferNumber, 3, ShaderFactory::ConstMatrixBuffers);

	bufferNumber = 3;
	context->IASetInputLayout(m_inputLayout);
	context->VSSetShader(m_vertexShader, NULL, 0);
	context->PSSetShader(m_pixelShader, NULL, 0);
	context->DrawIndexed(indexCount, 0, 0);
	return S_OK;
}
