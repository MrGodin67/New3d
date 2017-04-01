#include "PercompIncludes.h"
#include "ShaderFactory.h"


ID3D11Buffer* ShaderFactory::ConstMatrixBuffers[numConstBufferMatrixType];
ID3D11Buffer* ShaderFactory::ConstFogLightBuffers[numConstBufferFogLightType];


ShaderFactory::ConstBuffers::DirectionalLight ShaderFactory::ConstBuffers::DefaultAmbientLight = 
{
	{1.0f,1.0f,1.0f,1.0f},
	{0.80f,0.80f,0.80f,1.0f},
	{ 0.85f,0.85f,0.85f,1.0f },
	{ 100.0f,4.0f,-128.5f }
};
ShaderFactory::ConstBuffers::Fog ShaderFactory::ConstBuffers::DefaultFog =
{
	18.0f,200.0f
};

ID3D11InputLayout* ShaderFactory::CreateInputLayout(ID3DBlob* pShaderBlob, const D3D11_INPUT_ELEMENT_DESC desc)
{
	HRESULT hr = S_OK;
	ID3D11InputLayout* layout;
	int numElements = sizeof(desc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	if (FAILED(hr = Locator::GetDevice()->CreateInputLayout(&desc, numElements,
		pShaderBlob->GetBufferPointer(),
		sizeof(pShaderBlob->GetBufferPointer()),
		&layout)))
	{
		assert(hr == S_OK);
	}
	return layout;
}
ID3D11SamplerState* ShaderFactory::CreateSamplerState(const D3D11_SAMPLER_DESC& desc)
{
	HRESULT hr = S_OK;
	ID3D11SamplerState* state;
	if (FAILED(hr = Locator::GetDevice()->CreateSamplerState(&desc, &state)))
	{
		assert(hr == S_OK);
	};
	return state;
}
HRESULT ShaderFactory::CreateConstMatrixBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC constantBufferDesc = { 0 };
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	hr = Locator::GetDevice()->CreateBuffer(&constantBufferDesc, nullptr, &ConstMatrixBuffers[CB_projection]);
	if (FAILED(hr))
	{
		goto failed;
	}
	hr = Locator::GetDevice()->CreateBuffer(&constantBufferDesc, nullptr, &ConstMatrixBuffers[CB_frame]);
	if (FAILED(hr))
	{
		goto failed;
	}
	hr = Locator::GetDevice()->CreateBuffer(&constantBufferDesc, nullptr, &ConstMatrixBuffers[CB_object]);
	if (FAILED(hr))
	{
		goto failed;
	}
	goto done;
failed:
	ConstMatrixBuffers[CB_projection]->Release();
	ConstMatrixBuffers[CB_frame]->Release();
	ConstMatrixBuffers[CB_object]->Release();
	return hr;
done:
	return S_OK;
}
HRESULT ShaderFactory::CreateFogLightBuffer(const ConstBuffers::DirectionalLight & light, const ConstBuffers::Fog & fog)
{
	HRESULT hr;
	D3D11_BUFFER_DESC constantBufferDesc = { 0 };
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeof(ConstBuffers::DirectionalLight);
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	D3D11_SUBRESOURCE_DATA subData = { 0 };
	subData.pSysMem = &light;
	hr = Locator::GetDevice()->CreateBuffer(&constantBufferDesc, &subData, &ConstFogLightBuffers[CB_light]);
	if (SUCCEEDED(hr))
	{
		subData.pSysMem = &fog;
		constantBufferDesc.ByteWidth = sizeof(ConstBuffers::Fog);
		hr = Locator::GetDevice()->CreateBuffer(&constantBufferDesc, &subData, &ConstFogLightBuffers[CB_fog]);
		assert(hr == S_OK);
	}
	assert(hr == S_OK);
	return hr;
}



////////////////////////////////////
const  D3D11_BUFFER_DESC ShaderFactory::ConstBuffers::DefaultConst =
{
	0, D3D11_USAGE_DEFAULT,D3D11_BIND_CONSTANT_BUFFER,
	0,0,0
};
 D3D11_BUFFER_DESC ShaderFactory::ConstBuffers::DynamicWrite =
{
	0, D3D11_USAGE_DYNAMIC,D3D11_BIND_CONSTANT_BUFFER,
	D3D11_CPU_ACCESS_WRITE,0,0
};
HRESULT ShaderFactory::ConstBuffers::CreateBuffer(const D3D11_BUFFER_DESC desc, ID3D11Buffer** buffer)
{
	HRESULT hr = 0;
	hr = Locator::GetDevice()->CreateBuffer(&desc, NULL, buffer);
	assert(hr == S_OK);
	return hr;
}

HRESULT ShaderFactory::ConstBuffers::CreateDynamicWrite(const void* data, const UINT byteWidth, ID3D11Buffer ** buffer)
{
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC desc = DynamicWrite;

	D3D11_SUBRESOURCE_DATA subData = { 0 };
	subData.pSysMem = data;
	desc.ByteWidth = byteWidth;
	hr = Locator::GetDevice()->CreateBuffer(&desc, &subData, buffer);
	assert(hr == S_OK);
	
	return hr;
}

HRESULT ShaderFactory::ConstBuffers::CreateDynamicWrite(const UINT byteWidth, ID3D11Buffer ** buffer)
{
	HRESULT hr = 0L;
	D3D11_BUFFER_DESC desc = DynamicWrite;
	desc.ByteWidth = byteWidth;
	hr = Locator::GetDevice()->CreateBuffer(&desc, NULL, buffer);
	assert(hr == S_OK);
	return hr;
}



////////////////////////////////////////////////
const D3D11_SAMPLER_DESC ShaderFactory::SamplerStates::BasicTexture =
{
	D3D11_FILTER_MIN_MAG_MIP_LINEAR,
	D3D11_TEXTURE_ADDRESS_CLAMP,
	D3D11_TEXTURE_ADDRESS_CLAMP,
	D3D11_TEXTURE_ADDRESS_CLAMP,
	0.0f,1,
	D3D11_COMPARISON_ALWAYS,
	{ 0,0,0,0 },0,
	D3D11_FLOAT32_MAX
};
HRESULT ShaderFactory::SamplerStates::CreateSamplerState( const D3D11_SAMPLER_DESC& desc, ID3D11SamplerState** state)
{
	HRESULT hr = S_OK;

	if (FAILED(hr = Locator::GetDevice()->CreateSamplerState(&desc, state)))
	{
		return E_FAIL;
	}
	return hr;
}
///////////////////////////////////////////////////////
const D3D11_INPUT_ELEMENT_DESC ShaderFactory::InputLayoutDesc::InstancedWorld32[7] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
};
const D3D11_INPUT_ELEMENT_DESC ShaderFactory::InputLayoutDesc::InstancedPosition32[4] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 }

};
const D3D11_INPUT_ELEMENT_DESC ShaderFactory::InputLayoutDesc::InstancedPV32[5] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }

};
const D3D11_INPUT_ELEMENT_DESC ShaderFactory::InputLayoutDesc::PosTexNorm32[3] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }


};
const D3D11_INPUT_ELEMENT_DESC ShaderFactory::InputLayoutDesc::PositionColor32[2] =
{
	{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	{ "COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 }
};
const D3D11_INPUT_ELEMENT_DESC ShaderFactory::InputLayoutDesc::PositionNormalColor32[3] =
{
	{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 }
};
const D3D11_INPUT_ELEMENT_DESC ShaderFactory::InputLayoutDesc::PositionTex32[2] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
const D3D11_INPUT_ELEMENT_DESC ShaderFactory::InputLayoutDesc::PosTexNormTanBiClr32[6] =
{
	{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
	{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
	{ "TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
	{ "BINORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
	{ "COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 }

	
};
HRESULT ShaderFactory::InputLayoutDesc::CreateLayout(const D3D11_INPUT_ELEMENT_DESC* desc, const UINT numbElements, ID3DBlob* vertexblob, ID3D11InputLayout** layout)
{
	HRESULT hr = 0;
	
	if (FAILED(hr = Locator::GetDevice()->CreateInputLayout(desc, numbElements,
		vertexblob->GetBufferPointer(),
		vertexblob->GetBufferSize(),layout)))
	{
		assert(hr == S_OK);
	}
	return hr;
}
ShaderFactory::Shader::~Shader()
{
	SafeRelease(m_inputLayout);
	SafeRelease(m_pixelShader);
	SafeRelease(m_vertexShader);
	SafeRelease(m_sampler);
}
