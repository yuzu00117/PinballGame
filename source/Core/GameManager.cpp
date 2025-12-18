#include "main.h"
#include "GameManager.h"
#include "Renderer.h"
#include "Scene.h"
#include "DebugSettings.h"
#include "Collider.h"
#include "ColliderGroup.h"
#include "RigidBody.h"

// システム関連
#include "Audio.h"
#include "Input.h"
#include "HP.h"
#include <windows.h>
#include <unordered_set>

// 静的メンバ変数の定義
GameManager::Scene GameManager::m_CurrentScene = GameManager::Scene::Title;  // 初期シーンはタイトル
std::vector<GameObject*> GameManager::m_SceneGameObjects;                  // 現在のシーンのGameObjectリスト
std::set<GameManager::ColliderPair> GameManager::m_PreviousPairs;          // 前フレームの衝突ペア情報
std::set<GameManager::ColliderPair> GameManager::m_PreviousTriggerPairs;   // 前フレームのトリガーペア情報

// デバッグ用コライダー描画フラグ
bool g_EnableColliderDebugDraw = false; // デフォルトは無効

// ----------------------------------------------------------------------
// 初期化処理
// ----------------------------------------------------------------------
void GameManager::Init() 
{
    // レンダラー初期化
    Renderer::Init();

    // オーディオシステム初期化
    Audio::InitMaster();

    // 入力システム初期化
    Input::Init();

	// 現在のシーンのゲームオブジェクトを生成
    m_SceneGameObjects = CreateSceneObjects(m_CurrentScene);
    // 生成したGameObjectのInitを呼び出して初期化
    for (GameObject* gameObject : m_SceneGameObjects) gameObject->Init();
}

// ----------------------------------------------------------------------
// 終了処理
// ----------------------------------------------------------------------
void GameManager::Uninit() {

	// 現在のシーンのゲームオブジェクトを解放
    for (GameObject* gameObject : m_SceneGameObjects) {
        gameObject->Uninit();
        delete gameObject;
    }
    m_SceneGameObjects.clear();

    // レンダラー終了処理
	Renderer::Uninit();

    // オーディオシステム終了処理
    Audio::UninitMaster();
}

// ----------------------------------------------------------------------
// 更新処理
// ----------------------------------------------------------------------
void GameManager::Update(float deltaTime)
{
    // 入力状態の更新
    Input::Update();

    // 各シーンのゲームオブジェクトを更新
    for (GameObject* gameObject : m_SceneGameObjects) {
        gameObject->Update(deltaTime);
    }

    // コライダー同士の当たり判定処理
    CheckCollisions();

    // --- ゲームオーバー判定 ---
    // TODO: 現在の仕様だと、HPが0以下になった瞬間にシーンが切り替わってしまい、
    //       何が起こったのか分かりづらいので、ゲームオーバー演出を追加するなどの改善が必要。
    if (m_CurrentScene == Scene::Game)
    {
        // 連続でゲームオーバー処理が走らないためにフラグを用意
        static bool isGameOver = false;

        // 死亡判定がtrueの場合、ゲームオーバー処理を実行
        if (!isGameOver && HP::IsDead())
        {
            isGameOver = true;
            ChangeScene(Scene::Result);
        }
    }

#if defined(_DEBUG)
    // デバッグ用コライダー描画フラグ切り替え
    static bool prevDebugDraw = false;
    bool currDebugDraw = Input::GetKeyPress(VK_F1); // F1キーで切り替え
    if (currDebugDraw && !prevDebugDraw) {
        g_EnableColliderDebugDraw = !g_EnableColliderDebugDraw;
    }
    prevDebugDraw = currDebugDraw;

    // デバッグ用シーン切り替え（Enterキー）
    // Enter キー（VK_RETURN）が押されていたら、現在のシーンに応じて次のシーンへ
	// 押下チェック
	static bool prevEnter = false;
	SHORT ks = GetAsyncKeyState(VK_RETURN);
	bool currEnter = (ks & 0x8000) != 0; // 今回のフレームで押されているかどうか
    if (currEnter && !prevEnter) { // 押された瞬間を検出
        switch (m_CurrentScene) {
        case Scene::Title:
            ChangeScene(Scene::Game);
            break;
        case Scene::Game:
            ChangeScene(Scene::Result);
            break;
        case Scene::Result:
            ChangeScene(Scene::Title);
            break;
        }
    }
	prevEnter = currEnter; // 前回の状態を更新
#endif // NDEBUG
}

// ----------------------------------------------------------------------
// 描画処理
// ----------------------------------------------------------------------
void GameManager::Draw()
{
	Renderer::Begin(); // レンダリング開始

	// シーンのゲームオブジェクトを描画
    for (GameObject* gameObject : m_SceneGameObjects) {
        gameObject->Draw();
    }

	Renderer::End(); // レンダリング終了
}

// ----------------------------------------------------------------------
// シーン変更処理
// ----------------------------------------------------------------------
void GameManager::ChangeScene(Scene newScene)
{
    // 旧シーン解放
    for (GameObject* gameObject : m_SceneGameObjects) {
        gameObject->Uninit();
        delete gameObject;
    }
    m_SceneGameObjects.clear();

	// 現在のシーンを更新
    m_CurrentScene = newScene;

	// 新シーン初期化
	m_SceneGameObjects = CreateSceneObjects(newScene);

    for (auto obj : m_SceneGameObjects) {
        obj->Init();
    }
}

// ----------------------------------------------------------------------
// コライダー同士の当たり判定処理
// ----------------------------------------------------------------------
void GameManager::CheckCollisions()
{
    std::vector<Collider*> colliders;
    colliders.reserve(m_SceneGameObjects.size());

    // シーン直下のGameObjectから、子も含めて全てのコライダーを収集
    for (GameObject* gameObject : m_SceneGameObjects)
    {
        if (!gameObject) continue;
        gameObject->CollectCollidersRecursive(colliders);
    }

    const size_t n = colliders.size();
    
    // 今フレームの衝突ペア情報を格納するセット
    std::set<ColliderPair> currentCollisionPairs; // 衝突ペア情報
    std::set<ColliderPair> currentTriggerPairs;   // トリガーペア情報

    // ★ 今フレームに存在するコライダー集合を作成
    std::unordered_set<Collider*> activeColliders;
    activeColliders.reserve(n * 2);
    for (auto* c : colliders)
    {
        if (c) activeColliders.insert(c);
    }

    // --- 衝突判定（Enter / Stay）---
    for (size_t i = 0; i < n; ++i)
    {
        Collider* colliderA = colliders[i];
        if (!colliderA) continue;

        for (size_t j = i + 1; j < n; ++j)
        {
            Collider* colliderB = colliders[j];
            if (!colliderB) continue;

            CollisionInfo infoA;
            CollisionInfo infoB;

            if (!colliderA->CheckCollision(colliderB, infoA, infoB))
                continue;

            const bool isTriggerPair = (colliderA->m_IsTrigger || colliderB->m_IsTrigger);

            ColliderPair pair = MakePair(colliderA, colliderB);

            if (isTriggerPair)
            {
                currentTriggerPairs.insert(pair);
                const bool wasTriggering =
                    (m_PreviousTriggerPairs.find(pair) != m_PreviousTriggerPairs.end());

                if (wasTriggering)
                {
                    colliderA->InvokeOnTriggerStay(infoA);
                    colliderB->InvokeOnTriggerStay(infoB);
                }
                else
                {
                    colliderA->InvokeOnTriggerEnter(infoA);
                    colliderB->InvokeOnTriggerEnter(infoB);
                }
            }
            else
            {
                currentCollisionPairs.insert(pair);
                const bool wasColliding =
                    (m_PreviousPairs.find(pair) != m_PreviousPairs.end());

                // 既存：RigidBody解決（Triggerではやらない） :contentReference[oaicite:7]{index=7}
                if (auto* ownerA = colliderA->m_Owner)
                    if (auto* rbA = ownerA->GetComponent<RigidBody>())
                        rbA->ResolveCollision(infoA);

                if (auto* ownerB = colliderB->m_Owner)
                    if (auto* rbB = ownerB->GetComponent<RigidBody>())
                        rbB->ResolveCollision(infoB);

                if (wasColliding)
                {
                    colliderA->InvokeOnCollisionStay(infoA);
                    colliderB->InvokeOnCollisionStay(infoB);
                }
                else
                {
                    colliderA->InvokeOnCollisionEnter(infoA);
                    colliderB->InvokeOnCollisionEnter(infoB);
                }
            }
        }
    }

    // --- Collision Exit ---
    for (const auto& pair : m_PreviousPairs)
    {
        Collider* a = pair.first;
        Collider* b = pair.second;

        if (activeColliders.find(a) == activeColliders.end() ||
            activeColliders.find(b) == activeColliders.end())
            continue;

        if (currentCollisionPairs.find(pair) == currentCollisionPairs.end())
        {
            CollisionInfo infoA{};
            infoA.self  = a; infoA.other = b;

            CollisionInfo infoB{};
            infoB.self  = b; infoB.other = a;

            a->InvokeOnCollisionExit(infoA);
            b->InvokeOnCollisionExit(infoB);
        }
    }

    // --- Trigger Exit ---
    for (const auto& pair : m_PreviousTriggerPairs)
    {
        Collider* a = pair.first;
        Collider* b = pair.second;

        if (activeColliders.find(a) == activeColliders.end() ||
            activeColliders.find(b) == activeColliders.end())
            continue;

        if (currentTriggerPairs.find(pair) == currentTriggerPairs.end())
        {
            CollisionInfo infoA{};
            infoA.self  = a; infoA.other = b;

            CollisionInfo infoB{};
            infoB.self  = b; infoB.other = a;

            a->InvokeOnTriggerExit(infoA);
            b->InvokeOnTriggerExit(infoB);
        }
    }

    // 次フレーム用に保存
    m_PreviousPairs.swap(currentCollisionPairs);
    m_PreviousTriggerPairs.swap(currentTriggerPairs);
}