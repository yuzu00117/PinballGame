#include "EnemySpawner.h"
#include "Hole.h"
#include "EnemyBase.h"
#include "EnemyStraight.h"
#include <cstdlib>          // rand

// 初期化処理
void EnemySpawner::Init()
{
    // 親クラスの初期化呼び出し
    GameObject::Init();
    
    // 最初のスポーンまでのタイマー
    m_SpawnTimer = m_SpawnInterval;
}

// 更新処理
void EnemySpawner::Update()
{
    // 親クラスの更新処理呼び出し
    GameObject::Update();

    if (m_TargetHoles.empty()) {
        // ターゲットホールが登録されていない場合はスポーンしない
        return;
    }

    // 1フレームごとにスポーンタイマーをカウントダウン
    if (--m_SpawnTimer <= 0) {
        // タイマーが0以下になったらエネミーをスポーン
        SpawnEnemy();
        // タイマーをリセット
        m_SpawnTimer = m_SpawnInterval;
    }
}

// エネミーをスポーン
void EnemySpawner::SpawnEnemy()
{
    // ターゲットホールがない場合はスポーンしない
    if (m_TargetHoles.empty()) {
        return;
    }

    // スポーン位置（Xをランダム、Zは固定）
    const float x = GetRandomFloat(m_SpawnXMin, m_SpawnXMax);
    const float y = 0.0f; // 地面の高さに合わせる
    const float z = m_SpawnZ;
    const Vector3 spawnPos = { x, y, z };

    // ランダムにターゲットホールを選択
    const int holeIndex = rand() % static_cast<int>(m_TargetHoles.size());
    Hole* targetHole = m_TargetHoles[holeIndex];

    // 敵を生成
    EnemyStraight* enemy = nullptr;
    enemy = m_Parent->CreateChild<EnemyStraight>();
    enemy->m_Transform.Position = spawnPos;

    // ターゲットホールの位置をエネミーに設定
    enemy->SetTargetPosition(targetHole->GetHolePosition());
}

// [min, max]の範囲でランダムなfloat値を取得
float EnemySpawner::GetRandomFloat(float min, float max)
{
    const float t = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    return min + (max - min) * t;
}