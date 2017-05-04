#include "PercompIncludes.h"
#include "DebugInfo.h"
#include "Graphics.h"

void MenuViewPort::set_default_format(IDWriteTextFormat * format)
{
	pFormat = format;
}

void MenuViewPort::reset(D2D1_POINT_2F anchor)
{
	frame.left = anchor.x;
	frame.top = anchor.y;
	frame.right = frame.left;
	frame.bottom = frame.top;
	line_start.x = frame.left + indents.x;
	line_start.y = frame.top + indents.y;
	lines.clear();
}

void MenuViewPort::setText(std::wstring txt, D2D1_COLOR_F color)
{
	FontFactory::FontDesc desc;
	int len = lstrlenW(txt.c_str())* (int)(pFormat->GetFontSize() * 0.55f);
	if (len > (frame.right - frame.left))
	{
		frame.right = frame.left + len;
	}
	desc.pFormat = pFormat;
	desc.color = color;
	float height = pFormat->GetFontSize() + 6.0f;
	desc.text = txt;
	desc.drawRect = D2D1::RectF(line_start.x, line_start.y, line_start.x + len*2, line_start.y + 32.0f);
	line_start.y += height;
	frame.bottom = (line_start.y + height);
	lines.push_back(desc);
}

void MenuViewPort::draw(Graphics & gfx)
{
	gfx.D2D_DrawFilledRectangle(nullptr, frame, color);
	for (auto& it : lines)
	{
		gfx.D2D_RenderText(it);
	}
}
