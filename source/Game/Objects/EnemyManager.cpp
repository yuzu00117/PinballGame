#include "EnemyManager.h"

#include "EnemyBase.h"
#include <algorithm>

EnemyManager* EnemyManager::s_Instance = nullptr;

// ------------------------------------------------------------------------------
// シングルトン取得
// ------------------------------------------------------------------------------
// - s_Instance を返す
// - 存在しない場合は nullptr
EnemyManager* EnemyManager::Get()
{
    return s_Instance;
}

// ------------------------------------------------------------------------------
// 敵一覧取得
// ------------------------------------------------------------------------------
// - EnemyManager が存在する場合：その m_Enemies を返す
// - 存在しない場合：空の静的 vector を返す（参照は常に有効）
const std::vector<EnemyBase*>& EnemyManager::GetEnemies()
{
    static std::vector<EnemyBase*> s_Empty;
    return s_Instance ? s_Instance->m_Enemies : s_Empty;
}

// ------------------------------------------------------------------------------
// 敵登録
// ------------------------------------------------------------------------------
// - EnemyManager が存在しない / enemy が nullptr の場合は何もしない
// - 重複登録を防ぐ（std::find で存在確認）
// NOTE: 所有はしない。EnemyBase の破棄は別経路で行うこと。
void EnemyManager::RegisterEnemy(EnemyBase* enemy)
{
    if (s_Instance == nullptr || enemy == nullptr)
    {
        return;
    }

    auto& enemies = s_Instance->m_Enemies;
    const auto it = std::find(enemies.begin(), enemies.end(), enemy);
    if (it == enemies.end())
    {
        enemies.push_back(enemy);
    }
}

// ------------------------------------------------------------------------------
// 初期化
// ------------------------------------------------------------------------------
// - GameObject::Init() を呼ぶ
// - 管理リストを初期化
// - シングルトン参照を this に設定
// NOTE: 既に s_Instance が別個体を指していても上書きする設計（多重生成に注意）
void EnemyManager::Init()
{
    GameObject::Init();
    m_Enemies.clear();
    s_Instance = this;
}

// ------------------------------------------------------------------------------
// 更新
// ------------------------------------------------------------------------------
// - GameObject::Update() を呼ぶ（子/Component更新）
// - Update 後に、死亡/無効参照を掃除する
void EnemyManager::Update(float deltaTime)
{
    GameObject::Update(deltaTime);
    CleanupDeadEnemies();
}

// ------------------------------------------------------------------------------
// 終了
// ------------------------------------------------------------------------------
// - 管理リストをクリア（参照解除）
// - 自分が s_Instance の場合のみ nullptr に戻す
// - GameObject::Uninit() を呼ぶ
void EnemyManager::Uninit()
{
    m_Enemies.clear();
    if (s_Instance == this)
    {
        s_Instance = nullptr;
    }
    GameObject::Uninit();
}

// ------------------------------------------------------------------------------
// 死亡/無効参照の除去
// ------------------------------------------------------------------------------
// - nullptr または enemy->IsDead() を満たす要素を erase-remove で除去する
// NOTE: 破棄はしない（非所有のため）
void EnemyManager::CleanupDeadEnemies()
{
    m_Enemies.erase(
        std::remove_if(
            m_Enemies.begin(),
            m_Enemies.end(),
            [](EnemyBase* enemy) {
                return enemy == nullptr || enemy->IsDead();
            }),
        m_Enemies.end());
}
