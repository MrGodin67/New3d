#include "PercompIncludes.h"
#include "TextureFactory.h"
#include "StringHandler.h"
#include "DirectXTex.h"
using namespace DirectX;
TextureFactory::TextureFactory()
{
}


TextureFactory::~TextureFactory()
{
}
HRESULT TextureFactory::CreateSRVFromFile(SRV_Package & pkg, std::string filename)
{
	if (!DXStringHandler::DXDoesFileExist(filename))
		return E_FAIL;

	HRESULT result;
	wstring fName;
	DXStringHandler::DXConvertFromStrToWStr(filename, fName);
	TexMetadata imageMetadata;
	
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ScratchImage* pScratchImage = new ScratchImage();
	ID3D11Texture2D* m_Tex = nullptr;
	result = LoadFromWICFile(const_cast<WCHAR*>(fName.c_str()), WIC_FLAGS_NONE, &imageMetadata, *pScratchImage);
	if (FAILED(result))
	{
		goto Done;
	}


	pkg.textureDesc.Height = (UINT)imageMetadata.width;
	pkg.textureDesc.Width = (UINT)imageMetadata.height;
	pkg.textureDesc.MipLevels = 0;
	pkg.textureDesc.ArraySize = 1;
	pkg.textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	pkg.textureDesc.SampleDesc.Count = 1;
	pkg.textureDesc.SampleDesc.Quality = 0;
	pkg.textureDesc.Usage = D3D11_USAGE_DEFAULT;
	pkg.textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	pkg.textureDesc.CPUAccessFlags = 0;
	pkg.textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;


	int width = (int)imageMetadata.width;

	result = Locator::GetDevice()->CreateTexture2D(&pkg.textureDesc, NULL, &m_Tex);
	if (FAILED(result))
	{
		goto Done;
	}

	rowPitch = (UINT)((width * 4) * sizeof(unsigned char));
	Locator::GetContext()->UpdateSubresource(m_Tex, 0, NULL, pScratchImage->GetPixels(), rowPitch, 0);

	srvDesc.Format = pkg.textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 4;


	result = Locator::GetDevice()->CreateShaderResourceView(m_Tex, &srvDesc, &pkg.texture);
	if (FAILED(result))
	{
		goto Done;
	}
	Locator::GetContext()->GenerateMips(pkg.texture);
Done:
	m_Tex->Release();
	pScratchImage->Release();

	return result;

}
HRESULT TextureFactory::CreateSRVFromFile(ID3D11ShaderResourceView** outTexture, std::string filename)
{
	if (!DXStringHandler::DXDoesFileExist(filename))
		return E_FAIL;

	HRESULT result;
	wstring fName;
	DXStringHandler::DXConvertFromStrToWStr(filename, fName);
	TexMetadata imageMetadata;
	D3D11_TEXTURE2D_DESC textureDesc;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ScratchImage* pScratchImage = new ScratchImage();
	ID3D11Texture2D* m_Tex = nullptr;
	result = LoadFromWICFile(const_cast<WCHAR*>(fName.c_str()), WIC_FLAGS_NONE, &imageMetadata, *pScratchImage);
	if (FAILED(result))
	{
		goto Done;
	}


	textureDesc.Height = (UINT)imageMetadata.width;
	textureDesc.Width = (UINT)imageMetadata.height;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;


	int width = (int)imageMetadata.width;

	result = Locator::GetDevice()->CreateTexture2D(&textureDesc, NULL, &m_Tex);
	if (FAILED(result))
	{
		goto Done;
	}

	rowPitch = (UINT)((width * 4) * sizeof(unsigned char));
    Locator::GetContext()->UpdateSubresource(m_Tex, 0, NULL, pScratchImage->GetPixels(), rowPitch, 0);

	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 4;


	result = Locator::GetDevice()->CreateShaderResourceView(m_Tex, &srvDesc, outTexture);
	if (FAILED(result))
	{
		goto Done;
	}
   Locator::GetContext()->GenerateMips(*outTexture);
Done:
	m_Tex->Release();
	pScratchImage->Release();

	return result;

}
HRESULT TextureFactory::CreateSRVFromFileDDS(ID3D11ShaderResourceView** outTexture, std::string filename)
{
	HRESULT result;

	wstring fName;
	DXStringHandler::DXConvertFromStrToWStr(filename, fName);
	// Load the texture in.
	TexMetadata imageMetadata;

	ScratchImage* pScratchImage = new ScratchImage();
	result = LoadFromDDSFile(
		(WCHAR*)fName.data(),
		DDS_FLAGS_NONE,
		&imageMetadata,
		*pScratchImage);
	if (FAILED(result))
	{
		return E_FAIL;
	}

	result = CreateShaderResourceView(
		Locator::GetDevice(),
		pScratchImage->GetImages(),
		pScratchImage->GetImageCount(),
		imageMetadata,
		outTexture);

	pScratchImage->Release();
	if (FAILED(result)) { return E_FAIL; }

	return S_OK;

}
HRESULT TextureFactory::CreateSRVFromFileTGA(ID3D11ShaderResourceView** outTexture, std::string filename)
{
	
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ID3D11Texture2D* targaTex;
	unsigned char* m_targaData = nullptr;
	// Load the targa image data into memory.
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;

	TargaHeader targaFileHeader;
	unsigned char* targaImage;


	// Open the targa file for reading in binary.
	error = fopen_s(&filePtr, filename.c_str(), "rb");
	if (error != 0)
	{
		return E_FAIL;
	}

	// Read in the file header.
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return E_FAIL;
	}

	// Get the important information from the header.
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if (bpp != 32)
	{
		return E_FAIL;
	}

	// Calculate the size of the 32 bit image data.
	imageSize = width * height * 4;

	// Allocate memory for the targa image data.
	targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return E_FAIL;
	}

	// Read in the targa image data.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return E_FAIL;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return E_FAIL;
	}

	// Allocate memory for the targa destination data.
	m_targaData = new unsigned char[imageSize];
	if (!m_targaData)
	{
		return E_FAIL;
	}

	// Initialize the index into the targa destination data array.
	index = 0;

	// Initialize the index into the targa image data.
	k = (width * height * 4) - (width * 4);

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
	for (j = 0; j<height; j++)
	{
		for (i = 0; i<width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2];  // Red.
			m_targaData[index + 1] = targaImage[k + 1];  // Green.
			m_targaData[index + 2] = targaImage[k + 0];  // Blue
			m_targaData[index + 3] = targaImage[k + 3];  // Alpha

														 // Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (width * 8);
	}

	// Release the targa image data now that it was copied into the destination array.
	delete [] targaImage;


	// Setup the description of the texture.
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Create the empty texture.
	hResult = Locator::GetDevice()->CreateTexture2D(&textureDesc, NULL, &targaTex);
	if (FAILED(hResult))
	{
		return E_FAIL;
	}

	// Set the row pitch of the targa image data.
	rowPitch = (width * 4) * sizeof(unsigned char);

	// Copy the targa image data into the texture.
	Locator::GetContext()->UpdateSubresource(targaTex, 0, NULL, m_targaData, rowPitch, 0);

	// Setup the shader resource view description.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// Create the shader resource view for the texture.
	hResult = Locator::GetDevice()->CreateShaderResourceView(targaTex, &srvDesc, outTexture);
	if (FAILED(hResult))
	{
		return E_FAIL;
	}

	// Generate mipmaps for this texture.
	Locator::GetContext()->GenerateMips(*outTexture);

	// Release the targa image data now that the image data has been loaded into the texture.
	delete [] m_targaData;
	targaTex->Release();
	return S_OK;
}
HRESULT TextureFactory::CreateSRV(SRV_Package & pkg, std::string filename)
{
	HRESULT hr = S_OK;
	if (DXStringHandler::DXDoesFileExist(filename))
	{
		std::string ext;
		DXStringHandler::DXExtractFileExt(filename, ext);
		if (ext == ".tga")
		{

			if (FAILED(hr = CreateSRVFromFileTGA(&pkg.texture, filename)))
			{
				return E_FAIL;
			}
		}
		else if (ext == ".dds")
		{
			if (FAILED(hr = CreateSRVFromFileDDS(&pkg.texture, filename)))
			{
				return E_FAIL;
			}
		}
		else if (ext == ".bmp" || ext == ".jpg" || ext == ".png")
		{
			if (FAILED(hr = CreateSRVFromFile(pkg, filename)))
			{
				return E_FAIL;
			}
		}
		else { return E_FAIL; }// invalid file
	}
	else { return E_FAIL; };// file exists
	return hr;
}
HRESULT TextureFactory::CreateSRV(ID3D11ShaderResourceView ** outTexture, std::string filename)
{
	HRESULT hr = S_OK;
	if (DXStringHandler::DXDoesFileExist(filename))
	{
		std::string ext;
		DXStringHandler::DXExtractFileExt(filename, ext);
		if (ext == ".tga")
		{

			if (FAILED(hr = CreateSRVFromFileTGA(outTexture, filename)))
			{
				return E_FAIL;
			}
		}
		else if (ext == ".dds")
		{
			if (FAILED(hr = CreateSRVFromFileDDS(outTexture, filename)))
			{
				return E_FAIL;
			}
		}
		else if (ext == ".bmp" || ext == ".jpg" || ext == ".png")
		{
			if (FAILED(hr = CreateSRVFromFile(outTexture, filename)))
			{
				return E_FAIL;
			}
		}
		else { return E_FAIL; }// invalid file
	}
	else { return E_FAIL; };// file exists
	return hr;
}

ID2D1Bitmap * TextureFactory::D2D1BitmapTexture::CreateBitmap( std::wstring filename)
{
	HRESULT hr;
	D2D1Texture tex;
	hr = tex.Load(Locator::GetD2DRenderTarget(), filename);
	assert(hr == S_OK);
	return tex.GetBitmap();
}

TextureFactory::D2D1ImageManager::~D2D1ImageManager()
{
	for (auto& it : mapped_Images)
		it.second.image->Release();
}

void TextureFactory::D2D1ImageManager::AddImage(std::wstring filename, std::string name, const D2D1_SIZE_U clipSize)
{
	mapped_Images[name] = D2DImageStruct(D2D1BitmapTexture::CreateBitmap(filename), clipSize.width, clipSize.height);
}

TextureFactory::D2D1ImageManager::D2DImageStruct * TextureFactory::D2D1ImageManager::GetImage( std::string name)
{
	return &mapped_Images[name];
}


