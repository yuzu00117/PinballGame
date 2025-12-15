#pragma once

#include "GameObject.h"
#include "Vector3.h"

class CollliderGroup;
class MeshRenderer;

/// <summary>
/// ピンボールのフリッパクラス
/// 親オブジェクトは回転軸、子オブジェクトにアームメッシュを持つ
/// 実際の配置はFieldクラスで行っています
/// TODO: 現在の実装では、どの位置で反射しても同じ力で弾く仕様になっているため、
///       将来的に「当たった位置で弾く力が変わる」ように改良したい
/// </summary>
class Flipper : public GameObject
{
public:
    // ----------------------------------------------------------------------
    // 構造体定義
    // ----------------------------------------------------------------------
    enum class Side
    {
        Left,
        Right
    };

    // ----------------------------------------------------------------------
    // 関数定義
    // ----------------------------------------------------------------------

    /// <summary>
    /// コンストラクタ
    /// </summary>
    Flipper(Side side); 

    /// <summary>
    /// ライフサイクルメソッド
    /// </summary>
    void Init() override;
    void Update(float deltaTime) override;
    void Draw() override;
    void Uninit() override;

    /// <summary>
    /// 衝突コールバック
    /// フリッパーでボールを弾くための簡易実装
    /// </summary>
    void OnCollisionStay(const CollisionInfo& info) override;

private:
    // ----------------------------------------------------------------------
    // 定数定義
    // ----------------------------------------------------------------------
    static constexpr float kDefaultArmLength = 6.0f;                // デフォルトアーム長さ
    static constexpr float kDefaultArmThickness = 1.5f;             // デフォルトアーム厚さ
    static constexpr float kDefaultArmHeight = 2.0f;                // デフォルトアーム高さ
    static constexpr float kFlipperRotateSpeedDegPerSec = 360.0f;   // フリッパー回転速度（度/秒）
    static constexpr float kFlipperHorizontalSpeed = 50.0f;         // フリッパーがボールを弾くときの水平速度調整値
    static constexpr float kFlipperUpSpeed = 2.5f;                  // フリッパーがボールを弾くときの上方向速度調整値
    // HACK: フレームレートをどこかで設定するようにしたい
    static constexpr float kDeltaTime = 1.0f / 60.0f;               // フレーム想定時間（60FPS固定想定）
    
    // ----------------------------------------------------------------------
    // 変数定義
    // ----------------------------------------------------------------------
    // 角度関連
    float m_DefaultAngle = 0.0f;                                    // 休み位置
    float m_ActiveAngle  = 0.0f;                                    // 動作位置

    Side m_Side;                                                    // 左右の区別用構造体変数

    // アーム用子オブジェクト
    GameObject* m_ArmObject = nullptr;                              // アーム用子オブジェクトポインタ

    // アーム形状
    float m_ArmLength = kDefaultArmLength;                          // アーム長さ
    float m_ArmThickness = kDefaultArmThickness;                    // アーム厚さ
    float m_ArmHeight = kDefaultArmHeight;                          // アーム高さ
    
    static constexpr const char* VertexShaderPath =                 // 頂点シェーダのパス
        "shader\\bin\\unlitTextureVS.cso";   
    static constexpr const char* PixelShaderPath  =                 // ピクセルシェーダのパス
        "shader\\bin\\unlitTexturePS.cso";   
    
    BYTE GetActiveKey() const;                                      // 動作キー取得
};  