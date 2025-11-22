#pragma once

#include "GameObject.h"
#include "Vector3.h"

class CollliderGroup;
class MeshRenderer;

/// <summary>
/// ピンボールのフリッパクラス
/// 親オブジェクト: 回転軸
/// 子オブジェクト: 実際のアーム
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
    void Update() override;
    void Draw() override;
    void Uninit() override;

private:
    // ----------------------------------------------------------------------
    // 定数定義
    // ----------------------------------------------------------------------
    static constexpr float kDefaultArmLength = 4.0f;        // デフォルトアーム長さ
    static constexpr float kDefaultArmThickness = 0.6f;     // デフォルトアーム厚さ
    static constexpr float kDefaultArmHeight = 0.5f;        // デフォルトアーム高さ
    
    // ----------------------------------------------------------------------
    // 変数定義
    // ----------------------------------------------------------------------
    // 角度関連
    float m_DefaultAngle = 0.0f;                            // 休み位置
    float m_ActiveAngle  = 0.0f;                            // 動作位置

    Side m_Side;                                            // 左右の区別用構造体変数

    // アーム用子オブジェクト
    GameObject* m_ArmObject = nullptr;                      // アーム用子オブジェクトポインタ

    // アーム形状
    float m_ArmLength = kDefaultArmLength;                  // アーム長さ
    float m_ArmThickness = kDefaultArmThickness;            // アーム厚さ
    float m_ArmHeight = kDefaultArmHeight;                  // アーム高さ
    
    BYTE GetActiveKey() const;                              // 動作キー取得
};  