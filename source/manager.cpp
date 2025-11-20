#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "scene.h"
#include "DebugSettings.h"
#include "Collider.h"
#include "ColliderGroup.h"

// システム関連
#include "audio.h"
#include "input.h"
#include <windows.h>

// 静的メンバ変数の定義
Manager::Scene Manager::m_CurrentScene = Manager::Scene::Title; // 初期シーンはタイトル
std::vector<GameObject*> Manager::m_SceneGameObjects;           // 現在のシーンのGameObjectリスト
std::set<Manager::ColliderPair> Manager::m_PreviousPairs;       // 前フレームの衝突ペア情報

// デバッグ用コライダー描画フラグ
bool g_EnableColliderDebugDraw = false; // デフォルトは無効

// ----------------------------------------------------------------------
// 初期化処理
// ----------------------------------------------------------------------
void Manager::Init() 
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
void Manager::Uninit() {

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
void Manager::Update()
{
    // 入力状態の更新
    Input::Update();

    // 各シーンのゲームオブジェクトを更新
    for (GameObject* gameObject : m_SceneGameObjects) {
        gameObject->Update();
    }

    // コライダー同士の当たり判定処理
    CheckCollisions();

    // デバッグ用コライダー描画
    static bool prevDebugDraw = false;
    bool currDebugDraw = Input::GetKeyPress(VK_F1); // F1キーで切り替え
    if (currDebugDraw && !prevDebugDraw) {
        g_EnableColliderDebugDraw = !g_EnableColliderDebugDraw;
    }
    prevDebugDraw = currDebugDraw;
        
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
}

// ----------------------------------------------------------------------
// 描画処理
// ----------------------------------------------------------------------
void Manager::Draw()
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
void Manager::ChangeScene(Scene newScene)
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
void Manager::CheckCollisions()
{
    std::vector<Collider*> colliders;
    colliders.reserve(m_SceneGameObjects.size());

    // シーン内の全コライダーを収集
    for (GameObject* gameObject : m_SceneGameObjects)
    {
        if (!gameObject) continue; // nullptrの場合はスキップ

        // もしColliderGroupを使っている場合は、まずそれを優先
        if (auto* colliderGroup = gameObject->GetComponent<ColliderGroup>())
        {
            colliders.push_back(colliderGroup);
            continue;
        }

        // 単体のColliderを持つ場合
        if (auto* collider = gameObject->GetComponent<Collider>())
        {
            colliders.push_back(collider);
        }
    }

    const size_t n = colliders.size();
    std::set<ColliderPair> currentPairs; // 今フレームの衝突ペア情報

    // ペアごとの当たり判定処理
    for (size_t i = 0; i < n; ++i)
    {
        Collider* colliderA = colliders[i];
        if (!colliderA) continue;

        for (size_t j = i + 1; j < n; ++j)
        {
            Collider* colliderB = colliders[j];
            if (!colliderB) continue;

            // 当たり判定を行う
            CollisionInfo infoA;
            CollisionInfo infoB;
            if (colliderA->CheckCollision(colliderB, infoA, infoB))
            {
                // 衝突ペアを記録
                ColliderPair pair = MakePair(colliderA, colliderB);
                currentPairs.insert(pair);

                // 前フレームに衝突していたか確認
                bool wasColliding = (m_PreviousPairs.find(pair) != m_PreviousPairs.end());

                if (wasColliding)
                {
                    // 衝突継続イベント
                    colliderA->InvokeOnCollisionStay(infoA);
                    colliderB->InvokeOnCollisionStay(infoB);
                }
                else
                {
                    // 衝突開始イベント
                    colliderA->InvokeOnCollisionEnter(infoA);
                    colliderB->InvokeOnCollisionEnter(infoB);
                }
            }
        }
    }

    // Exit判定
    for (const auto& pair : m_PreviousPairs)
    {
        if (currentPairs.find(pair) == currentPairs.end())
        {
            Collider* a = pair.a;
            Collider* b = pair.b;
            if (!a || !b) continue;

            // 衝突終了イベント
            CollisionInfo infoA{};
            infoA.self = a;
            infoA.other = b;

            CollisionInfo infoB{};
            infoB.self = b;
            infoB.other = a;

            a->InvokeOnCollisionExit(infoA);
            b->InvokeOnCollisionExit(infoB);
        }
    }
    m_PreviousPairs.swap(currentPairs); // 今フレームの情報を保存 for 次フレーム
}