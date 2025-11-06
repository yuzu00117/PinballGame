#ifndef _FLIPPER_H
#define _FLIPPER_H

#include "vector3.h"
#include "gameobject.h"
#include <algorithm>
#include <cmath>
#include <windows.h>

/// <summary>
/// ピンボールのフリッパーオブジェクト
/// ADキーで回転操作可能
/// </summary>
struct FlipperDesc {
    Vector3 pivot = { 0.0f, 0.5f, 0.0f }; // ピボット（y は高さ）
    float   length = 1.2f;                // XZ平面の長さ（ワールド単位）
    float   thickness = 0.18f;            // 幅（当たり判定の半径は thickness*0.5）
    float   restAngle = 0.0f;             // 休止角度[rad]（+X 方向基準、反時計回り+）
    float   maxAngle  = 0.9f;             // 押し上げ角[rad]
    float   upSpeed   = 18.0f;            // 上げ角速度[rad/s]
    float   downSpeed = 10.0f;            // 戻り角速度[rad/s]
    float   restitution = 0.35f;          // 反発係数
    float   hitBoost    = 1.2f;           // 先端速度をどれだけ乗せるか
    bool    invert = false;               // 右フリッパー用（回転向きを反転）
    BYTE    key = VK_LEFT;                // 入力キー（左/右で設定）
};

class Flipper : public GameObject
{
public:
    // --- 関数宣言 ---
    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;

    /// <summary>
    /// ボールとの衝突
    /// </summary>
    void Resolve(Vector3& ballPosition, Vector3& ballVelocity, float ballRadius, float DeltaTime);

    void Reset(const FlipperDesc& desc) { m_Desc = desc; m_CurrentAngle = desc.restAngle; }
    const FlipperDesc& GetDesc() const { return m_Desc; }

private:
    // --- 関数定義 ---
    /// <summary>
    /// フリッパーの先端と根元の位置を取得
    /// </summary>
    void GetSegment(Vector3& outA, Vector3& outB) const;

	// --- 定数定義 ---

    // --- 変数定義 ---
    ID3D11Buffer*               m_VertexBuffer  = nullptr;
    ID3D11InputLayout*          m_VertexLayout  = nullptr;
    ID3D11VertexShader*         m_VertexShader  = nullptr;
    ID3D11PixelShader*          m_PixelShader   = nullptr;
    ID3D11ShaderResourceView*   m_Texture       = nullptr;
    class ModelRenderer*        m_ModelRenderer = nullptr;

    FlipperDesc m_Desc;
    float m_CurrentAngle = 0.0f; // 現在の角度
    float m_PrevAngle    = 0.0f; // 1フレーム前の角度
};

#endif // _FLIPPER_H