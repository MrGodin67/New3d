#include "PercompIncludes.h"
#include "Retical.h"
#include "Utilities.h"
#include "Graphics.h"
#include "Camera3D_2.h"

Retical::Retical(int screenWidth, int screenHeight, std::wstring outerFilename, std::wstring innerFilename,float scale)
	:
	scale(scale)
{
	m_outterRingImage = TextureFactory::D2D1BitmapTexture::CreateBitmap(outerFilename);
	m_innerRingImage = TextureFactory::D2D1BitmapTexture::CreateBitmap(innerFilename);
	
	float width = m_outterRingImage->GetSize().width;
	float height = m_outterRingImage->GetSize().height;
	frame.left = (screenWidth / 2) - (width / 2);
	frame.right = (screenWidth / 2) + (width / 2);
	frame.top = (screenHeight / 2) - (height / 2);
	frame.bottom = (screenHeight / 2) + (height / 2);
	center.x = (float)(screenWidth / 2);
	center.y = (float)(screenHeight / 2);
	m_state = ReticalState::scaling_out;
	m_currentImage = m_innerRingImage;
}

Retical::~Retical()
{
	SafeRelease(m_innerRingImage);
	SafeRelease(m_outterRingImage);
}

void Retical::Update(float dt, CFirstPersonCamera& cam,bool aquired_target)
{
	DirectX::XMVECTOR org, vel;
	Utilities::MapMouseToWorld(org, vel, cam.GetProjMatrix(), cam.GetViewMatrix(),
	{ frame.right - frame.left,frame.bottom - frame.top }, { center.x,center.y });
	if (aquired_target)
		m_currentImage = m_innerRingImage;
	else
		m_currentImage = m_outterRingImage;
	switch (m_state)
	{
	case ReticalState::none:
	{
		int d = 0;
	}
		break;
	case ReticalState::scaling_in:
	{
		static float scale_out_timer = 0.0f;
		scale_out_timer += dt;

		scale = Utilities::map(scale_out_timer, 1.0f, 0.0f, 0.25f, 0.35f);
		D2D1_SIZE_F size;
		if (scale <= 0.25f)
		{
			scale_out_timer = 0.0f;
			scale = 0.25f;
			m_state = ReticalState::scaling_out;

		}
		else
		{
			m_matScale = D2D1::Matrix3x2F::Scale(D2D1::SizeF(scale, scale), center);
		}
	}
		break; 
	case ReticalState::scaling_out:
	{
		static float scale_out_timer = 0.0f;
		scale_out_timer += dt*2.0f;
		
		scale = Utilities::map(scale_out_timer, 0.0f, 1.0f, 0.25f, 0.35f);
		D2D1_SIZE_F size;
		if (scale >= 0.35f)
		{
			scale_out_timer = 0.0f;
			scale = 0.35f;
			m_state = ReticalState::scaling_in;

		}
		else
		{
			m_matScale = D2D1::Matrix3x2F::Scale(D2D1::SizeF(scale, scale), center);
		}
	}
		break;
	case ReticalState::rotate:
	{
		
		static float rot = 0.0f;
		rot += dt;
		angle = Utilities::map(rot, 0.0f, 1.0f, 0.0f, 60.0f);
		if (angle >= 60.0f)
		{
			m_state = ReticalState::scaling_in;
			rot = 0.0f;
			angle = 0.0f;
		}
		else
		{
			
		}
	}
		break;
	}
}

void Retical::Draw(Graphics & gfx)
{
	gfx.DrawSprite(&D2D1::Matrix3x2F::Scale(D2D1::SizeF(0.25f, 0.25f), center) , frame, m_currentImage);
	gfx.DrawSprite(&getMatrix(), frame, m_currentImage,0,0.75f);
}

D2D1_MATRIX_3X2_F Retical::getMatrix()
{
	
	return m_matScale * D2D1::Matrix3x2F::Rotation(angle,center)*D2D1::Matrix3x2F::Identity();
}
