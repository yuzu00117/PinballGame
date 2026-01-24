#include "EnemyManager.h"
#include "EnemyBase.h"

#include <algorithm>

EnemyManager* EnemyManager::s_Instance = nullptr;

EnemyManager* EnemyManager::Get()
{
    return s_Instance;
}

const std::vector<EnemyBase*>& EnemyManager::GetEnemies()
{
    static std::vector<EnemyBase*> s_Empty;
    return s_Instance ? s_Instance->m_Enemies : s_Empty;
}

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

void EnemyManager::Init()
{
    GameObject::Init();
    m_Enemies.clear();
    s_Instance = this;
}

void EnemyManager::Update(float deltaTime)
{
    GameObject::Update(deltaTime);
    CleanupDeadEnemies();
}

void EnemyManager::Uninit()
{
    m_Enemies.clear();
    if (s_Instance == this)
    {
        s_Instance = nullptr;
    }
    GameObject::Uninit();
}

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
