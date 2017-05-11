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
	std::unordered_map<std::string, std::vector<FontFactory::FontDesc>> m_items;
	IDWriteTextFormat* pFormat;
	
public:
	MenuViewPort() = default;
	void set_default_format(IDWriteTextFormat* format);
	void reset(D2D1_POINT_2F anchor);
	void setText(std::wstring txt, D2D1_COLOR_F color = {1.0f,1.0f,1.0f,1.0f});
	void draw(class Graphics& gfx);
};

class DebugButtonStrip
{
public:
	struct button_db
	{
		FontFactory::FontDesc text;
		MenuViewPort menu;
		bool enabled;

	};
private:
	std::unordered_map<std::string, button_db> m_items;
	std::vector<std::string> catagory_names;
	IDWriteTextFormat* pFormat;
	D2D1_POINT_2U anchor;
	bool pointIn(D2D1_RECT_F rect, int x, int y)
	{
		return (x > rect.left && x < rect.right && y > rect.top && y < rect.bottom);
	}
public:
	
	DebugButtonStrip(int anchorX, int anchorY, IDWriteTextFormat* format )
		:pFormat(format)
	{
		anchor.x = (unsigned int)(anchorX);
		anchor.y = (unsigned int)(anchorY);
	}
	int onMouseClick(int x, int y)
	{
		int counter = 0;
		for (auto& it : catagory_names)
		{
			if (pointIn(m_items[it].text.drawRect, x, y))
			{
				m_items[it].enabled = !m_items[it].enabled;
				return counter;
			}
			counter++;
		}
		return -1;
	}
	void  addButton(std::string name, std::wstring text)
	{
		
		auto& it = std::find(catagory_names.begin(), catagory_names.end(), name);
		if (it == catagory_names.end())
		{
			button_db btn;
			btn.text.text = text;
			btn.menu.set_default_format(pFormat);
			btn.text.alignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			btn.text.pFormat = pFormat;
			int txtLen = lstrlenW(text.c_str())* (int)(pFormat->GetFontSize() * 0.65f);
			float f_size = pFormat->GetFontSize();
			btn.text.drawRect = D2D1::RectF((float)anchor.x, (float)anchor.y, anchor.x + txtLen, (float)anchor.y+(f_size+4));
			anchor.x = anchor.x + (unsigned int)txtLen + 4;
			catagory_names.push_back(name);
			btn.menu.reset({ btn.text.drawRect.left,btn.text.drawRect.bottom + 4.0f });
			m_items[name] = btn;
		}
		
		
	}
	void addText(std::string catagory, std::wstring text)
	{
		auto& it = std::find(catagory_names.begin(), catagory_names.end(), catagory);
		if (it != catagory_names.end())
		{
		  m_items[catagory].menu.setText(text);
	    }
	}
	void draw(class Graphics& gfx);
};