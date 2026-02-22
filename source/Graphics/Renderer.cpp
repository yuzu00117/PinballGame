#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#include "main.h"
#include "renderer.h"
#include <io.h>


D3D_FEATURE_LEVEL       Renderer::m_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

ID3D11Device*           Renderer::m_Device{};
ID3D11DeviceContext*    Renderer::m_DeviceContext{};
IDXGISwapChain*         Renderer::m_SwapChain{};
ID3D11RenderTargetView* Renderer::m_RenderTargetView{};
ID3D11DepthStencilView* Renderer::m_DepthStencilView{};

ID3D11Buffer*			Renderer::m_WorldBuffer{};
ID3D11Buffer*			Renderer::m_ViewBuffer{};
ID3D11Buffer*			Renderer::m_ProjectionBuffer{};
ID3D11Buffer*			Renderer::m_MaterialBuffer{};
ID3D11Buffer*			Renderer::m_LightBuffer{};


ID3D11DepthStencilState* Renderer::m_DepthStateEnable{};
ID3D11DepthStencilState* Renderer::m_DepthStateDisable{};


ID3D11BlendState*		Renderer::m_BlendState{};
ID3D11BlendState*		Renderer::m_BlendStateATC{};

ID2D1Factory* Renderer::m_D2DFactory = nullptr;
ID2D1RenderTarget* Renderer::m_D2DRT = nullptr;
IDWriteFactory* Renderer::m_DWriteFactory = nullptr;
IDWriteTextFormat* Renderer::m_TextFormat = nullptr;
ID2D1SolidColorBrush* Renderer::m_Brush = nullptr;

ID3D11Texture2D*          Renderer::m_MainHDRTex = nullptr;
ID3D11RenderTargetView*   Renderer::m_MainHDRRTV = nullptr;
ID3D11ShaderResourceView* Renderer::m_MainHDRSRV = nullptr;

ID3D11Texture2D*          Renderer::m_LuminanceTex = nullptr;
ID3D11RenderTargetView*   Renderer::m_LuminanceRTV = nullptr;
ID3D11ShaderResourceView* Renderer::m_LuminanceSRV = nullptr;

ID3D11Texture2D*          Renderer::m_BlurTex = nullptr;
ID3D11RenderTargetView*   Renderer::m_BlurRTV = nullptr;
ID3D11ShaderResourceView* Renderer::m_BlurSRV = nullptr;

ID3D11VertexShader* Renderer::m_PostVS = nullptr;
ID3D11PixelShader*  Renderer::m_PostLuminancePS = nullptr;
ID3D11PixelShader*  Renderer::m_PostBlurPS = nullptr;
ID3D11PixelShader*  Renderer::m_PostCompositePS = nullptr;
ID3D11Buffer*       Renderer::m_BlurCBuffer = nullptr;
std::vector<Renderer::TextDesc> Renderer::m_TextList;
static ID3D11Buffer* 		s_DebugVB 		= nullptr;
static UINT 				s_DebugVBBytes 	= 0;
static ID3D11VertexShader*	s_DebugLineVS	= nullptr;
static ID3D11PixelShader*	s_DebugLinePS	= nullptr;
static ID3D11InputLayout*	s_DebugLineIL	= nullptr;

XMFLOAT4X4 Renderer::m_CurrentWorld = {
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1
};

struct CBWorld
{
	DirectX::XMFLOAT4X4 World;
	DirectX::XMFLOAT4X4 WorldInvTranspose;
};

void Renderer::Init()
{
	HRESULT hr = S_OK;

	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = SCREEN_WIDTH;
	swapChainDesc.BufferDesc.Height = SCREEN_HEIGHT;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = GetWindow();
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;

	UINT createFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	hr = D3D11CreateDeviceAndSwapChain( NULL,
										D3D_DRIVER_TYPE_HARDWARE,
										NULL,
										createFlags,
										NULL,
										0,
										D3D11_SDK_VERSION,
										&swapChainDesc,
										&m_SwapChain,
										&m_Device,
										&m_FeatureLevel,
										&m_DeviceContext );

	ID3D11Texture2D* renderTarget{};
	m_SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&renderTarget );
	m_Device->CreateRenderTargetView( renderTarget, NULL, &m_RenderTargetView );
	renderTarget->Release();

	ID3D11Texture2D* depthStencile{};
	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = swapChainDesc.BufferDesc.Width;
	textureDesc.Height = swapChainDesc.BufferDesc.Height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_D16_UNORM;
	textureDesc.SampleDesc = swapChainDesc.SampleDesc;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	m_Device->CreateTexture2D(&textureDesc, NULL, &depthStencile);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = textureDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags = 0;
	m_Device->CreateDepthStencilView(depthStencile, &depthStencilViewDesc, &m_DepthStencilView);
	depthStencile->Release();


	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

	D3D11_VIEWPORT viewport;
	viewport.Width = (FLOAT)SCREEN_WIDTH;
	viewport.Height = (FLOAT)SCREEN_HEIGHT;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	m_DeviceContext->RSSetViewports( 1, &viewport );

	D3D11_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID; 
	rasterizerDesc.CullMode = D3D11_CULL_BACK; 
	rasterizerDesc.DepthClipEnable = TRUE; 
	rasterizerDesc.MultisampleEnable = FALSE; 

	ID3D11RasterizerState *rs;
	m_Device->CreateRasterizerState( &rasterizerDesc, &rs );

	m_DeviceContext->RSSetState( rs );

	D3D11_BLEND_DESC blendDesc{};
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_Device->CreateBlendState( &blendDesc, &m_BlendState );

	blendDesc.AlphaToCoverageEnable = TRUE;
	m_Device->CreateBlendState( &blendDesc, &m_BlendStateATC );

	float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	m_DeviceContext->OMSetBlendState(m_BlendState, blendFactor, 0xffffffff );

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = FALSE;

	m_Device->CreateDepthStencilState( &depthStencilDesc, &m_DepthStateEnable );

	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ZERO;
	m_Device->CreateDepthStencilState( &depthStencilDesc, &m_DepthStateDisable );

	m_DeviceContext->OMSetDepthStencilState( m_DepthStateEnable, NULL );

	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 4;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* samplerState{};
	m_Device->CreateSamplerState( &samplerDesc, &samplerState );

	m_DeviceContext->PSSetSamplers( 0, 1, &samplerState );

	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = sizeof(float);

	// World
	bufferDesc.ByteWidth = sizeof(CBWorld);
	m_Device->CreateBuffer( &bufferDesc, NULL, &m_WorldBuffer );
	m_DeviceContext->VSSetConstantBuffers( 0, 1, &m_WorldBuffer);

	// View
	bufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
	m_Device->CreateBuffer( &bufferDesc, NULL, &m_ViewBuffer );
	m_DeviceContext->VSSetConstantBuffers( 1, 1, &m_ViewBuffer );

	// Projection
	bufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
	m_Device->CreateBuffer( &bufferDesc, NULL, &m_ProjectionBuffer );
	m_DeviceContext->VSSetConstantBuffers( 2, 1, &m_ProjectionBuffer );


	bufferDesc.ByteWidth = sizeof(MATERIAL);

	m_Device->CreateBuffer( &bufferDesc, NULL, &m_MaterialBuffer );
	m_DeviceContext->VSSetConstantBuffers( 3, 1, &m_MaterialBuffer );
	m_DeviceContext->PSSetConstantBuffers( 3, 1, &m_MaterialBuffer );


	bufferDesc.ByteWidth = sizeof(LIGHT);

	m_Device->CreateBuffer( &bufferDesc, NULL, &m_LightBuffer );
	m_DeviceContext->VSSetConstantBuffers( 4, 1, &m_LightBuffer );
	m_DeviceContext->PSSetConstantBuffers( 4, 1, &m_LightBuffer );





	LIGHT light{};
	light.Enable = true;
	light.Direction = XMFLOAT4(0.3f, -1.0f, 0.3f, 0.0f);
	light.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	light.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetLight(light);



	MATERIAL material{};
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);





	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_D2DFactory);
	assert(SUCCEEDED(hr));

	IDXGISurface* backBuffer = nullptr;
	hr = m_SwapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(&backBuffer));
	assert(SUCCEEDED(hr));

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)
		);
	hr = m_D2DFactory->CreateDxgiSurfaceRenderTarget(backBuffer, &props, &m_D2DRT);
	backBuffer->Release();
	assert(SUCCEEDED(hr));

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&m_DWriteFactory));
	assert(SUCCEEDED(hr));

	hr = m_DWriteFactory->CreateTextFormat(
		L"Segoe UI",
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		24.0f,
		L"",
		&m_TextFormat
	);
	assert(SUCCEEDED(hr));

	hr = m_D2DRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_Brush);
	assert(SUCCEEDED(hr));

	// ブルーム用RenderTarget生成 (HDR)
	D3D11_TEXTURE2D_DESC hdrTexDesc{};
	hdrTexDesc.Width = SCREEN_WIDTH;
	hdrTexDesc.Height = SCREEN_HEIGHT;
	hdrTexDesc.MipLevels = 1;
	hdrTexDesc.ArraySize = 1;
	hdrTexDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	hdrTexDesc.SampleDesc.Count = 1;
	hdrTexDesc.SampleDesc.Quality = 0;
	hdrTexDesc.Usage = D3D11_USAGE_DEFAULT;
	hdrTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	m_Device->CreateTexture2D(&hdrTexDesc, NULL, &m_MainHDRTex);
	m_Device->CreateRenderTargetView(m_MainHDRTex, NULL, &m_MainHDRRTV);
	m_Device->CreateShaderResourceView(m_MainHDRTex, NULL, &m_MainHDRSRV);

	// 抽出・ぼかし用RenderTarget生成 (1/4縮小)
	hdrTexDesc.Width = SCREEN_WIDTH / 4;
	hdrTexDesc.Height = SCREEN_HEIGHT / 4;
	m_Device->CreateTexture2D(&hdrTexDesc, NULL, &m_LuminanceTex);
	m_Device->CreateRenderTargetView(m_LuminanceTex, NULL, &m_LuminanceRTV);
	m_Device->CreateShaderResourceView(m_LuminanceTex, NULL, &m_LuminanceSRV);

	m_Device->CreateTexture2D(&hdrTexDesc, NULL, &m_BlurTex);
	m_Device->CreateRenderTargetView(m_BlurTex, NULL, &m_BlurRTV);
	m_Device->CreateShaderResourceView(m_BlurTex, NULL, &m_BlurSRV);

	// ポストプロセス用シェーダー読み込み
	FILE* fp;
	long int fsize;
	unsigned char* buffer;

	// VS
	fp = fopen("shader\\bin\\PostVS.cso", "rb");
	if (fp) {
		fseek(fp, 0, SEEK_END); fsize = ftell(fp); fseek(fp, 0, SEEK_SET);
		buffer = new unsigned char[fsize]; fread(buffer, fsize, 1, fp); fclose(fp);
		m_Device->CreateVertexShader(buffer, fsize, NULL, &m_PostVS);
		delete[] buffer;
	}
	
	// PS
	fp = fopen("shader\\bin\\PostLuminancePS.cso", "rb");
	if (fp) {
		fseek(fp, 0, SEEK_END); fsize = ftell(fp); fseek(fp, 0, SEEK_SET);
		buffer = new unsigned char[fsize]; fread(buffer, fsize, 1, fp); fclose(fp);
		m_Device->CreatePixelShader(buffer, fsize, NULL, &m_PostLuminancePS);
		delete[] buffer;
	}
	fp = fopen("shader\\bin\\PostBlurPS.cso", "rb");
	if (fp) {
		fseek(fp, 0, SEEK_END); fsize = ftell(fp); fseek(fp, 0, SEEK_SET);
		buffer = new unsigned char[fsize]; fread(buffer, fsize, 1, fp); fclose(fp);
		m_Device->CreatePixelShader(buffer, fsize, NULL, &m_PostBlurPS);
		delete[] buffer;
	}
	fp = fopen("shader\\bin\\PostCompositePS.cso", "rb");
	if (fp) {
		fseek(fp, 0, SEEK_END); fsize = ftell(fp); fseek(fp, 0, SEEK_SET);
		buffer = new unsigned char[fsize]; fread(buffer, fsize, 1, fp); fclose(fp);
		m_Device->CreatePixelShader(buffer, fsize, NULL, &m_PostCompositePS);
		delete[] buffer;
	}

	D3D11_BUFFER_DESC bcDesc{};
	bcDesc.Usage = D3D11_USAGE_DEFAULT;
	bcDesc.ByteWidth = sizeof(BlurCB);
	bcDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	m_Device->CreateBuffer(&bcDesc, NULL, &m_BlurCBuffer);

}

void Renderer::Uninit()
{

	m_WorldBuffer->Release();
	m_ViewBuffer->Release();
	m_ProjectionBuffer->Release();
	m_LightBuffer->Release();
	m_MaterialBuffer->Release();


	m_DeviceContext->ClearState();
	m_RenderTargetView->Release();
	m_SwapChain->Release();
	m_DeviceContext->Release();
	m_Device->Release();

    if (m_Brush)         { m_Brush->Release();         m_Brush = nullptr; }
    if (m_TextFormat)    { m_TextFormat->Release();    m_TextFormat = nullptr; }
    if (m_DWriteFactory) { m_DWriteFactory->Release(); m_DWriteFactory = nullptr; }
    if (m_D2DRT)         { m_D2DRT->Release();         m_D2DRT = nullptr; }
    if (m_D2DFactory)    { m_D2DFactory->Release();    m_D2DFactory = nullptr; }
    if (m_BlurCBuffer)     { m_BlurCBuffer->Release(); m_BlurCBuffer = nullptr; }
    if (m_PostCompositePS) { m_PostCompositePS->Release(); m_PostCompositePS = nullptr; }
    if (m_PostBlurPS)      { m_PostBlurPS->Release(); m_PostBlurPS = nullptr; }
    if (m_PostLuminancePS) { m_PostLuminancePS->Release(); m_PostLuminancePS = nullptr; }
    if (m_PostVS)          { m_PostVS->Release(); m_PostVS = nullptr; }
    if (m_BlurSRV)         { m_BlurSRV->Release(); m_BlurSRV = nullptr; }
    if (m_BlurRTV)         { m_BlurRTV->Release(); m_BlurRTV = nullptr; }
    if (m_BlurTex)         { m_BlurTex->Release(); m_BlurTex = nullptr; }
    if (m_LuminanceSRV)    { m_LuminanceSRV->Release(); m_LuminanceSRV = nullptr; }
    if (m_LuminanceRTV)    { m_LuminanceRTV->Release(); m_LuminanceRTV = nullptr; }
    if (m_LuminanceTex)    { m_LuminanceTex->Release(); m_LuminanceTex = nullptr; }
    if (m_MainHDRSRV)      { m_MainHDRSRV->Release(); m_MainHDRSRV = nullptr; }
    if (m_MainHDRRTV)      { m_MainHDRRTV->Release(); m_MainHDRRTV = nullptr; }
    if (m_MainHDRTex)      { m_MainHDRTex->Release(); m_MainHDRTex = nullptr; }

}

void Renderer::Begin()
{
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	// メイン描画先をHDRテクスチャにする
	m_DeviceContext->ClearRenderTargetView( m_MainHDRRTV, clearColor );
	m_DeviceContext->ClearDepthStencilView( m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_DeviceContext->OMSetRenderTargets(1, &m_MainHDRRTV, m_DepthStencilView);

	// 前フレームのポストプロセスで無効化したステートを復旧する
	SetDepthEnable(true);
	SetATCEnable(false); // 通常のアルファブレンドにする
}

void Renderer::End()
{
	ID3D11ShaderResourceView* nullSRV[] = { nullptr, nullptr };

	// ポストプロセス描画用にブレンドステートを無効化（不透明上書き）
	m_DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	m_DeviceContext->OMSetDepthStencilState(m_DepthStateDisable, 0);

	// --------------------------------------------------------
	// 1. 輝度抽出パス (HDR -> LuminanceTex 1/4)
	// --------------------------------------------------------
	m_DeviceContext->OMSetRenderTargets(1, &m_LuminanceRTV, nullptr);
	D3D11_VIEWPORT vpQ = { 0.0f, 0.0f, (float)SCREEN_WIDTH / 4, (float)SCREEN_HEIGHT / 4, 0.0f, 1.0f };
	m_DeviceContext->RSSetViewports(1, &vpQ);

	m_DeviceContext->IASetInputLayout(nullptr);
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_DeviceContext->VSSetShader(m_PostVS, nullptr, 0);
	m_DeviceContext->PSSetShader(m_PostLuminancePS, nullptr, 0);

	m_DeviceContext->PSSetShaderResources(0, 1, &m_MainHDRSRV);
	m_DeviceContext->Draw(3, 0);
	m_DeviceContext->PSSetShaderResources(0, 2, nullSRV); // 外す

	// --------------------------------------------------------
	// 2. ぼかしパス1 (LuminanceTex -> BlurTex) 横方向
	// --------------------------------------------------------
	m_DeviceContext->OMSetRenderTargets(1, &m_BlurRTV, nullptr);
	m_DeviceContext->PSSetShader(m_PostBlurPS, nullptr, 0);

	BlurCB cb{};
	cb.TexelSize = { 1.0f / (SCREEN_WIDTH / 4.0f), 1.0f / (SCREEN_HEIGHT / 4.0f) };
	cb.Dir = { 1.0f, 0.0f };
	m_DeviceContext->UpdateSubresource(m_BlurCBuffer, 0, nullptr, &cb, 0, 0);
	m_DeviceContext->PSSetConstantBuffers(0, 1, &m_BlurCBuffer);

	m_DeviceContext->PSSetShaderResources(0, 1, &m_LuminanceSRV);
	m_DeviceContext->Draw(3, 0);
	m_DeviceContext->PSSetShaderResources(0, 2, nullSRV);

	// --------------------------------------------------------
	// 3. ぼかしパス2 (BlurTex -> LuminanceTex) 縦方向
	// --------------------------------------------------------
	m_DeviceContext->OMSetRenderTargets(1, &m_LuminanceRTV, nullptr);
	cb.Dir = { 0.0f, 1.0f };
	m_DeviceContext->UpdateSubresource(m_BlurCBuffer, 0, nullptr, &cb, 0, 0);

	m_DeviceContext->PSSetShaderResources(0, 1, &m_BlurSRV);
	m_DeviceContext->Draw(3, 0);
	m_DeviceContext->PSSetShaderResources(0, 2, nullSRV);

	// --------------------------------------------------------
	// 4. 合成・トーンマッピングパス (Main + LuminanceTex -> BackBuffer)
	// --------------------------------------------------------
	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, nullptr);
	D3D11_VIEWPORT vpF = { 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f, 1.0f };
	m_DeviceContext->RSSetViewports(1, &vpF);

	m_DeviceContext->PSSetShader(m_PostCompositePS, nullptr, 0);
	
	ID3D11ShaderResourceView* srvs[] = { m_MainHDRSRV, m_LuminanceSRV };
	m_DeviceContext->PSSetShaderResources(0, 2, srvs);
	m_DeviceContext->Draw(3, 0);
	m_DeviceContext->PSSetShaderResources(0, 2, nullSRV);

	// --------------------------------------------------------
	// 終了処理
	// --------------------------------------------------------
	// 次のフレームに備えてRTVをメインHDAに戻しておく処理（Beginでやるのでなくても良いが念のため）
	m_DeviceContext->OMSetRenderTargets(1, &m_MainHDRRTV, m_DepthStencilView);

	// D2D テキストの遅延描画
	if (!m_TextList.empty())
	{
		m_D2DRT->BeginDraw();
		for (const auto& t : m_TextList)
		{
			D2D1_RECT_F layout = D2D1::RectF(t.x, t.y, t.x + 800.0f, t.y + 200.0f);
			m_D2DRT->DrawText(
				t.text.c_str(),
				static_cast<UINT32>(t.text.length()),
				m_TextFormat,
				layout,
				m_Brush
			);
		}
		m_D2DRT->EndDraw();
		m_TextList.clear();
	}

	m_SwapChain->Present( 1, 0 );
}

void Renderer::SetDepthEnable( bool Enable )
{
	if( Enable )
		m_DeviceContext->OMSetDepthStencilState( m_DepthStateEnable, NULL );
	else
		m_DeviceContext->OMSetDepthStencilState( m_DepthStateDisable, NULL );

}

void Renderer::SetATCEnable( bool Enable )
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	if (Enable)
		m_DeviceContext->OMSetBlendState(m_BlendStateATC, blendFactor, 0xffffffff);
	else
		m_DeviceContext->OMSetBlendState(m_BlendState, blendFactor, 0xffffffff);

}

void Renderer::SetWorldViewProjection2D()
{
	SetWorldMatrix(XMMatrixIdentity());
	SetViewMatrix(XMMatrixIdentity());

	XMMATRIX projection;
	projection = XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	SetProjectionMatrix(projection);

	// 定数バッファのバインドが他の描画処理で外れている場合に備えて強制再バインド
	m_DeviceContext->VSSetConstantBuffers(0, 1, &m_WorldBuffer);
	m_DeviceContext->VSSetConstantBuffers(1, 1, &m_ViewBuffer);
	m_DeviceContext->VSSetConstantBuffers(2, 1, &m_ProjectionBuffer);
}

void Renderer::SetWorldMatrix(XMMATRIX WorldMatrix)
{
	XMStoreFloat4x4(&m_CurrentWorld, WorldMatrix);

	CBWorld cb{};

	XMStoreFloat4x4(&cb.World, XMMatrixTranspose(WorldMatrix));

	XMMATRIX invWorld = XMMatrixInverse(nullptr, WorldMatrix);
	XMStoreFloat4x4(&cb.WorldInvTranspose, XMMatrixTranspose(invWorld));

	m_DeviceContext->UpdateSubresource(m_WorldBuffer, 0, NULL, &cb, 0, 0);
}

void Renderer::SetViewMatrix(XMMATRIX ViewMatrix)
{
	XMFLOAT4X4 viewf;
	XMStoreFloat4x4(&viewf, XMMatrixTranspose(ViewMatrix));
	m_DeviceContext->UpdateSubresource(m_ViewBuffer, 0, NULL, &viewf, 0, 0);
}

void Renderer::SetProjectionMatrix(XMMATRIX ProjectionMatrix)
{
	XMFLOAT4X4 projectionf;
	XMStoreFloat4x4(&projectionf, XMMatrixTranspose(ProjectionMatrix));
	m_DeviceContext->UpdateSubresource(m_ProjectionBuffer, 0, NULL, &projectionf, 0, 0);

}

void Renderer::SetMaterial( MATERIAL Material )
{
	m_DeviceContext->UpdateSubresource( m_MaterialBuffer, 0, NULL, &Material, 0, 0 );

}

void Renderer::SetLight( LIGHT Light )
{
	m_DeviceContext->UpdateSubresource(m_LightBuffer, 0, NULL, &Light, 0, 0);
}

void Renderer::CreateVertexShader( ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName )
{

	FILE* file;
	long int fsize;

	file = fopen(FileName, "rb");
	assert(file);

	fsize = _filelength(_fileno(file));
	unsigned char* buffer = new unsigned char[fsize];
	fread(buffer, fsize, 1, file);
	fclose(file);

	m_Device->CreateVertexShader(buffer, fsize, NULL, VertexShader);


	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 6, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 10, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);

	m_Device->CreateInputLayout(layout,
		numElements,
		buffer,
		fsize,
		VertexLayout);

	delete[] buffer;
}

void Renderer::CreatePixelShader( ID3D11PixelShader** PixelShader, const char* FileName )
{
	FILE* file;
	long int fsize;

	file = fopen(FileName, "rb");
	assert(file);

	fsize = _filelength(_fileno(file));
	unsigned char* buffer = new unsigned char[fsize];
	fread(buffer, fsize, 1, file);
	fclose(file);

	m_Device->CreatePixelShader(buffer, fsize, NULL, PixelShader);

	delete[] buffer;
}

void Renderer::DrawText(const std::wstring& text, float x, float y)
{
	m_TextList.push_back({text, x, y});
}

static void EnsureDebugLinePipeline()
{
	const char* vsPath = "shader\\bin\\DebugLineVS.cso";
	const char* psPath = "shader\\bin\\DebugLinePS.cso";

	FILE* fp = fopen(vsPath, "rb"); assert(fp);
	fseek(fp, 0, SEEK_END); long vsSize = ftell(fp); fseek(fp, 0, SEEK_SET);
	std::vector<unsigned char> vsBlob(vsSize);
	fread(vsBlob.data(), 1, vsSize, fp); fclose(fp);

	Renderer::GetDevice()->CreateVertexShader(vsBlob.data(), vsSize, nullptr, &s_DebugLineVS);

	D3D11_INPUT_ELEMENT_DESC il[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                 D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(float)*3,   D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    Renderer::GetDevice()->CreateInputLayout(il, _countof(il), vsBlob.data(), vsSize, &s_DebugLineIL);

    fp = fopen(psPath, "rb"); assert(fp);
    fseek(fp, 0, SEEK_END); long psSize = ftell(fp); fseek(fp, 0, SEEK_SET);
    std::vector<unsigned char> psBlob(psSize);
    fread(psBlob.data(), 1, psSize, fp); fclose(fp);
    Renderer::GetDevice()->CreatePixelShader(psBlob.data(), psSize, nullptr, &s_DebugLinePS);
}

void Renderer::DrawDebugLines(const DebugLineVertex* vertices, UINT vertexCount)
{
    if (!vertices || vertexCount == 0) return;

    EnsureDebugLinePipeline();

    auto* dev = GetDevice();
    auto* ctx = GetDeviceContext();

    ID3D11InputLayout*     	 prevIL  = nullptr;
    ID3D11VertexShader*    	 prevVS  = nullptr;
    ID3D11PixelShader*     	 prevPS  = nullptr;
    D3D11_PRIMITIVE_TOPOLOGY prevTopo;

    ctx->IAGetInputLayout(&prevIL);
    ctx->VSGetShader(&prevVS, nullptr, nullptr);
    ctx->PSGetShader(&prevPS, nullptr, nullptr);
    ctx->IAGetPrimitiveTopology(&prevTopo);

	XMMATRIX prevWorld = XMLoadFloat4x4(&m_CurrentWorld);

    const UINT bytesNeeded = sizeof(DebugLineVertex) * vertexCount;
    if (!s_DebugVB || s_DebugVBBytes < bytesNeeded)
    {
        if (s_DebugVB) { s_DebugVB->Release(); s_DebugVB = nullptr; }
        s_DebugVBBytes = std::max<UINT>(bytesNeeded, 4096);

        D3D11_BUFFER_DESC bd{};
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = s_DebugVBBytes;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        dev->CreateBuffer(&bd, nullptr, &s_DebugVB);
    }

    D3D11_MAPPED_SUBRESOURCE msr{};
    ctx->Map(s_DebugVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    memcpy(msr.pData, vertices, bytesNeeded);
    ctx->Unmap(s_DebugVB, 0);

    UINT stride = sizeof(DebugLineVertex), offset = 0;
    ctx->IASetInputLayout(s_DebugLineIL);
    ctx->IASetVertexBuffers(0, 1, &s_DebugVB, &stride, &offset);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    ctx->VSSetShader(s_DebugLineVS, nullptr, 0);
    ctx->PSSetShader(s_DebugLinePS, nullptr, 0);

	SetWorldMatrix(XMMatrixIdentity());

    ctx->Draw(vertexCount, 0);

    ctx->IASetInputLayout(prevIL);
    ctx->VSSetShader(prevVS, nullptr, 0);
    ctx->PSSetShader(prevPS, nullptr, 0);
    ctx->IASetPrimitiveTopology(prevTopo);

    if (prevIL) prevIL->Release();
    if (prevVS) prevVS->Release();
    if (prevPS) prevPS->Release();
}
