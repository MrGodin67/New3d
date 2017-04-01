#pragma once
#include <d3d11.h>
#include <d2d1.h>
#include <DirectXMath.h>
class Locator
{
	static ID3D11Device* mpDevice;
	static ID3D11DeviceContext* mpContext ;
	static ID2D1RenderTarget* mpD2DRenderTarget;
	static DirectX::XMMATRIX matProjection;
	static DirectX::XMMATRIX matView;
public:
	static void SetProjectionMatrix(DirectX::XMMATRIX proj) { matProjection = proj; }
	static void SetViewMatrix(DirectX::XMMATRIX view) { matView = view; }
	static void SetDevices(ID2D1RenderTarget* d2dRT,ID3D11Device* dev, ID3D11DeviceContext* context);
	static ID3D11Device* GetDevice();
	static ID3D11DeviceContext* GetContext();
	static ID2D1RenderTarget* GetD2DRenderTarget();
	static DirectX::XMMATRIX GetViewMatrix() { return matView; }
	static DirectX::XMMATRIX GetProjectionMatrix() { return matProjection; }
	
};