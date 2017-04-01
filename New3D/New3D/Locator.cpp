#include "PercompIncludes.h"
#include "Locator.h"

using namespace DirectX;
ID3D11Device* Locator::mpDevice = nullptr;
ID3D11DeviceContext* Locator::mpContext = nullptr;
ID2D1RenderTarget* Locator::mpD2DRenderTarget = nullptr;
XMMATRIX Locator::matProjection = XMMatrixIdentity();
XMMATRIX Locator::matView = XMMatrixIdentity();


void Locator::SetDevices(ID2D1RenderTarget* d2dRT, ID3D11Device * dev, ID3D11DeviceContext * context)
{
	assert(d2dRT);
	assert(dev);
	assert(context);
	mpD2DRenderTarget = d2dRT;
	mpDevice = dev;
	mpContext = context;
}

ID3D11Device * Locator::GetDevice()
{
	return mpDevice;
}

ID3D11DeviceContext * Locator::GetContext()
{
	return mpContext;
}

ID2D1RenderTarget * Locator::GetD2DRenderTarget()
{
	return mpD2DRenderTarget;
}
