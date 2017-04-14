#include "PercompIncludes.h"
#include "Application.h"
#include "Direct3DWindow.h"

bool doThis(char* txt)
{
	app->InitShaders();
	app->loaded =  app->InitTerrain(txt);
	return app->loaded;
}

void Application::InitShaders()
{
	HRESULT hr = ShaderFactory::CreateConstMatrixBuffer();
	if (FAILED(hr))
	{
		int x = 0;
	}
	m_shaders = std::make_unique<ShaderFactory::ShaderManager>();
	ShaderFactory::InitShaderDesc desc;
	desc.first.filename = L"lightVS.hlsl";
	desc.first.entryPoint = "LightVertexShader";
	desc.first.version = "latest";

	desc.second.filename = L"lightPS.hlsl";
	desc.second.entryPoint = "LightPixelShader";
	desc.second.version = "latest";
	m_shaders->AddShader<ColorLightShader>(desc);




}

bool Application::InitTerrain(char* fn)
{
	if (!m_terrain->Initialize(gfx.GetDevice(),fn))// "data\\setup.txt"))
	{
		return false;
	};
	return true;
}

Application::Application(Direct3DWindow& wnd)
	:Game(wnd)
{
	app = this;
	
	m_terrain = std::make_unique<TerrainClass>();
	fut = std::async(doThis,(char*)"data\\setup.txt");
	//InitTerrain("data\\setup.txt");
	m_shader = std::make_unique<ColorShaderClass>();
	m_shader->Initialize(gfx.GetDevice(), gfx.GetHandle());
	float aspect = (float)(gfx.GetViewport().Width / gfx.GetViewport().Height);
	m_cam.SetProjParams(0.25f*DirectX::XM_PI, aspect, 0.1f, 1000.0f);
	m_cam.SetViewParams(DirectX::XMVectorSet(3.0f, 10.75f, 3.0f, 1.0f), DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f));
	m_cam.SetRotateButtons(false, false, true);
	m_cam.SetEnablePositionMovement(true);
	m_cam.SetEnableYAxisMovement(true);
	m_cam.SetNumberOfFramesToSmoothMouseData(12);
	m_cam.SetScalers(0.004f, 12.0f);
	m_cam.SetViewPortCenter(DirectX::XMFLOAT2((float)(gfx.GetViewport().Width / 2), (float)(gfx.GetViewport().Height / 2)));
	m_cam.SetDrag(true);
	m_cam.SetResetCursorAfterMove(true);
	
	m_cam.FrameMove(0.016f);
	txtDesc.color = D2D1::ColorF(1.0f,1.0f,1.0f,1.0f);
	txtDesc.text = L"Loading Level.. ";
	txtDesc.drawRect = D2D1::RectF((float)(wnd.ScreenWidth() / 2) - 100.0f, (float)(wnd.ScreenHeight() / 2), (float)(wnd.ScreenWidth() / 2)+100.0f, (float)(wnd.ScreenHeight() / 2)+30.0f);
	txtDesc.pFormat = gfx.D2DFonts()->GetFormat("Tahoma12");
	txtDesc.alignment = DWRITE_TEXT_ALIGNMENT_CENTER;
	
	

}


Application::~Application()
{
	m_shader->Shutdown();
	m_terrain->Shutdown();
	SafeRelease(ShaderFactory::ConstMatrixBuffers[0]);
	SafeRelease(ShaderFactory::ConstMatrixBuffers[1]);
	SafeRelease(ShaderFactory::ConstMatrixBuffers[2]);
	//HRESULT hr = ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
}

HRESULT Application::OnUpdate(UpdateEventArgs & e)
{
	m_cam.FrameMove(e.ElapsedTime);
	gfx.GetContext()->UpdateSubresource(ShaderFactory::ConstMatrixBuffers[ShaderFactory::CB_frame], 0, nullptr, &XMMatrixTranspose(m_cam.GetViewMatrix()), 0, 0);
	gfx.GetContext()->UpdateSubresource(ShaderFactory::ConstMatrixBuffers[ShaderFactory::CB_frame], 0, nullptr, &XMMatrixTranspose(m_cam.GetProjMatrix()), 0, 0);
	
	return S_OK;
}

HRESULT Application::OnRender(RenderEventArgs & e)
{
	gfx.BeginScene(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	gfx.EnableWireframe();
	//gfx.DisableCulling();
	
	if (loaded)
	{
		m_terrain->Render(gfx.GetContext());
		DirectX::XMMATRIX world = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		m_shader->Render(gfx.GetContext(), m_terrain->GetIndexCount(), world, m_cam.GetViewMatrix(), gfx.m_projectionMatrix);
	}
	else
	{
		gfx.D2D_RenderText(txtDesc);
	}
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
