#ifndef _BALL_H
#define _BALL_H

#include "vector3.h"
#include "gameobject.h"

/// <summary>
/// ピンボールのボールクラス
/// </summary>
class Ball : public GameObject
{
public:
    // --- 関数宣言 ---
    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;

    // 位置の取得・設定
    const Vector3& GetPosition() const { return m_Position; }
    void SetPosition(const Vector3& pos) { m_Position = pos; }

    // 速度の取得・設定
    const Vector3& GetVelocity() const { return m_Velocity; }
    void SetVelocity(const Vector3& vel) { m_Velocity = vel; }

    // 半径の取得
    float GetRadius() const { return m_Radius; }

private:
	// --- 定数定義 ---
	static const Vector3 DefaultBallScale;                          // デフォルトのボールスケール
	static constexpr float DefaultBallRadius = 0.5f;                // デフォルトのボール半径
	static constexpr float DefaultBallBounce = 0.8f;                // デフォルトの反発係数

    // --- 変数定義 ---
    ID3D11InputLayout *m_VertexLayout;                              // 頂点レイアウト
    ID3D11VertexShader *m_VertexShader;                             // 頂点シェーダー
    ID3D11PixelShader *m_PixelShader;                               // ピクセルシェーダー
    class ModelRenderer *m_ModelRenderer;                           // モデルレンダラー
    ID3D11ShaderResourceView *m_Texture;                            // テクスチャ

    Vector3 m_Position = { 0.0f, 0.0f, 0.0f };                      // 位置
    Vector3 m_Rotation = { 0.0f, 0.0f, 0.0f };                      // 回転
    Vector3 m_Scale = DefaultBallScale;                             // スケール
    Vector3 m_Velocity = { 0.0f, 0.0f, 0.0f };                      // 速度
    float m_Radius = DefaultBallRadius;                             // 半径
    float m_Bounce = DefaultBallBounce;                             // 反発係数
};

#endif // _BALL_H