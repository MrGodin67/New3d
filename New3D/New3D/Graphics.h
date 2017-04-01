#pragma once
#include "Direct3D_2D.h"
#include "FontFactory.h"
#include "ShaderFactory.h"
#include "Locator.h"
class Graphics : public Direct3D
{
public:
	
	HRESULT CreateTextObjects();
	
public:
	Graphics(int screenWidth, int screenHeight, HWND & hwnd,
		bool v_sync, bool fullscreen);
	~Graphics();
	HRESULT BeginScene(DirectX::XMFLOAT4 color);
	HRESULT EndScene();
	HRESULT RenderModel(class Model2& model, const DirectX::XMMATRIX& matrix, ShaderFactory::Shader* shader);
	///////////
	// Direct 2D
	///////////
	void DrawSprite(D2D1_MATRIX_3X2_F * trans, D2D1_RECT_F &PosSize, ID2D1Bitmap *pSprite, D2D1_RECT_F *ClipRect = nullptr,
		float Opacity = 1.0f,D2D1_BITMAP_INTERPOLATION_MODE InterpMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
	void D2D_DrawRectangle(D2D1::Matrix3x2F* matrix, D2D1_RECT_F rect, D2D1_COLOR_F);
	void D2D_DrawFilledRectangle(D2D1_MATRIX_3X2_F* matrix, D2D1_RECT_F rect, D2D1_COLOR_F);
	void D2D_DrawRoundedRectangle(D2D1_MATRIX_3X2_F* matrix, const D2D1_ROUNDED_RECT& rRect, const D2D1_COLOR_F& color);
	void D2D_DrawFilledRoundedRectangle(D2D1_MATRIX_3X2_F* matrix, const D2D1_ROUNDED_RECT& rRect, const D2D1_COLOR_F& color);
	void D2D_RenderText(FontFactory::FontDesc& desc, const D2D1_MATRIX_3X2_F& matrix= D2D1::Matrix3x2F::Identity());
	void D2D_DrawEllipse(const D2D_POINT_2F& center, const D2D1_POINT_2F& radius, const D2D1_COLOR_F& color,const D2D1_MATRIX_3X2_F& matrix = D2D1::Matrix3x2F::Identity());
	void D2D1_DrawLine(D2D1_MATRIX_3X2_F* matrix, const D2D_POINT_2F & startPt, const D2D1_POINT_2F & endPt, const D2D1_COLOR_F & color);
	void D2D1_DrawTriangle(D2D1_MATRIX_3X2_F* matrix, const D2D_POINT_2F* Pts, const D2D1_COLOR_F& color);
	FontFactory* D2DFonts();
private:
	std::unique_ptr<FontFactory> m_FontFactory;
};