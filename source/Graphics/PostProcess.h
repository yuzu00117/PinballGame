//------------------------------------------------------------------------------
// PostProcess.h
//------------------------------------------------------------------------------
// 役割:
// ポストプロセス（ブルーム・トーンマップ等）の描画処理を管理するクラス。
//
// 設計意図:
// Renderer クラスの肥大化を防ぐため、HDR バッファから最終的なバックバッファへの
// 輝度抽出・ブラー・合成処理を本クラスで一手に担う。
//
// 構成:
// - ポストプロセス用のシェーダーリソースおよびテクスチャの管理
// - ポストプロセスの描画フローの実行
//
// NOTE:
// メインの HDR テクスチャは Renderer 側で管理し、本クラスにはその SRV のみを渡す。
// 内部で利用する 1/4 縮小バッファなどの所有権は本クラスが持つ。
//------------------------------------------------------------------------------
#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

class PostProcess
{
private:
    // ------------------------------------------------------------------------------
    // 構造体定義
    // ------------------------------------------------------------------------------
    struct BlurCB {
        DirectX::XMFLOAT2 TexelSize;
        DirectX::XMFLOAT2 Dir;
    };

    // ------------------------------------------------------------------------------
    // メンバ変数
    // ------------------------------------------------------------------------------
    ID3D11Texture2D*          m_LuminanceTex;      // 所有：1/4 縮小輝度テクスチャ
    ID3D11RenderTargetView*   m_LuminanceRTV;      // 所有：Luminance 用 RTV
    ID3D11ShaderResourceView* m_LuminanceSRV;      // 所有：Luminance 用 SRV

    ID3D11Texture2D*          m_BlurTex;           // 所有：1/4 縮小ブラーテクスチャ
    ID3D11RenderTargetView*   m_BlurRTV;           // 所有：Blur 用 RTV
    ID3D11ShaderResourceView* m_BlurSRV;           // 所有：Blur 用 SRV

    ID3D11VertexShader*       m_PostVS;            // 所有：ポストプロセス用 VertexShader
    ID3D11PixelShader*        m_PostLuminancePS;   // 所有：輝度抽出用 PixelShader
    ID3D11PixelShader*        m_PostBlurPS;        // 所有：ブラー用 PixelShader
    ID3D11PixelShader*        m_PostCompositePS;   // 所有：合成用 PixelShader

    ID3D11Buffer*             m_BlurCBuffer;       // 所有：ブラー方向等用の定数バッファ

public:
    // ------------------------------------------------------------------------------
    // 関数定義
    // ------------------------------------------------------------------------------

    /// ポストプロセス用リソースの初期化
    /// - 内部で使用するテクスチャ・RTV・SRV・シェーダーの生成を行う
    void Init();

    /// ポストプロセス用リソースの破棄
    /// - 確保したすべての中間リソース・シェーダーのメモリを解放する
    void Uninit();

    /// ポストプロセスパスの実行
    /// - 非所有: mainHdrSRV (Renderer 側から渡されるメイン HDR 画像)
    /// - mainHdrSRV を元に輝度抽出・ブラーを行い、最終結果を現在の RTV に書き込む
    /// - ※呼び出し前に、書き込み先の RTV / Viewport 等が正しく設定されていること
    void Draw(ID3D11ShaderResourceView* mainHdrSRV);
};
