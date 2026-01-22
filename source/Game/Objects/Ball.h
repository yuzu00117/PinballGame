//------------------------------------------------------------------------------
// Ball
//------------------------------------------------------------------------------
// ピンボールゲームにおけるボール（球体）を表すモジュール。
// - GameObject を継承した可動オブジェクト
// - 物理挙動・衝突・描画を統合して管理する
//
// 構成：
// - 描画：ModelRenderer
// - 衝突：ColliderGroup + SphereCollider
// - 物理：RigidBody（重力・反発係数）
//
// 本ファイルは「ボールというゲーム上の実体」を定義し、
// 個々の物理計算や衝突判定の詳細は各 Component に委譲する。
//------------------------------------------------------------------------------
#ifndef _BALL_H
#define _BALL_H

#include "Vector3.h"
#include "GameObject.h"

// 前方宣言
class ModelRenderer;
class ColliderGroup;
class RigidBody;

/// ピンボールのボールクラス
/// - 描画：ModelRenderer
/// - 衝突：ColliderGroup + SphereCollider
/// - 物理：RigidBody（重力・反発係数）
/// - 高さ制限：テーブル面の上下（kTableMinY〜kTableMaxY）を超えないように補正
class Ball : public GameObject
{
public:
    // ----------------------------------------------------------------------
    // ライフサイクルメソッド
    // ----------------------------------------------------------------------
    /// 初期化処理（Component の追加や初期値設定を行う）
    void Init() override;
    /// 終了処理（明示的な解放が必要な場合に実装）
    void Uninit() override;
    /// 更新処理（deltaTimeは秒単位）
    void Update(float deltaTime) override;
    /// 描画処理
    void Draw() override;

    // ----------------------------------------------------------------------
    // アクセサ
    // ----------------------------------------------------------------------
    /// 位置を取得する（Transform.Position の参照）
    /// 戻り値：参照（呼び出し側で変更可能）
    Vector3& GetPosition() { return m_Transform.Position; }
    /// 速度を取得する（Ball 側の速度の参照）
    /// 戻り値：参照（呼び出し側で変更可能）
    Vector3& GetVelocity() { return m_Velocity; }

    /// 半径を取得する
    float GetRadius() const { return m_BallRadius; }

    // ----------------------------------------------------------------------
    // 操作
    // ----------------------------------------------------------------------
    /// ボールをリセットする
    /// - 位置を初期位置へ戻す
    /// - 速度をゼロクリアする
    void ResetBall();

private:
    // ----------------------------------------------------------------------
    // ボールのデフォルト設定
    // ----------------------------------------------------------------------
    inline static const Vector3 kDefaultBallScale = { 0.5f, 0.5f, 0.5f }; // デフォルトのボールスケール
    static constexpr float kDefaultBallRadius = 0.5f;                     // デフォルトのボール半径
    static constexpr float kDefaultBallBounce = 0.15f;                    // デフォルトの反発係数

    /// テーブル面の高さ制限（Y軸）
    static constexpr float kTableMinY = 0.0f;                             // 床
    static constexpr float kTableMaxY = 1.0f;                             // 天井

    // ----------------------------------------------------------------------
    // コンポーネント参照
    // ----------------------------------------------------------------------
    ModelRenderer* m_ModelRenderer = nullptr;                             // 非所有：ModelRenderer
    ColliderGroup* m_ColliderGroup = nullptr;                             // 非所有：ColliderGroup
    RigidBody* m_RigidBody = nullptr;                                     // 非所有：RigidBody

    // ----------------------------------------------------------------------
    // ボールの属性
    // ----------------------------------------------------------------------
    Vector3 m_Velocity = { 0.0f, 0.0f, 0.0f };                            // 速度
    float m_BallRadius = kDefaultBallRadius;                              // 半径
    float m_BallBounce = kDefaultBallBounce;                              // 反発係数
};

#endif // _BALL_H
