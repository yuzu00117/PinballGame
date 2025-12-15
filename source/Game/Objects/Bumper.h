#ifndef _BUMPER_H
#define _BUMPER_H

#include "vector3.h"
#include "gameobject.h"

/// <summary>
/// ピンボールのバンパーオブジェクト
/// ボールが跳ね返るオブジェクト
/// </summary>
class Bumper : public GameObject
{
public:
    Bumper(const Vector3& c, float r, float rest = 1.1f, float kick = 1.0f)
        : m_Center(c), m_Radius(r), m_Restitution(rest), m_KickVelocity(kick) {}

    // --- 関数宣言 ---
    void Init() override;
    void Uninit() override;
    void Update(float deltaTime) override;
    void Draw() override;

    /// <summary>
    /// ボールとの衝突
    /// </summary>
    void Resolve(Vector3& ballPosition, Vector3& ballVelocity, float ballRadius);

    // --- ゲッター ---
    const Vector3& GetCenter() const { return m_Center; }
    float GetRadius() const { return m_Radius; }

private:
	// --- 定数定義 ---
    static constexpr float DefaultBumperRadius = 1.0f;          // デフォルトのバンパー半径
    static constexpr float DefaultBumperRestitution = 1.1f;     // デフォルトのバンパー反発係数
    static constexpr float DefaultBumperKickVelocity = 1.0f;    // デフォルトのバンパーキック速度

    // --- 変数定義 ---
    ID3D11Buffer*               m_VertexBuffer  = nullptr;
    ID3D11InputLayout*          m_VertexLayout  = nullptr;
    ID3D11VertexShader*         m_VertexShader  = nullptr;
    ID3D11PixelShader*          m_PixelShader   = nullptr;
    ID3D11ShaderResourceView*   m_Texture       = nullptr;
    class ModelRenderer*        m_ModelRenderer = nullptr;

    Vector3 m_Center{};                                         // バンパーの中心位置
    float m_Radius = DefaultBumperRadius;                       // バンパーの半径
    float m_Restitution = DefaultBumperRestitution;             // バンパーの反発係数
    float m_KickVelocity = DefaultBumperKickVelocity;           // バンパーのキック速度
};

#endif // _BUMPER_H