#pragma once
#include <d2d1.h>
#include "FontFactory.h"
#include "Timer.h"
class MenuViewPort
{
private:
	D2D1_POINT_2F indents = {10.0f,10.0f};
	D2D1_POINT_2F line_start = { 0.0f,0.0f };
	D2D1_RECT_F frame;
	D2D1_COLOR_F color = { 0.0f,0.0f,0.0f,0.75f };
	std::vector<FontFactory::FontDesc> lines;
	IDWriteTextFormat* pFormat;
	
public:
	MenuViewPort() = default;
	void set_default_format(IDWriteTextFormat* format);
	void reset(D2D1_POINT_2F anchor);
	void setText(std::wstring txt, D2D1_COLOR_F color = {1.0f,1.0f,1.0f,1.0f});
	void draw(class Graphics& gfx);
};