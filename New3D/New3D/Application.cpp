#include "PercompIncludes.h"
#include "Application.h"
#include "Direct3DWindow.h"



bool Application::InitTerrain()
{
	Terrain::TerrainDesc desc;
	desc.colorMapFilename = "data\\colorm02.bmp";
	desc.height = 257;
	desc.width = 257;
	
	desc.heightOffset = 812.0f;
	desc.position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	desc.heightMapFilename = "data\\output.r16";
	desc.textureFilenames.push_back("data\\dirt01d.tga");
	desc.textureFilenames.push_back("data\\dirt01n.tga");
	m_terrain = std::make_unique<Terrain>(desc);
	if (!m_terrain)
		return false;
	return true;
}

void Application::InitShaders()
{
	HRESULT hr = ShaderFactory::CreateConstMatrixBuffer();
	hr = ShaderFactory::CreateFogLightBuffer(ShaderFactory::ConstBuffers::DefaultAmbientLight, ShaderFactory::ConstBuffers::DefaultFog);
	m_shaders = std::make_unique<ShaderFactory::ShaderManager>();
	ShaderFactory::InitShaderDesc desc;
	desc.first.filename = L"lightVS.hlsl";
	desc.first.entryPoint = "LightVertexShader";
	desc.first.version = "latest";

	desc.second.filename = L"lightPS.hlsl";
	desc.second.entryPoint = "LightPixelShader";
	desc.second.version = "latest";
	m_shaders->AddShader<ColorLightShader>(desc);
	//TERRAIN terrainVS.vs TerrainVertexShader latest terrainPS.ps TerrainPixelShader latest
	
	desc.first.filename = L"terrainVS.vs";
	desc.first.entryPoint = "TerrainVertexShader";
	desc.first.version = "latest";

	desc.second.filename = L"terrainPS.ps";
	desc.second.entryPoint = "TerrainPixelShader";
	desc.second.version = "latest";
	m_shaders->AddShader<TerrainShader>(desc);


}



Application::Application(Direct3DWindow& wnd)
	:Game(wnd)
{
	InitShaders();
	InitTerrain();
	m_shader = std::make_unique<ColorShaderClass>();
	m_shader->Initialize(gfx.GetDevice(), gfx.GetHandle());
	float aspect = (float)(gfx.GetViewport().Width / gfx.GetViewport().Height);
	m_cam.SetProjParams(0.25f*DirectX::XM_PI, aspect, 0.1f, 1000.0f);
	m_cam.SetViewParams(DirectX::XMVectorSet(128.0f, 0.75f, 128.0f, 1.0f), DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f));
	m_cam.SetRotateButtons(false, false, true);
	m_cam.SetEnablePositionMovement(true);
	m_cam.SetEnableYAxisMovement(false);
	m_cam.SetNumberOfFramesToSmoothMouseData(5);
	m_cam.SetScalers(0.004f, 2.0f);
	m_cam.SetViewPortCenter(DirectX::XMFLOAT2((float)(gfx.GetViewport().Width / 2), (float)(gfx.GetViewport().Height / 2)));
	m_cam.SetDrag(false);
	m_cam.SetResetCursorAfterMove(true);
	
	m_cam.FrameMove(0.016f);
	txtDesc.color = D2D1::ColorF(1.0f,1.0f,1.0f,1.0f);
	txtDesc.text = L"Loading Level.. ";
	txtDesc.drawRect = D2D1::RectF((float)(wnd.ScreenWidth() / 2) - 100.0f, (float)(wnd.ScreenHeight() / 2), (float)(wnd.ScreenWidth() / 2)+100.0f, (float)(wnd.ScreenHeight() / 2)+30.0f);
	txtDesc.pFormat = gfx.D2DFonts()->GetFormat("Tahoma12");
	txtDesc.alignment = DWRITE_TEXT_ALIGNMENT_CENTER;
	
	m_player = std::make_unique<Player>(m_cam);
	

}


Application::~Application()
{
	m_shader->Shutdown();
	
	SafeRelease(ShaderFactory::ConstMatrixBuffers[0]);
	SafeRelease(ShaderFactory::ConstMatrixBuffers[1]);
	SafeRelease(ShaderFactory::ConstMatrixBuffers[2]);
	SafeRelease(ShaderFactory::ConstFogLightBuffers[0]);
	SafeRelease(ShaderFactory::ConstFogLightBuffers[1]);
	//HRESULT hr = ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
}

HRESULT Application::OnUpdate(UpdateEventArgs & e)
{
	m_cam.FrameMove(e.ElapsedTime);
	gfx.GetContext()->UpdateSubresource(ShaderFactory::ConstMatrixBuffers[ShaderFactory::CB_frame], 0, nullptr, &XMMatrixTranspose(m_cam.GetViewMatrix()), 0, 0);
	gfx.GetContext()->UpdateSubresource(ShaderFactory::ConstMatrixBuffers[ShaderFactory::CB_projection], 0, nullptr, &XMMatrixTranspose(m_cam.GetProjMatrix()), 0, 0);
	
	m_terrain->FindCellsInView(m_cam.GetFrustum());
	m_player->Update(e.ElapsedTime, m_terrain.get());
	return S_OK;
}

HRESULT Application::OnRender(RenderEventArgs & e)
{
	gfx.BeginScene(DirectX::XMFLOAT4(0.85f, 0.85f, 0.85f, 1.0f));
	//gfx.D2D_RenderText(txtDesc);
	gfx.RenderTerrain(m_terrain.get(), &m_shaders->GetShader<TerrainShader>());
	m_player->RenderDebugStats(gfx);
	gfx.EndScene();
	return S_OK;
}

void Application::OnKeyPressed(KeyEventArgs & e)
{
	if (e.Key == KeyCode::Escape)
		PostQuitMessage(0x0);
}

void Application::OnKeyReleased(KeyEventArgs & e)
{
}

void Application::OnMouseMoved(MouseMotionEventArgs & e)
{
}

void Application::OnMouseButtonPressed(MouseButtonEventArgs & e)
{
}

void Application::OnMouseButtonReleased(MouseButtonEventArgs & e)
{
}

void Application::OnMouseWheel(MouseWheelEventArgs & e)
{
}

void Application::OnWndMessage(WndMsgEventArgs & e)
{
}
