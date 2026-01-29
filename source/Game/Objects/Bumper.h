//------------------------------------------------------------------------------
// Bumper
//------------------------------------------------------------------------------
// 役割:
// ピンボールのバンパーを表すゲームオブジェクト。
// ボールが衝突したときに反発（キック）を与え、プレイ感やリズムを作る。
//
// 設計意図:
// Bumper 自体は「バンパーとしての振る舞い（衝突時のキック）」のみを責務とし、
// 描画や当たり判定は GameObject の Component 機構で組み立てる。
// これにより、見た目（モデル）や判定形状の差し替えを Component 単位で行える。
//
// 構成:
// - 描画 : ModelRenderer
// - 衝突 : ColliderGroup + SphereCollider
//
// NOTE:
// 描画処理は Component（ModelRenderer）に委譲する。
// 反発量（キック速度）は定数として管理し、調整ポイントを明確化する。
//------------------------------------------------------------------------------
#pragma once

#include "GameObject.h"
#include "CollisionInfo.h"

// 前方宣言
class ColliderGroup;
class ModelRenderer;

/// バンパークラス
/// - ボール衝突時に反発（キック）を与える
/// - 描画や衝突は Component を追加して構築する
/// - 派生先で挙動を変える場合は OnCollisionEnter 等を override する
class Bumper : public GameObject
{
public:
    // ----------------------------------------------------------------------
    // ライフサイクルメソッド
    // ----------------------------------------------------------------------
    /// 初期化処理
    /// - Component の追加、モデル/コライダーの初期設定を行う
    /// - 副作用：AddComponent 直後に各 Component の Init() が呼ばれる
    void Init() override;

    /// 更新処理（deltaTime は秒単位）
    /// NOTE: 現状は演出等がないため何もしない。必要になったら状態更新を追加する。
    void Update(float deltaTime) override;

    /// 描画処理
    /// - 基本は GameObject::Draw()（Component 描画）に委譲する
    void Draw() override;

    /// 終了処理
    /// NOTE: GameObject 側で Component は unique_ptr 破棄される。
    ///       本クラスは非所有参照を nullptr へ戻すのみ。
    void Uninit() override;

    // ----------------------------------------------------------------------
    // 衝突イベント
    // ----------------------------------------------------------------------
    /// 衝突開始時の処理
    /// - ボールに対して反発（キック）を与える
    /// - 追加でスコア加算/サウンド/エフェクト等を起こす場合はここに集約する
    void OnCollisionEnter(const CollisionInfo& info) override;

private:
    // ----------------------------------------------------------------------
    // バンパー設定（調整パラメータ）
    // ----------------------------------------------------------------------
    static constexpr float kBumperDefaultSize           = 1.6f;  // バンパーのデフォルトサイズ（見た目/判定の基準）
    static constexpr float kBumperDefaultColliderRadius = 1.6f;  // SphereCollider の半径

    // 反発（キック）速度
    // - 接触法線方向やゲーム設計に応じて適用方法は cpp 側で決定する
    static constexpr float kBumperKickHorizontalSpeed   = 25.0f; // 水平方向のキック速度
    static constexpr float kBumperKickVerticalSpeed     = 15.0f; // 垂直方向のキック速度

    // ----------------------------------------------------------------------
    // コンポーネント参照（非所有）
    // ----------------------------------------------------------------------
    /// 非所有：ModelRenderer（所有は GameObject::m_Components）
    /// - 有効期間：当該 Component が保持されている間のみ有効
    ModelRenderer* m_ModelRenderer = nullptr;

    // ----------------------------------------------------------------------
    // 状態
    // ----------------------------------------------------------------------
    float m_ShockCooldownTimer = 0.0f; // 衝撃波発生のクールダウンタイマー（秒）
};

