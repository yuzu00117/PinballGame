//------------------------------------------------------------------------------
// Sprite
//------------------------------------------------------------------------------
// 役割:
// スクリーン座標ベースの2Dスプライト描画を行うクラス。
// UI・HUD・タイトル背景など、画面に直接貼り付ける2D画像の表示に使用する。
//
// 設計意図:
// スクリーン座標（左上原点・ピクセル単位）で位置とサイズを指定し、
// SetWorldViewProjection2D による正射影投影で描画する。
// 頂点バッファを DYNAMIC にすることで、SetPosition / SetSize 等を
// 毎フレーム呼んでもパラメータが即反映される設計とする。
//
// 構成:
// - SetPosition / SetSize  : スクリーン座標・サイズ指定
// - SetColor               : 色・不透明度設定
// - SetUV                  : UV範囲設定（スプライトシート対応）
// - SetTexture             : テクスチャ読み込み
// - Init / Draw            : ライフサイクル
//
// NOTE:
// Draw() 内で毎フレーム UpdateVertexBuffer() を呼ぶため、
// SetPosition 等は Draw の前に呼ぶこと。
// 深度テストは Draw() 内で自動的に無効化・復元する。
//------------------------------------------------------------------------------
#pragma once

#include "main.h"
#include "GameObject.h"

class Sprite : public GameObject
{
public:
    void Init()   override;
    void Uninit() override;
    void Update(float deltaTime) override {}
    void Draw()   override;

    // ------------------------------------------------------------------
    // 設定関数
    // ------------------------------------------------------------------

    /// スクリーン座標（左上原点・ピクセル単位）を設定する
    void SetPosition(float x, float y) { m_X = x; m_Y = y; }

    /// 表示サイズ（ピクセル単位）を設定する
    void SetSize(float width, float height) { m_Width = width; m_Height = height; }

    /// 色・不透明度を設定する
    /// - 各成分は 0.0?1.0 の範囲で指定する
    void SetColor(float r, float g, float b, float a = 1.0f)
    {
        m_Color = XMFLOAT4(r, g, b, a);
    }

    /// UV範囲を設定する
    /// - スプライトシートのフレームアニメーションに使用する
    /// @param u0,v0  UVの左上  @param u1,v1  UVの右下
    void SetUV(float u0, float v0, float u1, float v1)
    {
        m_UV[0] = u0; m_UV[1] = v0;
        m_UV[2] = u1; m_UV[3] = v1;
    }

    /// テクスチャを読み込む
    /// - WIC 対応形式（png / jpg / bmp）を指定する
    /// - 既存テクスチャがある場合は解放してから再読み込みする
    void SetTexture(const std::wstring& filePath);

private:
    /// 頂点バッファをメンバパラメータで更新する
    /// - NOTE: 毎フレーム Draw() から呼ばれる
    void UpdateVertexBuffer();

    // ------------------------------------------------------------------
    // シェーダーリソース（所有）
    // ------------------------------------------------------------------
    ID3D11Buffer*             m_VertexBuffer = nullptr; // 所有：動的頂点バッファ
    ID3D11VertexShader*       m_VertexShader = nullptr; // 所有：頂点シェーダー
    ID3D11PixelShader*        m_PixelShader  = nullptr; // 所有：ピクセルシェーダー
    ID3D11InputLayout*        m_VertexLayout = nullptr; // 所有：入力レイアウト
    ID3D11ShaderResourceView* m_Texture      = nullptr; // 所有：テクスチャ（任意）

    // ------------------------------------------------------------------
    // 表示パラメータ
    // ------------------------------------------------------------------
    float    m_X      = 0.0f;                    // スクリーンX座標（左上原点）
    float    m_Y      = 0.0f;                    // スクリーンY座標（左上原点）
    float    m_Width  = 100.0f;                  // 表示幅（ピクセル）
    float    m_Height = 100.0f;                  // 表示高さ（ピクセル）
    XMFLOAT4 m_Color  = { 1.0f, 1.0f, 1.0f, 1.0f }; // 描画色（RGBA）
    float    m_UV[4]  = { 0.0f, 0.0f, 1.0f, 1.0f }; // UV範囲 [u0, v0, u1, v1]
};
