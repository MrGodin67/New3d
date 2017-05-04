#include "PercompIncludes.h"
#include "Graphics.h"
#include "Geometry.h"
#include "Locator.h"
#include "Model2.h"
#include "Player.h"

Graphics::Graphics(int screenWidth,int screenHeight,HWND & hwnd,
	bool v_sync,bool fullscreen)
	:
	Direct3D((UINT)screenWidth,(UINT)screenHeight,hwnd,v_sync,fullscreen)
{
	Locator::SetDevices(m_pD2DRenderTarget, m_pDevice.Get(), m_pImmediateContext.Get());
	m_FontFactory = std::make_unique<FontFactory>();
	InitShaders();
	CreateTextObjects();
	
}

Graphics::~Graphics()
{
	SafeRelease(ShaderFactory::ConstMatrixBuffers[0]);
	SafeRelease(ShaderFactory::ConstMatrixBuffers[1]);
	SafeRelease(ShaderFactory::ConstMatrixBuffers[2]);
	SafeRelease(ShaderFactory::ConstFogLightBuffers[0]);
	SafeRelease(ShaderFactory::ConstFogLightBuffers[1]);
}

HRESULT Graphics::BeginScene(DirectX::XMFLOAT4 color)
{
	float colors[4] = { color.x,color.y,color.z,color.w };
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, colors);
	m_pImmediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	HRESULT hr = InvokeD2DDeviceResources();

	if (SUCCEEDED(hr))
	{
		m_pD2DRenderTarget->BeginDraw();
		
	}
	
	return hr;
}
HRESULT Graphics::EndScene()
{

	HRESULT hr = S_OK;
	hr = m_pD2DRenderTarget->EndDraw();

	if (hr == D2DERR_RECREATE_TARGET)
	{
     	ReleaseD2DDeviceResources();
	}
	if (m_vsync_enabled)
	{
		hr = m_pSwapChain->Present(1,0);// capped frame rate
	}
	else
	{
		hr = m_pSwapChain->Present(0, 0);// full on flip'n 
	}
	return hr;
}

HRESULT Graphics::RenderPlayerAmmo(PlayerAmmo ammo)
{
	HRESULT hr = S_OK;
	DisableCulling();
	EnableAlphaToCoverageBlending();
	/*XMMATRIX world = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	m_pImmediateContext->UpdateSubresource(ShaderFactory::ConstMatrixBuffers[ShaderFactory::CB_object], 0, nullptr, &XMMatrixTranspose(world), 0, 0);
	
	hr = ShaderFactory::RenderShader<Geometry::LOD_VertexMin>(
		shader,
		m_ammo[0].GetMesh()->GetVertexBuffer(),
		m_ammo[0].GetMesh()->GetIndexBuffer(),
		m_ammo[0].GetMesh()->VerticesCount(),
		tex);*/
	EnableCulling();
	DisableAlphaBlending();;
	return hr;
}

HRESULT Graphics::RenderQuad(DirectX::XMMATRIX * matrix, MeshFactory::Mesh<Geometry::LOD_VertexMin>* mesh,
	ID3D11ShaderResourceView* texture)
{
	HRESULT hr = S_OK;
	DisableCulling();
	EnableAlphaToCoverageBlending();
	
	m_pImmediateContext->UpdateSubresource(ShaderFactory::ConstMatrixBuffers[ShaderFactory::CB_object], 0, nullptr, &XMMatrixTranspose(*matrix), 0, 0);
	std::vector<ID3D11ShaderResourceView*> tex;
	tex.push_back(texture);
	hr = ShaderFactory::RenderShader<Geometry::LOD_VertexMin>(
		&m_Shaders->GetShader<FogLightShader>(),
	mesh->GetVertexBuffer(),
	mesh->GetIndexBuffer(),
	mesh->VerticesCount(),
	tex);
	EnableCulling();
	DisableAlphaBlending();;
	return hr;
}

HRESULT Graphics::RenderTerrain(Terrain * terrain)
{
	HRESULT hr = S_OK;
	XMMATRIX world = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	m_pImmediateContext->UpdateSubresource(ShaderFactory::ConstMatrixBuffers[ShaderFactory::CB_object], 0, nullptr, &XMMatrixTranspose(world), 0, 0);
	
	for (int c = 0; c < terrain->RenderCount(); c++)
	{
		hr = ShaderFactory::RenderShader<Geometry::LOD_VertexMax>(
			&m_Shaders->GetShader<TerrainShader>(),
			terrain->RenderCells(c)->GetVertexBuffer(),
			terrain->RenderCells(c)->GetIndexBuffer(),
			terrain->RenderCells(c)->GetVertexCount(),
			terrain->GetTextures());

	}
	return hr;
}
HRESULT Graphics::RenderModel(Model2 & model, const DirectX::XMMATRIX & matrix)
{
	m_pImmediateContext->UpdateSubresource(ShaderFactory::ConstMatrixBuffers[ShaderFactory::CB_object], 0, nullptr, &XMMatrixTranspose(matrix), 0, 0);
	std::vector<ID3D11ShaderResourceView*> tex;
	HRESULT hr;
	ShaderFactory::Shader * shader = &m_Shaders->GetShader<FogLightShader>();
	//EnableAlphaBlending();
	for (int c = 0; c < model.NumMeshes(); c++)
	{
		tex.clear();
		tex.push_back(model.GetMesh(c)->GetTexture(0));
		hr = shader->Render(m_pImmediateContext.Get(),
			model.GetMesh(c)->GetVertexBuffer(),
			model.GetMesh(c)->GetIndexBuffer(),
			model.GetMesh(c)->VertexBytes(),
			model.GetMesh(c)->IndicesCount(),
			tex);
		assert(hr == S_OK);

	}
	//DisableAlphaBlending();
	return S_OK;
}


// Direct 2D
HRESULT Graphics::CreateTextObjects()
{
	bool result;
	result = m_FontFactory->CreateFormat("Tahoma32", { L"Tahoma", 32.0f });
	if (!result) { return E_FAIL; }
	result = m_FontFactory->CreateFormat("Tahoma22", { L"Tahoma", 22.0f });
	if (!result) { return E_FAIL; }
	result = m_FontFactory->CreateFormat("Tahoma12", { L"Tahoma", 12.0f });
	if (!result) { return E_FAIL; }
	result = m_FontFactory->CreateFormat("Tahoma18", { L"Tahoma", 18.0f });
	if (!result) { return E_FAIL; }
	result = m_FontFactory->CreateFormat("Tahoma10", { L"Tahoma", 10.0f });
	if (!result) { return E_FAIL; }
	result = m_FontFactory->CreateFormat("Tahoma14", { L"Tahoma", 14.0f });
	if (!result) { return E_FAIL; }
	result = m_FontFactory->CreateFormat("Tahoma16", { L"Tahoma", 16.0f });
	if (!result) { return E_FAIL; }
	return S_OK;
}
void Graphics::DrawSprite(D2D1_MATRIX_3X2_F * trans, D2D1_RECT_F & PosSize, ID2D1Bitmap * pSprite, D2D1_RECT_F * ClipRect, float Opacity, D2D1_BITMAP_INTERPOLATION_MODE InterpMode)
{
	if(trans)
	  m_pD2DRenderTarget->SetTransform(trans);
	else
	  m_pD2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	
	
	m_pD2DRenderTarget->DrawBitmap(pSprite, PosSize, Opacity, InterpMode, ClipRect);
	
}
void Graphics::D2D_DrawRectangle(D2D1::Matrix3x2F* matrix, D2D1_RECT_F rect, D2D1_COLOR_F color)
{
	if(!matrix)
		m_pD2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	else
		m_pD2DRenderTarget->SetTransform(matrix);
	
	m_pD2DWhiteBrush->SetColor(color);
	m_pD2DRenderTarget->DrawRectangle(rect, m_pD2DWhiteBrush);
}
void Graphics::D2D_DrawFilledRectangle(D2D1_MATRIX_3X2_F* matrix, D2D1_RECT_F rect, D2D1_COLOR_F color)
{
	if (!matrix)
		m_pD2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	else
		m_pD2DRenderTarget->SetTransform(matrix);

	m_pD2DWhiteBrush->SetColor(color);
	m_pD2DRenderTarget->FillRectangle(rect, m_pD2DWhiteBrush);
	
}
void Graphics::D2D_DrawRoundedRectangle(D2D1_MATRIX_3X2_F* matrix, const D2D1_ROUNDED_RECT & rRect, const D2D1_COLOR_F & color)
{
	if (!matrix)
		m_pD2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	else
		m_pD2DRenderTarget->SetTransform(matrix);

	m_pD2DWhiteBrush->SetColor(color);
	
	m_pD2DRenderTarget->DrawRoundedRectangle(rRect, m_pD2DWhiteBrush);
	
}
void Graphics::D2D_DrawFilledRoundedRectangle(D2D1_MATRIX_3X2_F* matrix, const D2D1_ROUNDED_RECT& rRect, const D2D1_COLOR_F& color)
{
	if (!matrix)
		m_pD2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	else
		m_pD2DRenderTarget->SetTransform(matrix);

	m_pD2DWhiteBrush->SetColor(color);
	m_pD2DRenderTarget->FillRoundedRectangle(rRect, m_pD2DWhiteBrush);
	
}
void Graphics::D2D_RenderText(FontFactory::FontDesc& desc, const D2D1_MATRIX_3X2_F& matrix)
{
	m_pD2DRenderTarget->SetTransform(matrix);
	ID2D1SolidColorBrush *pBrush = m_pD2DWhiteBrush;
	pBrush->SetColor(desc.color);
	UINT strLen = lstrlen(desc.text.c_str());
	desc.pFormat->SetTextAlignment(desc.alignment);
	m_pD2DRenderTarget->DrawTextW(const_cast<WCHAR*>(desc.text.c_str()), strLen, desc.pFormat, desc.drawRect, pBrush);
	m_pD2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	desc.pFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
}
void Graphics::D2D_DrawEllipse(const D2D_POINT_2F& center,const D2D1_POINT_2F& radius, const D2D1_COLOR_F& color,const D2D1_MATRIX_3X2_F& matrix)
{
	
	
	D2D1_ELLIPSE e;
	e.point = center; e.radiusX = radius.x; e.radiusY = radius.y;
	m_pD2DWhiteBrush->SetColor(color);
	m_pD2DRenderTarget->SetTransform(matrix);
	m_pD2DRenderTarget->FillEllipse(e, m_pD2DWhiteBrush);
	m_pD2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
}
void Graphics::D2D1_DrawLine(D2D1_MATRIX_3X2_F* matrix,const D2D_POINT_2F & startPt, const D2D1_POINT_2F & endPt, const D2D1_COLOR_F & color)
{
	if (!matrix)
		m_pD2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	else
		m_pD2DRenderTarget->SetTransform(matrix);

	m_pD2DWhiteBrush->SetColor(color);
	m_pD2DRenderTarget->DrawLine(startPt,endPt, m_pD2DWhiteBrush);
	
}

void Graphics::D2D1_DrawTriangle(D2D1_MATRIX_3X2_F* matrix, const D2D_POINT_2F * Pts, const D2D1_COLOR_F & color)
{
	D2D1_DrawLine(matrix,Pts[0], Pts[1], color);
	D2D1_DrawLine(matrix,Pts[1], Pts[2], color);
	D2D1_DrawLine(matrix,Pts[2], Pts[0], color);

}
FontFactory * Graphics::D2DFonts()
{
	return m_FontFactory.get();
}

void Graphics::InitShaders()
{
	HRESULT hr = ShaderFactory::CreateConstMatrixBuffer();
	hr = ShaderFactory::CreateFogLightBuffer(ShaderFactory::ConstBuffers::DefaultAmbientLight, ShaderFactory::ConstBuffers::DefaultFog);
	m_Shaders = std::make_unique<ShaderFactory::ShaderManager>();
	ShaderFactory::InitShaderDesc desc;
	desc.first.filename = L"lightVS.hlsl";
	desc.first.entryPoint = "LightVertexShader";
	desc.first.version = "latest";

	desc.second.filename = L"lightPS.hlsl";
	desc.second.entryPoint = "LightPixelShader";
	desc.second.version = "latest";
	m_Shaders->AddShader<ColorLightShader>(desc);
	//TERRAIN terrainVS.vs TerrainVertexShader latest terrainPS.ps TerrainPixelShader latest

	desc.first.filename = L"terrainVS.vs";
	desc.first.entryPoint = "TerrainVertexShader";
	desc.first.version = "latest";

	desc.second.filename = L"terrainPS.ps";
	desc.second.entryPoint = "TerrainPixelShader";
	desc.second.version = "latest";
	m_Shaders->AddShader<TerrainShader>(desc);

	desc.first.filename = L"foglightVS.hlsl";
	desc.first.entryPoint = "FogLightVertexShader";
	desc.first.version = "latest";

	desc.second.filename = L"foglightPS.hlsl";
	desc.second.entryPoint = "FogLightPixelShader";
	desc.second.version = "latest";
	m_Shaders->AddShader<FogLightShader>(desc);
}






