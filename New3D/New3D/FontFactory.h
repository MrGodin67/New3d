#pragma once
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <assert.h>
class FontFactory
{
	IDWriteTextFormat *CreateFormat(LPWSTR FontName, float FontSize,
		DWRITE_FONT_WEIGHT FontWeight = DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE FontStyle = DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH FontStretch = DWRITE_FONT_STRETCH_NORMAL,
		LPWSTR FontLocale = L"en-us");
	IDWriteFactory *pFactory;
public:
	typedef std::pair<std::wstring, float> FontPair;
	typedef std::unordered_map<std::string, IDWriteTextFormat*> FontMap;
	struct FontDesc
	{
		std::wstring text = L"\0";
		IDWriteTextFormat *pFormat = nullptr;
		D2D1_RECT_F drawRect = D2D1::RectF();
		D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
		DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT_LEADING;
		FontDesc() {}
		FontDesc(std::wstring text, IDWriteTextFormat* format, D2D1_RECT_F rect, D2D1_COLOR_F color)
			:text(text), pFormat(format), drawRect(rect), color(color)
		{}
	};
	FontFactory();
	~FontFactory();
	bool CreateFormat(const std::string& fontTitle, const FontPair& desc);
	IDWriteTextFormat* GetFormat(const std::string& fontTitle);
private:
	FontMap m_Fonts;
};

