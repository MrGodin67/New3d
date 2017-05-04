#pragma once
#include "Locator.h"
#include "D2D1Texture.h"
#include "Table.h"
#include <unordered_map>
class TextureFactory
{
public:
	struct SRV_Package
	{
		D3D11_TEXTURE2D_DESC textureDesc;
		ID3D11ShaderResourceView* texture;
	};
private:
	std::unordered_map<std::string, ID3D11ShaderResourceView*> m_Textures;
	
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};
	static HRESULT CreateSRVFromFile(SRV_Package& pkg, std::string filename);
	static HRESULT CreateSRVFromFile(ID3D11ShaderResourceView** outTexture, std::string filename);
	static HRESULT CreateSRVFromFileDDS(ID3D11ShaderResourceView** outTexture, std::string filename);
	static HRESULT CreateSRVFromFileTGA(ID3D11ShaderResourceView** outTexture, std::string filename);

public:
	TextureFactory();
	~TextureFactory();

	static HRESULT CreateSRV(SRV_Package& pkg, std::string filename);
	static HRESULT CreateSRV(ID3D11ShaderResourceView ** outTexture, std::string filename);
	class D2D1BitmapTexture
	{
		
	public:
		D2D1BitmapTexture() {}
		static ID2D1Bitmap* CreateBitmap(std::wstring filename);
	};
	class D2D1ImageManager
	{
	public:
		struct D2DImageStruct
		{
			D2D1_SIZE_F imageSize;
			D2D1_SIZE_F clipSize;
			ID2D1Bitmap* image;
			Table<D2D1_RECT_F> imageRects;
			D2DImageStruct() {}
			D2DImageStruct(ID2D1Bitmap* img,const UINT clipWidth,const UINT clipHeight)
				:image(img)
			{
				clipSize.width = (float)clipWidth;
				clipSize.height = (float)clipHeight;
				imageSize = image->GetSize();
			
				UINT rows = (UINT)imageSize.height / clipHeight;
				UINT cols = (UINT)imageSize.width / clipWidth;
				imageRects.resize(rows,cols);
				float pX = 0.0f;
				float pY = 0.0f;
				for (UINT r = 0; r < rows; r++)
				{
					pX = 0.0f;
					for (UINT c = 0; c < cols; c++)
					{
						imageRects(r, c) = D2D1::RectF(pX, pY, pX + clipWidth, pY + clipHeight);
						pX += clipWidth;
					}
					pY += clipHeight;
				}
			}
			inline D2D1_RECT_F GetClipRect(const UINT row, const UINT col)
			{
				return imageRects(row, col);
			}
			inline D2D1_RECT_F GetClipRect(const UINT index)
			{
				assert(index >= 0 && index < imageRects.numElements());
				return imageRects(index);
			}
		};
	private:
		std::unordered_map<std::string, D2DImageStruct> mapped_Images;
	public:
		D2D1ImageManager() {};
		~D2D1ImageManager();
		void AddImage(std::wstring filename,std::string name, const D2D1_SIZE_U clipSize);
		D2DImageStruct* GetImage(std::string name);
		
	};
};
typedef TextureFactory::D2D1ImageManager::D2DImageStruct D2DImage;

