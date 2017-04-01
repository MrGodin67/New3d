#pragma once
#include <Windows.h>
#include <WindowsX.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <wrl.h>
#include <wincodec.h>
#include <array>
#include <d3d11.h>
#include <DirectXMath.h>
#include <functional>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <assert.h>
#include <typeindex>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex_db.lib")
#else
#pragma comment(lib, "DirectXTex_r.lib")
#endif
template<typename T>
inline void SafeRelease(T& ptr)
{
	if (ptr != NULL)
	{
		ptr->Release();
		ptr = nullptr;
	}
}
template<typename T>
inline void SafeDelete(T& ptr)
{
	if (ptr != nullptr)
	{
		delete ptr;
		ptr = nullptr;
	}
}
template<typename T>
inline void SafeDeleteArray(T& ptr)
{
	if (ptr != NULL)
	{
		delete[] ptr;
		ptr = NULL;
	}
};