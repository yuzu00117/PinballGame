#ifndef _BALL_H
#define _BALL_H

#include "Vector3.h"
#include "GameObject.h"

// 前方宣言
class ModelRenderer;
class ColliderGroup;
class RigidBody;

/// <summary>
/// ピンボールのボールクラス
/// </summary>
class Ball : public GameObject
{
public:
    // ------------------------------------------------------------------------------
    // 関数宣言
    // ------------------------------------------------------------------------------
    /// <summary>
    /// ライフサイクルメソッド
    /// </summary>
    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;

    /// <summary>
    /// ゲッター
    /// 現在はTransformのPositionとVelocityのみ公開
    /// </summary>
    Vector3& GetPosition() { return m_Transform.Position; }
    Vector3& GetVelocity() { return m_Velocity; }

    /// <summary>
    /// 半径の取得
    /// </summary>
    float GetRadius() const { return m_Radius; }

    /// <summary>
    /// ボールをリセットする
    ///     ・ボールをリスポーンさせる
    /// </summary>
    void ResetBall();

private:
    // ------------------------------------------------------------------------------
	// 定数定義
    // ------------------------------------------------------------------------------
    // ボールの属性デフォルト値
	inline static const Vector3 kDefaultBallScale = { 0.5f, 0.5f, 0.5f };   // デフォルトのボールスケール
	static constexpr float kDefaultBallRadius = 0.5f;                       // デフォルトのボール半径
	static constexpr float kDefaultBallBounce = 0.15f;                      // デフォルトの反発係数
    // テーブル面の高さと上に飛んで良い高さの制限
    static constexpr float kTableMinY = 0.0f;                               // 床
    static constexpr float kTableMaxY = 1.0f;                               // 天井

    // ------------------------------------------------------------------------------
    // 変数定義 
    // ------------------------------------------------------------------------------
    ModelRenderer* m_ModelRenderer = nullptr;                               // モデルレンダラー
    ColliderGroup* m_ColliderGroup = nullptr;                               // コリジョングループ
    RigidBody* m_RigidBody = nullptr;                                       // リジッドボディ

    // ボールの属性
    Vector3 m_Velocity = { 0.0f, 0.0f, 0.0f };                              // 速度
    float m_Radius = kDefaultBallRadius;                                    // 半径
    float m_Bounce = kDefaultBallBounce;                                    // 反発係数
};

#endif // _BALL_H