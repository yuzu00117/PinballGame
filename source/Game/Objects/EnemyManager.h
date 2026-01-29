//------------------------------------------------------------------------------
// EnemyManager
//------------------------------------------------------------------------------
// 役割:
// EnemyBase の参照（非所有）を一元管理するマネージャー。
// ゲーム内の敵一覧へアクセスするための窓口を提供する。
//
// 設計意図:
// - 他システム（UI / ターゲット選択 / デバッグ表示など）から敵一覧へアクセスしやすくする。
// - EnemyManager インスタンスをシングルトン参照として扱い、静的関数から取得できるようにする。
//
// 構成:
// - シングルトン参照      : s_Instance（非所有）
// - 敵リスト管理（非所有） : m_Enemies（EnemyBase*）
// - 死亡/無効参照の掃除     : CleanupDeadEnemies()
//
// NOTE:
// - m_Enemies は EnemyBase を所有しない（破棄は各 Enemy 側 / 所有者が行う）。
// - GetEnemies() が返す参照は、EnemyManager が生存している間のみ有効。
// - 登録解除は明示的に行わず、Update 内で IsDead / nullptr を除去する方式。
//   （死亡判定のタイミングが要件に合うか確認すること）
//------------------------------------------------------------------------------
#pragma once

#include <vector>
#include "GameObject.h"

class EnemyBase;

/// エネミー参照（非所有）を一元管理するマネージャー
/// - シングルトン参照としてアクセスする
/// - EnemyBase* のみを保持し、所有・破棄は行わない
class EnemyManager : public GameObject
{
public:
    /// シングルトン取得
    /// - 戻り値：存在しない場合は nullptr
    /// - 有効期間：EnemyManager が Init() されてから Uninit() されるまで
    static EnemyManager* Get();

    /// 現在管理中のエネミー一覧を取得する
    /// - 戻り値：EnemyManager が存在しない場合は空の静的配列を返す
    /// - 所有権：返る参照/要素ポインタは非所有
    /// - 有効期間：
    ///   - EnemyManager が存在する場合：EnemyManager の m_Enemies が有効な間のみ
    ///   - EnemyManager が存在しない場合：空配列（静的）の参照は常に有効
    /// NOTE: 返る参照を保持し続けるのではなく、必要な都度取得すること
    static const std::vector<EnemyBase*>& GetEnemies();

    /// エネミーを登録する（重複登録はしない）
    /// - 引数 enemy：非所有（nullptr の場合は無視）
    /// - 副作用：EnemyManager が存在する場合、m_Enemies に追加され得る
    /// - 失敗条件：EnemyManager が存在しない場合は何もしない
    static void RegisterEnemy(EnemyBase* enemy);

    // ----------------------------------------------------------------------
    // ライフサイクル
    // ----------------------------------------------------------------------
    /// 初期化
    /// - GameObject::Init() を呼び出す
    /// - 管理リストを初期化し、s_Instance を this に設定する
    /// NOTE: 既存の s_Instance を置き換える設計になっている（多重生成に注意）
    void Init() override;

    /// 更新
    /// - GameObject::Update() 後、死亡/無効参照を除去する
    void Update(float deltaTime) override;

    /// 終了
    /// - 管理リストを破棄（参照解除）
    /// - 自分が s_Instance の場合、nullptr に戻す
    void Uninit() override;

private:
    // ----------------------------------------------------------------------
    // 内部状態
    // ----------------------------------------------------------------------
    static EnemyManager* s_Instance;      // 非所有：シングルトン参照
    std::vector<EnemyBase*> m_Enemies;    // 非所有：敵参照リスト

    // ----------------------------------------------------------------------
    // 内部処理
    // ----------------------------------------------------------------------
    /// 死亡/無効参照を除去する
    /// - nullptr または enemy->IsDead() の要素を erase-remove で取り除く
    /// NOTE: ここでは EnemyBase の Uninit/破棄は行わない（所有していないため）
    void CleanupDeadEnemies();
};
