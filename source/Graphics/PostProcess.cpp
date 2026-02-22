#include "PostProcess.h"

#include "Renderer.h"
#include "main.h"
#include <io.h>
#include <stdio.h>

// ------------------------------------------------------------------------------
// 初期化・終了処理
// ------------------------------------------------------------------------------
// - テクスチャ、シェーダー、定数バッファの生成と破棄
// - 画面サイズに依存したバッファを構築する

void PostProcess::Init()
{
    ID3D11Device* device = Renderer::GetDevice();
    HRESULT hr;

    m_LuminanceTex = nullptr;
    m_LuminanceRTV = nullptr;
    m_LuminanceSRV = nullptr;

    m_BlurTex = nullptr;
    m_BlurRTV = nullptr;
    m_BlurSRV = nullptr;

    m_PostVS = nullptr;
    m_PostLuminancePS = nullptr;
    m_PostBlurPS = nullptr;
    m_PostCompositePS = nullptr;
    m_BlurCBuffer = nullptr;

    // 抽出・ぼかし用RenderTarget生成 (1/4縮小)
    D3D11_TEXTURE2D_DESC hdrTexDesc{};
    hdrTexDesc.Width = SCREEN_WIDTH / 4;
    hdrTexDesc.Height = SCREEN_HEIGHT / 4;
    hdrTexDesc.MipLevels = 1;
    hdrTexDesc.ArraySize = 1;
    hdrTexDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    hdrTexDesc.SampleDesc.Count = 1;
    hdrTexDesc.SampleDesc.Quality = 0;
    hdrTexDesc.Usage = D3D11_USAGE_DEFAULT;
    hdrTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    hr = device->CreateTexture2D(&hdrTexDesc, NULL, &m_LuminanceTex);
    if (SUCCEEDED(hr)) {
        device->CreateRenderTargetView(m_LuminanceTex, NULL, &m_LuminanceRTV);
        device->CreateShaderResourceView(m_LuminanceTex, NULL, &m_LuminanceSRV);
    }

    hr = device->CreateTexture2D(&hdrTexDesc, NULL, &m_BlurTex);
    if (SUCCEEDED(hr)) {
        device->CreateRenderTargetView(m_BlurTex, NULL, &m_BlurRTV);
        device->CreateShaderResourceView(m_BlurTex, NULL, &m_BlurSRV);
    }

    // ポストプロセス用シェーダー読み込み
    FILE* fp = nullptr;
    errno_t err;
    long int fsize;
    unsigned char* buffer;

    // VS
    err = fopen_s(&fp, "shader\\bin\\PostVS.cso", "rb");
    if (err == 0 && fp) {
        fseek(fp, 0, SEEK_END); fsize = ftell(fp); fseek(fp, 0, SEEK_SET);
        buffer = new unsigned char[fsize]; fread(buffer, fsize, 1, fp); fclose(fp);
        device->CreateVertexShader(buffer, fsize, NULL, &m_PostVS);
        delete[] buffer;
    }

    // PS
    err = fopen_s(&fp, "shader\\bin\\PostLuminancePS.cso", "rb");
    if (err == 0 && fp) {
        fseek(fp, 0, SEEK_END); fsize = ftell(fp); fseek(fp, 0, SEEK_SET);
        buffer = new unsigned char[fsize]; fread(buffer, fsize, 1, fp); fclose(fp);
        device->CreatePixelShader(buffer, fsize, NULL, &m_PostLuminancePS);
        delete[] buffer;
    }
    err = fopen_s(&fp, "shader\\bin\\PostBlurPS.cso", "rb");
    if (err == 0 && fp) {
        fseek(fp, 0, SEEK_END); fsize = ftell(fp); fseek(fp, 0, SEEK_SET);
        buffer = new unsigned char[fsize]; fread(buffer, fsize, 1, fp); fclose(fp);
        device->CreatePixelShader(buffer, fsize, NULL, &m_PostBlurPS);
        delete[] buffer;
    }
    err = fopen_s(&fp, "shader\\bin\\PostCompositePS.cso", "rb");
    if (err == 0 && fp) {
        fseek(fp, 0, SEEK_END); fsize = ftell(fp); fseek(fp, 0, SEEK_SET);
        buffer = new unsigned char[fsize]; fread(buffer, fsize, 1, fp); fclose(fp);
        device->CreatePixelShader(buffer, fsize, NULL, &m_PostCompositePS);
        delete[] buffer;
    }

    D3D11_BUFFER_DESC bcDesc{};
    bcDesc.Usage = D3D11_USAGE_DEFAULT;
    bcDesc.ByteWidth = sizeof(BlurCB);
    bcDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    device->CreateBuffer(&bcDesc, NULL, &m_BlurCBuffer);
}

void PostProcess::Uninit()
{
    if (m_PostCompositePS) { m_PostCompositePS->Release(); m_PostCompositePS = nullptr; }
    if (m_PostBlurPS)      { m_PostBlurPS->Release(); m_PostBlurPS = nullptr; }
    if (m_PostLuminancePS) { m_PostLuminancePS->Release(); m_PostLuminancePS = nullptr; }
    if (m_PostVS)          { m_PostVS->Release(); m_PostVS = nullptr; }
    if (m_BlurCBuffer)     { m_BlurCBuffer->Release(); m_BlurCBuffer = nullptr; }
    if (m_BlurSRV)         { m_BlurSRV->Release(); m_BlurSRV = nullptr; }
    if (m_BlurRTV)         { m_BlurRTV->Release(); m_BlurRTV = nullptr; }
    if (m_BlurTex)         { m_BlurTex->Release(); m_BlurTex = nullptr; }
    if (m_LuminanceSRV)    { m_LuminanceSRV->Release(); m_LuminanceSRV = nullptr; }
    if (m_LuminanceRTV)    { m_LuminanceRTV->Release(); m_LuminanceRTV = nullptr; }
    if (m_LuminanceTex)    { m_LuminanceTex->Release(); m_LuminanceTex = nullptr; }
}

// ------------------------------------------------------------------------------
// 描画処理
// ------------------------------------------------------------------------------
// - 輝度抽出・水平方向ブラー・垂直方向ブラー・合成の4パスを実行する
// - 最終結果は呼び出し元の RTV に書き込まれる

void PostProcess::Draw(ID3D11ShaderResourceView* mainHdrSRV)
{
    ID3D11DeviceContext* context = Renderer::GetDeviceContext();
    ID3D11ShaderResourceView* nullSRV[] = { nullptr, nullptr };

    // この関数開始時点で設定されている RTV や Viewport を取得して退避（必要であれば）
    // （今回は Renderer 側で確実にバックバッファ等のRTVが設定されている想定での実装とする）
    ID3D11RenderTargetView* currentRTV;
    ID3D11DepthStencilView* currentDSV;
    context->OMGetRenderTargets(1, &currentRTV, &currentDSV);
    
    UINT numViewports = 1;
    D3D11_VIEWPORT currentViewport;
    context->RSGetViewports(&numViewports, &currentViewport);

    // --------------------------------------------------------
    // 1. 輝度抽出パス (HDR -> LuminanceTex 1/4)
    // --------------------------------------------------------
    context->OMSetRenderTargets(1, &m_LuminanceRTV, nullptr);
    D3D11_VIEWPORT vpQ = { 0.0f, 0.0f, (float)SCREEN_WIDTH / 4, (float)SCREEN_HEIGHT / 4, 0.0f, 1.0f };
    context->RSSetViewports(1, &vpQ);

    context->IASetInputLayout(nullptr);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->VSSetShader(m_PostVS, nullptr, 0);
    context->PSSetShader(m_PostLuminancePS, nullptr, 0);

    context->PSSetShaderResources(0, 1, &mainHdrSRV);
    context->Draw(3, 0);
    context->PSSetShaderResources(0, 2, nullSRV); // 外す

    // --------------------------------------------------------
    // 2. ぼかしパス1 (LuminanceTex -> BlurTex) 横方向
    // --------------------------------------------------------
    context->OMSetRenderTargets(1, &m_BlurRTV, nullptr);
    context->PSSetShader(m_PostBlurPS, nullptr, 0);

    BlurCB cb{};
    cb.TexelSize = { 1.0f / (SCREEN_WIDTH / 4.0f), 1.0f / (SCREEN_HEIGHT / 4.0f) };
    cb.Dir = { 1.0f, 0.0f };
    context->UpdateSubresource(m_BlurCBuffer, 0, nullptr, &cb, 0, 0);
    context->PSSetConstantBuffers(0, 1, &m_BlurCBuffer);

    context->PSSetShaderResources(0, 1, &m_LuminanceSRV);
    context->Draw(3, 0);
    context->PSSetShaderResources(0, 2, nullSRV);

    // --------------------------------------------------------
    // 3. ぼかしパス2 (BlurTex -> LuminanceTex) 縦方向
    // --------------------------------------------------------
    context->OMSetRenderTargets(1, &m_LuminanceRTV, nullptr);
    cb.Dir = { 0.0f, 1.0f };
    context->UpdateSubresource(m_BlurCBuffer, 0, nullptr, &cb, 0, 0);

    context->PSSetShaderResources(0, 1, &m_BlurSRV);
    context->Draw(3, 0);
    context->PSSetShaderResources(0, 2, nullSRV);

    // --------------------------------------------------------
    // 4. 合成・トーンマッピングパス (Main + LuminanceTex -> BackBuffer)
    // --------------------------------------------------------
    // 退避しておいた元のRenderTargetを戻す
    context->OMSetRenderTargets(1, &currentRTV, currentDSV);
    context->RSSetViewports(numViewports, &currentViewport);

    context->PSSetShader(m_PostCompositePS, nullptr, 0);
    
    ID3D11ShaderResourceView* srvs[] = { mainHdrSRV, m_LuminanceSRV };
    context->PSSetShaderResources(0, 2, srvs);
    context->Draw(3, 0);
    context->PSSetShaderResources(0, 2, nullSRV);

    if (currentRTV) currentRTV->Release();
    if (currentDSV) currentDSV->Release();
}
