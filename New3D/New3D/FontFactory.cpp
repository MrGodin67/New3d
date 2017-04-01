#include "PercompIncludes.h"
#include "FontFactory.h"


FontFactory::FontFactory()
{
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		(IUnknown**)(&pFactory));
}


FontFactory::~FontFactory()
{
	pFactory->Release();
	for (auto& it : m_Fonts)
		it.second->Release();
}
bool FontFactory::CreateFormat(const std::string & fontTitle, const FontPair & desc)
{
	m_Fonts[fontTitle] = CreateFormat(const_cast<LPWSTR>(desc.first.c_str()), desc.second);
	assert(m_Fonts[fontTitle]);
	return m_Fonts[fontTitle] ? true : false;
}
IDWriteTextFormat * FontFactory::GetFormat(const std::string & fontTitle)
{
	assert(m_Fonts[fontTitle]);
	return m_Fonts[fontTitle];
}
IDWriteTextFormat *FontFactory::CreateFormat(LPWSTR FontName, float FontSize,
	DWRITE_FONT_WEIGHT FontWeight, DWRITE_FONT_STYLE FontStyle,
	DWRITE_FONT_STRETCH FontStretch, LPWSTR FontLocale)
{

	IDWriteTextFormat *pFormat = nullptr;
	HRESULT hr = pFactory->CreateTextFormat(FontName, nullptr, FontWeight, FontStyle,
		FontStretch, FontSize, FontLocale, &pFormat);

	assert(hr == S_OK);
	return pFormat;
}
