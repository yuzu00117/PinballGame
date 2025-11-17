#ifndef _BALL_H
#define _BALL_H

#include "Vector3.h"
#include "GameObject.h"

class ModelRenderer;
class ColliderGroup;

/// <summary>
/// ピンボールのボールクラス
/// </summary>
class Ball : public GameObject
{
public:
    // ------------------------------------------------------------------------------
    // 関数宣言
    // ------------------------------------------------------------------------------
    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;

    // ゲッター
    Vector3& GetPosition() { return m_Transform.Position; }
    Vector3& GetVelocity() { return m_Velocity; }

    // 半径の取得
    float GetRadius() const { return m_Radius; }

private:
    // ------------------------------------------------------------------------------
	// 定数定義
    // ------------------------------------------------------------------------------
    // ボールの属性デフォルト値
	inline static const Vector3 kDefaultBallScale = { 0.5f, 0.5f, 0.5f };   // デフォルトのボールスケール
	static constexpr float kDefaultBallRadius = 0.5f;                       // デフォルトのボール半径
	static constexpr float kDefaultBallBounce = 0.8f;                       // デフォルトの反発係数

    // ------------------------------------------------------------------------------
    // 変数定義 
    // ------------------------------------------------------------------------------
    ModelRenderer* m_ModelRenderer = nullptr;                               // モデルレンダラー
    ColliderGroup* m_ColliderGroup = nullptr;                               // コリジョングループ

    // ボールの属性
    Vector3 m_Velocity = { 0.0f, 0.0f, 0.0f };                              // 速度
    float m_Radius = kDefaultBallRadius;                                    // 半径
    float m_Bounce = kDefaultBallBounce;                                    // 反発係数
};

#endif // _BALL_H