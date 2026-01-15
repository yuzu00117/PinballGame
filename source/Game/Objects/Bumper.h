#pragma once

#include "GameObject.h"
#include "Vector3.h"

// 前方宣言
class CollliderGroup;
class ModelRenderer;

/// <summary>
/// ピンボールのバンパークラス
/// ボールが衝突した際にボールを弾く
/// </summary>
class Bumper : public GameObject
{
public:
    // ----------------------------------------------------------------------
    // 関数定義
    // ----------------------------------------------------------------------
    /// <summary>
    /// ライフサイクルメソッド
    /// </summary>
    void Init() override;
    void Update(float deltaTime) override;
    void Draw() override;
    void Uninit() override;

    /// <summary>
    /// 衝突コールバック
    /// </summary>
    void OnCollisionEnter(const CollisionInfo& info) override;

private:
    // ----------------------------------------------------------------------
    // 定数定義
    // ----------------------------------------------------------------------
    // コライダー設定
    static constexpr float kDefaultColliderRadius = 2.0f;           // デフォルトのコライダー半径

    // バンパーがボールを弾く速度
    static constexpr float kBumperKickHorizontalSpeed = 25.0f;      // バンパーが水平に弾く速度
    static constexpr float kBumperKickVerticalSpeed = 15.0f;        // バンパーが垂直に弾く速度

    // ----------------------------------------------------------------------
    // 変数定義
    // ----------------------------------------------------------------------
    // コンポーネントのポインタ
    ModelRenderer* m_ModelRenderer = nullptr;                       // モデルレンダラー
};
