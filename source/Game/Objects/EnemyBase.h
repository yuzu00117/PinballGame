#pragma once

#include "GameObject.h"
#include "Vector3.h"

class CollliderGroup;
class MeshRenderer;

/// <summary>
/// エネミー共通の基底クラス
/// 全エネミーはこのクラスを継承して作成される
/// </summary>
class EnemyBase : public GameObject
{
public:
    // ----------------------------------------------------------------------
    // 関数定義
    // ----------------------------------------------------------------------
    /// <summary>
    /// ライフサイクルメソッド
    /// </summary>
    void Init() override;
    void Update() override;
    void Draw() override;
    void Uninit() override;

    /// <summary>
    /// 衝突コールバック
    /// フリッパーでボールを弾くための簡易実装
    /// </summary>
    void OnCollisionEnter(const CollisionInfo& info) override;

private:
    // ----------------------------------------------------------------------
    // 関数定義
    // ----------------------------------------------------------------------
    /// <summary>
    /// ターゲットへの正規化方向
    /// </summary>
    Vector3 GetDirToTarget() const;

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
    // 共通パラメーター
    float m_Speed = 0.15f;                      // 移動速度
    int   m_HP = 1;                             // 体力
    int   m_Score = 100;                        // スコア値
    Vector3 m_TargetPos = { 0.0f, 0.0f, 0.0f }; // 目標位置
    Vector3 m_Velocity = { 0.0f, 0.0f, 0.0f };  // 現在速度

    // コンポーネント
    CollliderGroup* m_ColliderGroup = nullptr; // コライダーグループコンポーネント
    MeshRenderer*   m_MeshRenderer = nullptr;   // メッシュレンダラーコンポーネント
};