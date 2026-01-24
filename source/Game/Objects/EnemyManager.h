#pragma once

#include <vector>
#include "GameObject.h"

class EnemyBase;

/// <summary>
/// エネミーの参照を一元管理するマネージャー
/// </summary>
class EnemyManager : public GameObject
{
public:
    /// <summary>
    /// シングルトン取得（存在しない場合は nullptr）
    /// </summary>
    static EnemyManager* Get();

    /// <summary>
    /// 現在管理中のエネミー一覧を取得する
    /// </summary>
    static const std::vector<EnemyBase*>& GetEnemies();

    /// <summary>
    /// エネミーを登録する
    /// </summary>
    static void RegisterEnemy(EnemyBase* enemy);

    void Init() override;
    void Update(float deltaTime) override;
    void Uninit() override;

private:
    static EnemyManager* s_Instance;
    std::vector<EnemyBase*> m_Enemies;

    void CleanupDeadEnemies();
};
