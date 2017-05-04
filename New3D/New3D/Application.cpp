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


Application::Application(Direct3DWindow& wnd)
	:Game(wnd)
{
	
	
	float aspect = (float)(gfx.GetViewport().Width / gfx.GetViewport().Height);
	m_cam.SetProjParams(0.25f*DirectX::XM_PI, aspect, 0.1f, 1000.0f);
	m_cam.SetViewParams(DirectX::XMVectorSet(128.0f, 0.75f, 128.0f, 1.0f), DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f));
	m_cam.SetRotateButtons(false, false, true);
	m_cam.SetEnablePositionMovement(true);
	m_cam.SetEnableYAxisMovement(true);
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
	
	TextureFactory::CreateSRV(&m_textures["player_ammo"], "ammo.png");
	//TextureFactory::CreateSRV(&m_textures["hex_outer"], "data\\texture\\hex_outer.png");
	//TextureFactory::CreateSRV(&m_textures["hex_inner"], "data\\texture\\hex_inner.png");
	TextureFactory::SRV_Package pkg_outer,pkg_inner;
	TextureFactory::CreateSRV(pkg_outer, "data\\texture\\hex_outer.png");
	m_textures["hex_outer"] = pkg_outer.texture;

	TextureFactory::CreateSRV(pkg_inner, "data\\texture\\hex_inner.png");
	m_textures["hex_inner"] = pkg_inner.texture;
	
	m_retical = std::make_unique<Retical>(wnd.ScreenWidth(),wnd.ScreenHeight(),
		 L"data\\texture\\hex_inner2.png", L"data\\texture\\hex_inner.png", 0.25f);

	model.LoadObjectFile("data\\ammo1.obj");
	DirectX::XMFLOAT2 screen = {(float)wnd.ScreenWidth(),(float)wnd.ScreenHeight()};
	DirectX::XMFLOAT2 p = {100.0f,100.0f} ;
	DirectX::XMFLOAT2 result = Utilities::ConvertTo3DScreenSpace(screen, p);
	

	debugDisplay.reset({10.0f,100.0f});
	debugDisplay.set_default_format(gfx.D2DFonts()->GetFormat("Tahoma12"));
	debugDisplay.setText(L"qqqqqq", {0.0f,1.0f,1.0f,1.0f});
	m_timer.reset();
	InitTerrain();
	terrainTime = m_timer.elapsed();
	m_shotTimer.reset();
	int x = 0;
}


Application::~Application()
{
	
	for (auto & it : m_textures)
		SafeRelease(it.second);
}

HRESULT Application::OnUpdate(UpdateEventArgs & e)
{
	m_cam.FrameMove(e.ElapsedTime);
	gfx.GetContext()->UpdateSubresource(ShaderFactory::ConstMatrixBuffers[ShaderFactory::CB_frame], 0, nullptr, &XMMatrixTranspose(m_cam.GetViewMatrix()), 0, 0);
	gfx.GetContext()->UpdateSubresource(ShaderFactory::ConstMatrixBuffers[ShaderFactory::CB_projection], 0, nullptr, &XMMatrixTranspose(m_cam.GetProjMatrix()), 0, 0);
	debugDisplay.reset({ 10.0f,100.0f });
	m_terrain->FindCellsInView(m_cam.GetFrustum());
	
	m_player->Update(e.ElapsedTime, m_terrain.get(),true);

	debugDisplay.setText( L"Camera X (" + wString(FormatDesc(m_cam.GetEyePt2().x,2.0f)) + L") : Y : (" + wString(FormatDesc(m_cam.GetEyePt2().y, 2.0f)) + L") Z : (" + wString(FormatDesc(m_cam.GetEyePt2().z, 2.0f)) + L")");
	
	debugDisplay.setText(L"Terrain Creation:" + std::to_wstring(terrainTime));
	debugDisplay.setText(wString(L"Percision : ") + wString(FormatDesc(100.1234567f,2.0f)));
	
	
	
	static std::wstring shot_data_string = L"";
	static bool targetAquired = false;
	if (fire_buttonDown)
	{
		if (m_shotTimer.elapsed() > 0.52f)
		{
			m_shotTimer.reset();
			Utilities::LineSegment line;
			line.pt0 = m_cam.GetEyePt2();
			DirectX::XMStoreFloat3(&line.pt1, (m_cam.GetEyePt() + m_cam.GetWorldAhead() * 1000.0f));
			m_timer.reset();
			TerrainIntersection inter = m_terrain->RayIntersectInView(line);
			shot_data_string = (wString(L"Find Intersect in terrain : ") + wString(FormatDesc((float)m_timer.elapsed(), 4.0f)));

			if (inter.hit)
			{
				target = DirectX::XMLoadFloat3(&inter.intersectPoint);
				targetAquired = true;
			}
			else
			{
				target = m_cam.GetEyePt() + m_cam.GetWorldAhead() * 100.0f;
				targetAquired = false;
			}
			PlayerAmmo ammo = m_player->FireShot(target);
			if (ammo.active)
				m_ammo.push_back(ammo);
		}
	
		
	}
	m_retical->Update(e.ElapsedTime, m_cam,targetAquired);
	debugDisplay.setText(shot_data_string);
	for (auto& it : m_ammo)
	{
		it.update(e.ElapsedTime, m_cam.GetEyePt2());
	}
	bool trip = false;
	while (!trip)
	{
		auto it = std::find_if(m_ammo.begin(), m_ammo.end(), [](PlayerAmmo& a)
		{ return a.active == false; });
		
		if (it != m_ammo.end())
		{	m_ammo.erase(it); }
		else{ trip = true; 	}
	}
	
	debugDisplay.setText(wString(L"Ammo : ") + wString(m_ammo.size()));
	return S_OK;
}

HRESULT Application::OnRender(RenderEventArgs & e)
{
	gfx.BeginScene(DirectX::XMFLOAT4(0.85f, 0.85f, 0.85f, 1.0f));
	//gfx.D2D_RenderText(txtDesc);
	gfx.RenderTerrain(m_terrain.get());
	//m_player->RenderDebugStats(gfx, m_ammo.size());
	//gfx.RenderModel(model, XMMatrixTranslation(128.0f,0.5f,128.0f));
	for (auto& it : m_ammo)
	{
		gfx.RenderModel(model, it.GetMatrix());
	}
	m_retical->Draw(gfx);
	debugDisplay.draw(gfx);
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
	if (!fire_buttonDown && e.LeftButton)
	{
		
		fire_buttonDown = e.LeftButton;
	}

}

void Application::OnMouseButtonReleased(MouseButtonEventArgs & e)
{
	if (e.Button == e.Left)
		fire_buttonDown = false;
}

void Application::OnMouseWheel(MouseWheelEventArgs & e)
{
}

void Application::OnWndMessage(WndMsgEventArgs & e)
{
}
