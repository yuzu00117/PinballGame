//------------------------------------------------------------------------------
// Ball
//------------------------------------------------------------------------------
// 役割:
// ピンボールゲームのボール（球体）を表す GameObject 派生クラス。
// 描画・衝突・物理の各機能は Component に委譲し、Ball は「ゲーム上の実体」として
// 初期値設定・制約（高さ制限）・リセット操作などを提供する。
//
// 設計意図:
// - 機能は Component（ModelRenderer / Collider / RigidBody）へ分離する
// - Ball 自体は “組み合わせ” と “ゲームルール上の補正” を担当する
// - 速度は RigidBody 側を主に使う想定だが、現状は Ball 側にも m_Velocity があるため
//   運用を混在させないように注意する（NOTE参照）
//
// 構成:
// - 描画：ModelRenderer
// - 衝突：ColliderGroup + SphereCollider
// - 物理：RigidBody（重力・反発係数）
// - 制約：テーブル面の上下（kTableMinY〜kTableMaxY）の高さ制限
//
// NOTE:
// - 現状 m_Velocity（Ball側）と m_RigidBody->m_Velocity（物理側）が二重管理になっている。
//   どちらを正とするか統一しないと、バグの温床になる。
//   例：ResetBall は両方をゼロ化しているが、Update は Ball側 m_Velocity を使っていない。
//------------------------------------------------------------------------------
#pragma once

#include "GameObject.h"
#include "Vector3.h"

// 前方宣言
class ModelRenderer;
class ColliderGroup;
class RigidBody;

/// ピンボールのボールクラス
/// - Component の組み合わせで「描画/衝突/物理」を構成する
/// - Ball は初期設定・制約（高さ制限）・リセット操作を担当する
class Ball : public GameObject
{
public:
    // ----------------------------------------------------------------------
    // ライフサイクルメソッド
    // ----------------------------------------------------------------------
    /// 初期化処理
    /// - Component を追加し初期値を設定する
    /// - 重力方向（傾き）をピンボール用に設定する
    void Init() override;

    /// 終了処理
    /// NOTE: Component の実体は GameObject が unique_ptr で所有しているため、
    ///       ここでは参照ポインタ（非所有）を nullptr に戻すだけでよい
    void Uninit() override;

    /// 更新処理（deltaTimeは秒単位）
    /// - Component / 子オブジェクト更新は GameObject::Update に委譲
    /// - ボールの高さ制限（Y）を適用する
    void Update(float deltaTime) override;

    /// 描画処理
    /// - Component / 子オブジェクト描画は GameObject::Draw に委譲
    void Draw() override;

    // ----------------------------------------------------------------------
    // アクセサ
    // ----------------------------------------------------------------------
    /// 位置を取得する（Transform.Position の参照）
    /// 戻り値：参照（呼び出し側で変更可能）
    Vector3& GetPosition() { return m_Transform.Position; }

    /// 速度を取得する（Ball 側の速度の参照）
    /// 戻り値：参照（呼び出し側で変更可能）
    /// NOTE: 現状 Ball側速度は物理更新に使われていない可能性があるため運用注意
    Vector3& GetVelocity() { return m_Velocity; }

    /// 半径を取得する
    float GetRadius() const { return m_BallRadius; }

    // ----------------------------------------------------------------------
    // 操作
    // ----------------------------------------------------------------------
    /// ボールをリセットする
    /// - 位置を初期位置へ戻す
    /// - 速度をゼロクリアする（Ball側 / RigidBody側）
    void ResetBall();

private:
    // ----------------------------------------------------------------------
    // ボールのデフォルト属性
    // ----------------------------------------------------------------------
    inline static const Vector3 kDefaultBallScale = { 0.5f, 0.5f, 0.5f };
    static constexpr float kDefaultBallRadius = 0.5f;
    static constexpr float kDefaultBallBounce = 0.15f;

    /// テーブル面の高さ制限（Y軸）
    static constexpr float kTableMinY = 0.0f;  // 床
    static constexpr float kTableMaxY = 1.0f;  // 天井

    /// 初期位置
    inline static const Vector3 kDefaultBallPosition = { 8.0f, 1.0f, 0.0f };

private:
    // ----------------------------------------------------------------------
    // Component 参照（非所有）
    // ----------------------------------------------------------------------
    ModelRenderer* m_ModelRenderer = nullptr;  // 非所有：ModelRenderer
    ColliderGroup* m_ColliderGroup = nullptr;  // 非所有：ColliderGroup
    RigidBody*     m_RigidBody     = nullptr;  // 非所有：RigidBody

    // ----------------------------------------------------------------------
    // ボール属性
    // ----------------------------------------------------------------------
    Vector3 m_Velocity = { 0.0f, 0.0f, 0.0f };  // NOTE: 二重管理に注意（ファイルヘッダ参照）
    float   m_BallRadius = kDefaultBallRadius;
    float   m_BallBounce = kDefaultBallBounce;
};
