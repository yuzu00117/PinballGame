//------------------------------------------------------------------------------
// EnemyBase.h
//------------------------------------------------------------------------------
// 役割:
// エネミーの共通基底クラス。
// 体力管理、移動、被ダメージ処理、撃破時の共通処理などを提供する。
// 全種のエネミーはこのクラスを継承して作成される。
//
// 設計意図:
// - エネミーの共通ロジックを一元化し、派生クラスへの実装負荷を下げる。
// - 外部（BallやShockWave）からのダメージ処理をカプセル化し、二重呼び出しを防ぐ。
//
// 構成:
// - HP / Score        : 体力と撃破後のスコア
// - TargetPos         : 目標位置（移動先）
// - ApplyDamage / Kill: ダメージ適用と即時撃破
// - OnKilled          : 撃破時の共通演出・破棄処理
//------------------------------------------------------------------------------
#pragma once

#include "GameObject.h"
#include "Vector3.h"

class ColliderGroup;
class AnimationModel;
class MeshRenderer;
class RigidBody;

/// エネミー共通の基底クラス
/// - 全エネミーはこのクラスを継承して作成される
class EnemyBase : public GameObject
{
public:
    // ----------------------------------------------------------------------
    // 関数定義
    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    // ライフサイクル
    // ----------------------------------------------------------------------
    /// 初期化処理
    /// - 必要なコンポーネントを追加し、EnemyManager へ登録する
    void Init() override;

    /// 更新処理
    /// - ターゲット方向への移動、アニメーション更新、死亡判定を行う
    void Update(float deltaTime) override;

    /// 描画処理
    /// - 親クラスの GameOject::Draw() に委譲する
    void Draw() override;

    /// 終了処理
    /// - コンポーネントのポインタをクリアする
    void Uninit() override;

    /// 衝突コールバック
    /// - ボールとの衝突判定（ダメージ適用）やホールへの落下を処理する
    void OnTriggerEnter(const CollisionInfo& info) override;

    /// ターゲット位置を設定する
    /// - targetPos: ワールド座標
    void SetTargetPosition(const Vector3& targetPos) { m_TargetPos = targetPos; }

    /// ダメージを与える
    /// - HP を減少させ、0以下になれば OnKilled() を呼び出す
    void ApplyDamage(int damage);

    /// 即座に撃破する
    /// - ShockWave 等の即死ギミックから呼び出され、HP をゼロにして OnKilled() を呼ぶ
    void Kill(); // 引数なしで即死させる

protected:
    /// 死亡時の共通処理（演出やスコア加算・破棄予約など）
    void OnKilled();

private:
    // ----------------------------------------------------------------------
    // ヘルパー
    // ----------------------------------------------------------------------
    /// ターゲットへの正規化方向を計算する
    /// - 戻り値: 正規化された方向ベクトル（目標が同じ場所ならゼロベクトル）
    Vector3 GetDirToTarget() const;

    // ----------------------------------------------------------------------
    // 定数定義
    // ----------------------------------------------------------------------
    // エネミーのデフォルトパラメーター
    static constexpr float kDefaultEnemyScale = 0.01f; // スケール

    // シェーダーパス
    static constexpr const char* VertexShaderPath =   // 頂点シェーダのパス
        "shader\\bin\\BaseLitVS.cso";   
    static constexpr const char* PixelShaderPath  =   // ピクセルシェーダのパス
        "shader\\bin\\BaseLitPS.cso";   
    
    // ----------------------------------------------------------------------
    // 変数定義
    // ----------------------------------------------------------------------
    // 共通パラメーター
    float   m_Speed     = 3.0f;                       // 移動速度
    int     m_HP        = 1;                          // 体力
    int     m_Score     = 100;                        // スコア値
    Vector3 m_TargetPos = { 0.0f, 0.0f, 0.0f };       // 目標位置
    Vector3 m_Velocity  = { 0.0f, 0.0f, 0.0f };       // 現在速度

    // アニメーション用
    // TODO: アニメーション関連
    int m_AnimFrame = 0;                              // アニメーションフレームカウンタ

    // コンポーネント（非所有）
    // NOTE: Component の実体所有権は GameObject が unique_ptr で保持している
    ColliderGroup*     m_ColliderGroup  = nullptr;    // 非所有：コライダーグループ
    // AnimationModel* m_AnimationModel = nullptr;    // 非所有：アニメーションモデル
    MeshRenderer*      m_MeshRenderer   = nullptr;    // 非所有：メッシュ描画
    RigidBody*         m_RigidBody      = nullptr;    // 非所有：物理演算
};
