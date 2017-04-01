#pragma once
#include <d3d11.h>
#include <assert.h>
#include "Locator.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <typeinfo>
#include <typeindex>
#include <DirectXMath.h>
class ShaderFactory
{

public:
	

	// shader interface class
	class Shader
	{
		
	protected:
		ID3D11PixelShader* m_pixelShader = nullptr;
		ID3D11VertexShader* m_vertexShader = nullptr;
		ID3D11InputLayout* m_inputLayout = nullptr;
		ID3D11SamplerState* m_sampler = nullptr;
	public:
		Shader() {};
		virtual ~Shader();
		virtual void SetVisibleCount(const UINT count) {}
		virtual HRESULT Render(ID3D11DeviceContext * context, ID3D11Buffer* VBuffer, ID3D11Buffer* IBuffer, 
			const UINT vertStructInBytes, const int indexCount,
			const std::vector<ID3D11ShaderResourceView*> textures) = 0;
	};
	struct LoadShaderDesc
	{
		std::wstring filename;
		std::string entryPoint;
		std::string version;
		LoadShaderDesc() {}
		LoadShaderDesc(std::wstring fn, std::string entrypt, std::string version = "latest")
			:filename(fn), entryPoint(entrypt), version(version)
		{}
	};
	typedef std::pair<LoadShaderDesc, LoadShaderDesc> InitShaderDesc;
	//////////////////////////
	// Shader Manager
	/////////////////////////
	class ShaderManager
	{
		std::unordered_map<std::type_index, std::unique_ptr<Shader>> m_shaders;
	public:
		ShaderManager() {}
		~ShaderManager() {}
		template<typename SHADER>
		SHADER& AddShader(const InitShaderDesc& desc )
		{
			std::type_index index(typeid(SHADER));
			if (m_shaders.count(std::type_index(typeid(SHADER))) != 0)
			{
				throw std::invalid_argument("shader already exists");
			}

			m_shaders[index] = std::make_unique<SHADER>(desc);
			return static_cast<SHADER&>(*m_shaders[index]);

		}
		template<typename SHADER>
		SHADER& GetShader()
		{
			std::type_index index(typeid(SHADER));
			if (m_shaders.count(std::type_index(typeid(SHADER))) == 0)
			{
				throw std::invalid_argument("Non-existant shader");
			};
			return static_cast<SHADER&>(*m_shaders[index]);
		}
		
	};
	// enums / structs
	enum ConstBufferMatrixType
	{
		CB_projection,
		CB_frame,
		CB_object,
		numConstBufferMatrixType

	};

	static ID3D11Buffer* ConstMatrixBuffers[numConstBufferMatrixType];
	enum ConstBufferFogLightType
	{
		CB_fog,
		CB_light,
		numConstBufferFogLightType
	};
	static ID3D11Buffer* ConstFogLightBuffers[numConstBufferFogLightType];
	
	struct alignas(16) MatrixBuffer
	{
		DirectX::XMMATRIX matrix;
	};
	
	class ConstBuffers
	{
	public:
		static const  D3D11_BUFFER_DESC DefaultConst;
		static D3D11_BUFFER_DESC DynamicWrite;
		
		static HRESULT CreateBuffer(const D3D11_BUFFER_DESC desc, ID3D11Buffer** buffer);
		static HRESULT CreateDynamicWrite(const void* data,const UINT byteWidth,ID3D11Buffer** buffer);
		static HRESULT CreateDynamicWrite(const UINT byteWidth, ID3D11Buffer** buffer);

		struct alignas(16) MatrixType
		{
			DirectX::XMMATRIX projection;
			DirectX::XMMATRIX view;
			DirectX::XMMATRIX world;
		};
		struct alignas(16) DirectionalLight
		{
			DirectX::XMFLOAT4 diffuse;
			DirectX::XMFLOAT4 ambient;
			DirectX::XMFLOAT4 fogColor;
			DirectX::XMFLOAT3 direction;
			float padding;// ensure aligned
			DirectionalLight() {}
			DirectionalLight(DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 ambient, DirectX::XMFLOAT4 fogColor, DirectX::XMFLOAT3 direction)
				:diffuse(diffuse), ambient(ambient), fogColor(fogColor),direction(direction), padding(0.0f)
			{}
			
		};
		struct alignas(16) Fog
		{
			float start;
			float range;
			float padding1, padding2;
			Fog() {}
			Fog(float start, float range)
				:start(start), range(range), padding1(0.0f), padding2(0.0f)
			{}
		};
	public:
		static DirectionalLight DefaultAmbientLight;
		static Fog DefaultFog;
	};
	
	class SamplerStates
	{
	public:
		static const D3D11_SAMPLER_DESC BasicTexture;
		static HRESULT CreateSamplerState(const D3D11_SAMPLER_DESC& desc, ID3D11SamplerState** state);
	};
	class InputLayoutDesc
	{
	public:
		const static  D3D11_INPUT_ELEMENT_DESC InstancedPosition32[4];
		const static  D3D11_INPUT_ELEMENT_DESC InstancedPV32[5];
		const static  D3D11_INPUT_ELEMENT_DESC InstancedWorld32[7];
		const static  D3D11_INPUT_ELEMENT_DESC PosTexNorm32[3];
		const static  D3D11_INPUT_ELEMENT_DESC PositionColor32[2];
		const static  D3D11_INPUT_ELEMENT_DESC PositionNormalColor32[3];
		const static  D3D11_INPUT_ELEMENT_DESC PositionTex32[2];
		const static  D3D11_INPUT_ELEMENT_DESC PosTexNormTanBiClr32[6];
		static HRESULT CreateLayout(const D3D11_INPUT_ELEMENT_DESC* desc,const UINT numbElements,ID3DBlob* vertexblob, ID3D11InputLayout** layout);
	};
	///////////////////////////////////////////////////////////
	// template functions
	///////////////////////////////////////////////////////////
	template< class ShaderClass >
	static std::string GetLatestProfile();

	template<>
	static std::string GetLatestProfile<ID3D11VertexShader>()
	{
		assert(Locator::GetDevice());

		// Query the current feature level:
		D3D_FEATURE_LEVEL featureLevel = Locator::GetDevice()->GetFeatureLevel();

		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_1:
		case D3D_FEATURE_LEVEL_11_0:
		{
			return "vs_5_0";
		}
		break;
		case D3D_FEATURE_LEVEL_10_1:
		{
			return "vs_4_1";
		}
		break;
		case D3D_FEATURE_LEVEL_10_0:
		{
			return "vs_4_0";
		}
		break;
		case D3D_FEATURE_LEVEL_9_3:
		{
			return "vs_4_0_level_9_3";
		}
		break;
		case D3D_FEATURE_LEVEL_9_2:
		case D3D_FEATURE_LEVEL_9_1:
		{
			return "vs_4_0_level_9_1";
		}
		break;
		} // switch( featureLevel )

		return "";
	}
	template<>
	static std::string GetLatestProfile<ID3D11PixelShader>()
	{
		assert(Locator::GetDevice());

		// Query the current feature level:
		D3D_FEATURE_LEVEL featureLevel = Locator::GetDevice()->GetFeatureLevel();
		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_1:
		case D3D_FEATURE_LEVEL_11_0:
		{
			return "ps_5_0";
		}
		break;
		case D3D_FEATURE_LEVEL_10_1:
		{
			return "ps_4_1";
		}
		break;
		case D3D_FEATURE_LEVEL_10_0:
		{
			return "ps_4_0";
		}
		break;
		case D3D_FEATURE_LEVEL_9_3:
		{
			return "ps_4_0_level_9_3";
		}
		break;
		case D3D_FEATURE_LEVEL_9_2:
		case D3D_FEATURE_LEVEL_9_1:
		{
			return "ps_4_0_level_9_1";
		}
		break;
		}
		return "";
	}


	template< class ShaderClass >
	static ShaderClass* CreateShader( ID3DBlob* pShaderBlob, ID3D11ClassLinkage* pClassLinkage);
	template<>
	static ID3D11VertexShader* CreateShader<ID3D11VertexShader>( ID3DBlob* pShaderBlob, ID3D11ClassLinkage* pClassLinkage)
	{
		assert(Locator::Locator::GetDevice());

		HRESULT hr = S_OK;
		ID3D11VertexShader* pVertexShader = nullptr;
		hr = Locator::Locator::GetDevice()->CreateVertexShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pClassLinkage, &pVertexShader);
		assert(hr == S_OK);
		return pVertexShader;
	}
	template<>
	static ID3D11PixelShader* CreateShader<ID3D11PixelShader>( ID3DBlob* pShaderBlob, ID3D11ClassLinkage* pClassLinkage)
	{
		assert(Locator::GetDevice());

		HRESULT hr = S_OK;
		ID3D11PixelShader* pPixelShader = nullptr;
		hr = Locator::GetDevice()->CreatePixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pClassLinkage, &pPixelShader);
		assert(hr == S_OK);
		return pPixelShader;
	}


	template< class ShaderClass >
	static ShaderClass* LoadShader(ID3DBlob** pShaderBlob, const LoadShaderDesc& desc)// const std::wstring& fileName, const std::string& entryPoint, const std::string& _profile)
	{
		ID3DBlob* pErrorBlob = nullptr;
		ShaderClass* pShader = nullptr;

		std::string profile = desc.version;
		if (profile == "latest")
		{
			profile = GetLatestProfile<ShaderClass>();
		}
		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
		flags |= D3DCOMPILE_DEBUG;
#endif

		HRESULT hr = D3DCompileFromFile(desc.filename.c_str(), nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, desc.entryPoint.c_str(), profile.c_str(),
			flags, 0, pShaderBlob, &pErrorBlob);

		if (FAILED(hr))
		{
			if (pErrorBlob)
			{
				std::string errorMessage = (char*)pErrorBlob->GetBufferPointer();
				OutputDebugStringA(errorMessage.c_str());

				SafeRelease(*pShaderBlob);
				SafeRelease(pErrorBlob);
			}

			return false;
		}
		pShader = CreateShader<ShaderClass>(*pShaderBlob, nullptr);
		SafeRelease(pErrorBlob);

		return pShader;
	}
	template<typename STRUCT>
	static HRESULT RenderShader(Shader* shader, ID3D11Buffer* VBuffer, ID3D11Buffer* IBuffer, const int indexCount, std::vector<ID3D11ShaderResourceView*> textures)
	{
		const UINT sizeInBytes = sizeof(STRUCT);
		return shader->Render(Locator::GetContext(), VBuffer, IBuffer, sizeInBytes, indexCount, textures);
	}
	///////////////////////////////////////////////////
	// non template
	/////////////////////////////////////////////////////
	static ID3D11InputLayout* CreateInputLayout( ID3DBlob* pShaderBlob, const D3D11_INPUT_ELEMENT_DESC desc);
	static ID3D11SamplerState* CreateSamplerState( const D3D11_SAMPLER_DESC& desc);
	static HRESULT CreateConstMatrixBuffer();
	static HRESULT CreateFogLightBuffer(const ConstBuffers::DirectionalLight& light,const ConstBuffers::Fog& fog);
	
};

