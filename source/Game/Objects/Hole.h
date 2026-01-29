//------------------------------------------------------------------------------
// Hole
//------------------------------------------------------------------------------
// 役割:
// ボール（および将来的に敵）が落ちる「ホール（落とし穴）」を表す GameObject。
// 衝突イベントを受け取り、対象に応じた処理（例：ボールのリセット）を行う。
//
// 設計意図:
// 「落下判定」は衝突（OnCollisionEnter）で検知し、
// 具体的な反応は対象型（Ball / Enemy など）で分岐して実装する。
// 描画は MeshRenderer、当たり判定は ColliderGroup に分離して構成する。
//
// 構成:
// - MeshRenderer            : 表示用（ユニットボックス、BaseLitシェーダ）
// - ColliderGroup + BoxCollider : 当たり判定用
// - 衝突イベント受信        : OnCollisionEnter
//
// NOTE:
// - m_MeshRenderer / m_ColliderGroup は非所有参照（所有は GameObject 側：unique_ptr）
// - Uninit() では参照を nullptr に戻すのみ（コンポーネント実体の破棄は所有側に依存）
//------------------------------------------------------------------------------
#pragma once

#include "gameobject.h"
#include "vector3.h"

class MeshRenderer;
class ColliderGroup;

/// ホール（落とし穴）を表すゲームオブジェクト
/// - メッシュ表示と当たり判定をコンポーネントで構成する
/// - 衝突開始時に対象に応じた処理を実行する（例：Ball のリセット）
class Hole : public GameObject
{
public:
    // ----------------------------------------------------------------------
    // ライフサイクルメソッド
    // ----------------------------------------------------------------------
    /// 初期化処理
    /// - MeshRenderer / ColliderGroup を生成して初期状態を設定する
    /// - 副作用：AddComponent 直後に各 Component::Init() が呼ばれる（GameObject規約）
    void Init() override;

    /// 終了処理
    /// - 保持している非所有参照を破棄（nullptr 化）する
    void Uninit() override;

    /// 更新処理（deltaTime は秒単位）
    /// - 現状は親クラスの Update に委譲
    void Update(float deltaTime) override;

    /// 描画処理
    /// - 現状は親クラスの Draw に委譲
    void Draw() override;

    // ----------------------------------------------------------------------
    // 衝突イベント
    // ----------------------------------------------------------------------
    /// 衝突開始時のコールバック
    /// - Ball が入った場合：ResetBall() を呼ぶ
    /// - Enemy は未実装（TODO）
    void OnCollisionEnter(const CollisionInfo& info) override;

    // ----------------------------------------------------------------------
    // 取得
    // ----------------------------------------------------------------------
    /// ホールの中心位置を返す
    Vector3 GetHolePosition() const { return m_Transform.Position; }

private:
    // ----------------------------------------------------------------------
    // リソース設定
    // ----------------------------------------------------------------------
    static constexpr const char* VertexShaderPath = "shader\\bin\\BaseLitVS.cso";
    static constexpr const char* PixelShaderPath  = "shader\\bin\\BaseLitPS.cso";

    // ----------------------------------------------------------------------
    // 参照（非所有）
    // ----------------------------------------------------------------------
    MeshRenderer*  m_MeshRenderer  = nullptr; // 非所有：表示用メッシュ
    ColliderGroup* m_ColliderGroup = nullptr; // 非所有：当たり判定用コライダー群
};