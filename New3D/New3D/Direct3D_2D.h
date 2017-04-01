
#pragma once
#include "Exceptions.h"

#define Direct3D_EXCEPTION( hr,note ) Direct3D::Exception( hr,note,_CRT_WIDE(__FILE__),__LINE__ )
class Direct3D
{
public:
	class Exception : public DXException
	{
	public:
		Exception(HRESULT hr, const std::wstring& note, const wchar_t* file, unsigned int line);
		std::wstring GetErrorName() const;
		std::wstring GetErrorDescription() const;
		virtual std::wstring GetFullMessage() const override;
		virtual std::wstring GetExceptionType() const override;
	private:
		HRESULT hr;
	};
private:
	HRESULT InvokeScreenAdapter();
	HRESULT InvokeDevices();
	void InvokeViewportMatrices();
	HRESULT InvokeDepthStencilAndBlendStates();


	HWND& m_windowHandle;
	UINT m_numerator = 0;
	UINT m_denominator = 0;
	bool m_fullscreen = false;
	D3D11_VIEWPORT m_Viewport;

protected:
	

	int m_videoCardMemory;
	char m_videoCardDescription[128];
	bool m_vsync_enabled;
	UINT m_screenWidth;
	UINT m_screenHeight;
	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;
	
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pImmediateContext;
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterStateNoCulling;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterStateWireframe;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthDisabledStencilState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_alphaEnableBlendingState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_alphaDisableBlendingState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_alphaEnableBlendingState2;

protected:
	//////////////////////////////////
	// Direct 2D sharing swap chain buffer with Direct 3D 
	///////////////////////////////////
	Microsoft::WRL::ComPtr<ID2D1Factory> m_pD2DFactory;
	ID2D1SolidColorBrush* m_pD2DWhiteBrush = nullptr;
	ID2D1RenderTarget* m_pD2DRenderTarget = nullptr;
	HRESULT InvokeD2DDeviceResources();
	void ReleaseD2DDeviceResources();
private:
	HRESULT InvokeD2DDeviceIndependentResources();

public:
	Direct3D(UINT scrnWidth, UINT scrnHeight, HWND& hwnd, bool vsync, bool fullscreen);
	virtual ~Direct3D();
	
	ID3D11Device* GetDevice() { return m_pDevice.Get(); }
	ID3D11DeviceContext* GetContext() { return m_pImmediateContext.Get(); }
	IDXGISwapChain* GetSwapchain() { return m_pSwapChain.Get(); }
	ID2D1RenderTarget* GetRenderTarget2D() { return m_pD2DRenderTarget; }
	D3D11_VIEWPORT GetViewport();
	HWND GetHandle() {
		return m_windowHandle;
	}
	DirectX::XMMATRIX m_projectionMatrix;

	void GetVideoCardInfo(char* cardName, int& memory);

	void EnableZBuffer();
	void DisableZBuffer();

	void EnableCulling();
	void DisableCulling();

	void EnableAlphaBlending();
	void EnableAlphaToCoverageBlending();
	void DisableAlphaBlending();

	void EnableWireframe();
	void DisableWireframe();

};
