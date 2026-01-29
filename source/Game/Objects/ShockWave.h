//------------------------------------------------------------------------------
// ShockWave
//------------------------------------------------------------------------------
// 役割:
// バンパー衝突時などに生成される衝撃波オブジェクト。
// Trigger 判定用 SphereCollider を時間で拡大し、接触した敵を倒す。
//
// 設計意図:
// - 範囲攻撃を「短寿命オブジェクト」として独立させ、呼び出し側の責務を減らす
// - 物理反発ではなく Trigger による接触通知で処理する
//
// 構成:
// - ColliderGroup / SphereCollider : 当たり判定（Trigger）
// - ライフサイクル               : Init / Update / Draw / Uninit
// - 衝突イベント                 : OnTriggerEnter
//
// NOTE:
// - ColliderGroup / SphereCollider への参照は非所有（GameObject が所有）
//------------------------------------------------------------------------------
#pragma once

#include "GameObject.h"

class ColliderGroup;
class SphereCollider;
class MeshRenderer;

/// 衝撃波オブジェクト
/// - 一定時間だけ半径を拡大する Trigger 判定を発生させる
/// - 接触した EnemyBase を Destroy() し、撃破扱いにする
class ShockWave : public GameObject
{
public:
    // ----------------------------------------------------------------------
    // ライフサイクル
    // ----------------------------------------------------------------------
    /// 初期化
    /// - ColliderGroup / SphereCollider を構築し、Trigger 判定を有効化する
    void Init() override;

    /// 更新
    /// - 経過時間に応じて半径を拡大し、寿命で Destroy() する
    void Update(float deltaTime) override;

    /// 描画
    /// - 現状は GameObject::Draw() のみ（必要なら Renderer を追加する）
    void Draw() override;

    /// 終了
    /// - 非所有参照をクリアする（安全のため）
    void Uninit() override;

    // ----------------------------------------------------------------------
    // 衝突イベント
    // ----------------------------------------------------------------------
    /// トリガー侵入
    /// - EnemyBase に接触した場合、敵を Destroy() し撃破扱いにする
    void OnTriggerEnter(const CollisionInfo& info) override;

private:
    // ----------------------------------------------------------------------
    // 調整パラメータ
    // ----------------------------------------------------------------------
    inline static const Vector3 kShockWaveDefaultScale      = { 0.75f, 0.75f, 0.75f }; // 衝撃波の基本スケール
    static constexpr float kShockWaveStartRadius            = 4.5f;  // 開始半径
    static constexpr float kShockWaveEndRadius              = 7.2f;  // 終了半径
    static constexpr float kShockWaveDuration               = 0.35f; // 寿命（秒）
    static constexpr const wchar_t* kShockWaveTexturePath   = L"asset\\texture\\BumperShockWave.png"; 
    static constexpr const char* kShockWaveVertexShaderPath = "shader\\bin\\BaseLitVS.cso";
    static constexpr const char* kShockWavePixelShaderPath  = "shader\\bin\\BaseLitPS.cso";

    // ----------------------------------------------------------------------
    // 状態
    // ----------------------------------------------------------------------
    float m_Elapsed = 0.0f; // 経過時間（秒）

    // ----------------------------------------------------------------------
    // コンポーネント参照（非所有）
    // ----------------------------------------------------------------------
    ColliderGroup*  m_ColliderGroup  = nullptr; // 非所有：ColliderGroup
    SphereCollider* m_SphereCollider = nullptr; // 非所有：SphereCollider
    MeshRenderer*   m_MeshRenderer   = nullptr; // 非所有：MeshRenderer
};
