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

// 追加：Direct2D／DirectWrite 用メンバ初期化
ID2D1Factory* Renderer::m_D2DFactory = nullptr;
ID2D1RenderTarget* Renderer::m_D2DRT = nullptr;
IDWriteFactory* Renderer::m_DWriteFactory = nullptr;
IDWriteTextFormat* Renderer::m_TextFormat = nullptr;
ID2D1SolidColorBrush* Renderer::m_Brush = nullptr;

// デバッグ描画用静的変数
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


void Renderer::Init()
{
	HRESULT hr = S_OK;




	// デバイス、スワップチェーン作成
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






	// レンダーターゲットビュー作成
	ID3D11Texture2D* renderTarget{};
	m_SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&renderTarget );
	m_Device->CreateRenderTargetView( renderTarget, NULL, &m_RenderTargetView );
	renderTarget->Release();


	// デプスステンシルバッファ作成
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

	// デプスステンシルビュー作成
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = textureDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags = 0;
	m_Device->CreateDepthStencilView(depthStencile, &depthStencilViewDesc, &m_DepthStencilView);
	depthStencile->Release();


	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);





	// ビューポート設定
	D3D11_VIEWPORT viewport;
	viewport.Width = (FLOAT)SCREEN_WIDTH;
	viewport.Height = (FLOAT)SCREEN_HEIGHT;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	m_DeviceContext->RSSetViewports( 1, &viewport );



	// ラスタライザステート設定
	D3D11_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID; 
	rasterizerDesc.CullMode = D3D11_CULL_BACK; 
	rasterizerDesc.DepthClipEnable = TRUE; 
	rasterizerDesc.MultisampleEnable = FALSE; 

	ID3D11RasterizerState *rs;
	m_Device->CreateRasterizerState( &rasterizerDesc, &rs );

	m_DeviceContext->RSSetState( rs );




	// ブレンドステート設定
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





	// デプスステンシルステート設定
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = FALSE;

	m_Device->CreateDepthStencilState( &depthStencilDesc, &m_DepthStateEnable );//深度有効ステート

	//depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ZERO;
	m_Device->CreateDepthStencilState( &depthStencilDesc, &m_DepthStateDisable );//深度無効ステート

	m_DeviceContext->OMSetDepthStencilState( m_DepthStateEnable, NULL );




	// サンプラーステート設定
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



	// 定数バッファ生成
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(XMFLOAT4X4);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = sizeof(float);

	m_Device->CreateBuffer( &bufferDesc, NULL, &m_WorldBuffer );
	m_DeviceContext->VSSetConstantBuffers( 0, 1, &m_WorldBuffer);

	m_Device->CreateBuffer( &bufferDesc, NULL, &m_ViewBuffer );
	m_DeviceContext->VSSetConstantBuffers( 1, 1, &m_ViewBuffer );

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





	// ライト初期化
	LIGHT light{};
	light.Enable = true;
	light.Direction = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	light.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	light.Diffuse = XMFLOAT4(1.5f, 1.5f, 1.5f, 1.0f);
	SetLight(light);



	// マテリアル初期化
	MATERIAL material{};
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);




	// --- ここから追加：Direct2D + DirectWrite 初期化 ---

	// 1) D2D ファクトリ生成
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_D2DFactory);
	assert(SUCCEEDED(hr));

	// 2) DXGI バックバッファから D2D レンダーターゲット作成
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

	// 3) DirectWrite ファクトリ & TextFormat
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&m_DWriteFactory));
	assert(SUCCEEDED(hr));

	hr = m_DWriteFactory->CreateTextFormat(
		L"Segoe UI",        // フォント名
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		24.0f,              // フォントサイズ
		L"",                // ロケール
		&m_TextFormat
	);
	assert(SUCCEEDED(hr));

	// 4) ブラシ生成
	hr = m_D2DRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_Brush);
	assert(SUCCEEDED(hr));


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

    // --- 追加：Direct2D/DirectWrite 解放 ---
    if (m_Brush)         { m_Brush->Release();         m_Brush = nullptr; }
    if (m_TextFormat)    { m_TextFormat->Release();    m_TextFormat = nullptr; }
    if (m_DWriteFactory) { m_DWriteFactory->Release(); m_DWriteFactory = nullptr; }
    if (m_D2DRT)         { m_D2DRT->Release();         m_D2DRT = nullptr; }
    if (m_D2DFactory)    { m_D2DFactory->Release();    m_D2DFactory = nullptr; }
    // --- Direct2D/DirectWrite 解放ここまで ---

}

void Renderer::Begin()
{
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_DeviceContext->ClearRenderTargetView( m_RenderTargetView, clearColor );
	m_DeviceContext->ClearDepthStencilView( m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Renderer::End()
{
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
}

void Renderer::SetWorldMatrix(XMMATRIX WorldMatrix)
{
	// 元の行列をそのまま保持
	XMStoreFloat4x4(&m_CurrentWorld, WorldMatrix);

	// シェーダーに渡す用に転置行列を計算
	XMFLOAT4X4 worldf;
	XMStoreFloat4x4(&worldf, XMMatrixTranspose(WorldMatrix));
	m_DeviceContext->UpdateSubresource(m_WorldBuffer, 0, NULL, &worldf, 0, 0);
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
	// 1) D2D 描画開始
	m_D2DRT->BeginDraw();

	// 2) テキスト描画
	D2D1_RECT_F layout = D2D1::RectF(x, y, x + 800, y + 200);
	m_D2DRT->DrawText(
		text.c_str(),
		static_cast<UINT32>(text.length()),
		m_TextFormat,
		layout,
		m_Brush
	);

	// 3) 描画確定
	m_D2DRT->EndDraw();
}

// デバッグ描画用パイプライン確保
static void EnsureDebugLinePipeline()
{
	const char* vsPath = "shader\\bin\\DebugLineVS.cso";
	const char* psPath = "shader\\bin\\DebugLinePS.cso";

	// バイトコード読み込み
	FILE* fp = fopen(vsPath, "rb"); assert(fp);
	fseek(fp, 0, SEEK_END); long vsSize = ftell(fp); fseek(fp, 0, SEEK_SET);
	std::vector<unsigned char> vsBlob(vsSize);
	fread(vsBlob.data(), 1, vsSize, fp); fclose(fp);

	// VS/IL 作成
	Renderer::GetDevice()->CreateVertexShader(vsBlob.data(), vsSize, nullptr, &s_DebugLineVS);

	D3D11_INPUT_ELEMENT_DESC il[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                 D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(float)*3,   D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    Renderer::GetDevice()->CreateInputLayout(il, _countof(il), vsBlob.data(), vsSize, &s_DebugLineIL);

    // PS作成
    fp = fopen(psPath, "rb"); assert(fp);
    fseek(fp, 0, SEEK_END); long psSize = ftell(fp); fseek(fp, 0, SEEK_SET);
    std::vector<unsigned char> psBlob(psSize);
    fread(psBlob.data(), 1, psSize, fp); fclose(fp);
    Renderer::GetDevice()->CreatePixelShader(psBlob.data(), psSize, nullptr, &s_DebugLinePS);
}

// デバッグライン描画
void Renderer::DrawDebugLines(const DebugLineVertex* vertices, UINT vertexCount)
{
    if (!vertices || vertexCount == 0) return;

    EnsureDebugLinePipeline();

    auto* dev = GetDevice();
    auto* ctx = GetDeviceContext();

    // --- 直前のパイプライン状態を退避 ---
    ID3D11InputLayout*     	 prevIL  = nullptr;
    ID3D11VertexShader*    	 prevVS  = nullptr;
    ID3D11PixelShader*     	 prevPS  = nullptr;
    D3D11_PRIMITIVE_TOPOLOGY prevTopo;

    ctx->IAGetInputLayout(&prevIL);
    ctx->VSGetShader(&prevVS, nullptr, nullptr);
    ctx->PSGetShader(&prevPS, nullptr, nullptr);
    ctx->IAGetPrimitiveTopology(&prevTopo);

    // World行列も退避
	XMMATRIX prevWorld = XMLoadFloat4x4(&m_CurrentWorld);

    // --- 動的VBを確保/拡張 ---
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

    // データ転送
    D3D11_MAPPED_SUBRESOURCE msr{};
    ctx->Map(s_DebugVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    memcpy(msr.pData, vertices, bytesNeeded);
    ctx->Unmap(s_DebugVB, 0);

    // ===== デバッグ用パイプライン設定 =====
    UINT stride = sizeof(DebugLineVertex), offset = 0;
    ctx->IASetInputLayout(s_DebugLineIL);
    ctx->IASetVertexBuffers(0, 1, &s_DebugVB, &stride, &offset);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    ctx->VSSetShader(s_DebugLineVS, nullptr, 0);
    ctx->PSSetShader(s_DebugLinePS, nullptr, 0);

    // デバッグ線は頂点がワールド座標なので、World=Iにする
	SetWorldMatrix(XMMatrixIdentity());

    ctx->Draw(vertexCount, 0);

    // --- パイプライン状態を元に戻す ---
    ctx->IASetInputLayout(prevIL);
    ctx->VSSetShader(prevVS, nullptr, 0);
    ctx->PSSetShader(prevPS, nullptr, 0);
    ctx->IASetPrimitiveTopology(prevTopo);

    if (prevIL) prevIL->Release();
    if (prevVS) prevVS->Release();
    if (prevPS) prevPS->Release();
}