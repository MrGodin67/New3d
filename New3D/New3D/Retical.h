#pragma once
#include "TextureFactory.h"
#include "Quad.h"
enum class ReticalState
{
	scaling_out,
	scaling_in,
	rotate,
	none
};
class Retical
{
private:
	ID2D1Bitmap* m_innerRingImage;
	ID2D1Bitmap* m_outterRingImage;
	ID2D1Bitmap* m_currentImage;
	D2D1_RECT_F frame;
	D2D1_POINT_2F center;
	D2D1_MATRIX_3X2_F m_matScale;
	D2D1_MATRIX_3X2_F m_matRot;
	D2D1_MATRIX_3X2_F m_matTrans;
	float angle = 0.0f;
	float scale = 1.0f;
	ReticalState m_state = ReticalState::scaling_out;
public:
	Retical(int screenWidth,int screenHeight, std::wstring outerFilename, std::wstring innerFilename,float scale);
	~Retical();
	void Update(float dt, class CFirstPersonCamera& cam, bool aquired_target);
	void Draw(class Graphics& gfx);
	D2D1_MATRIX_3X2_F getMatrix();
};