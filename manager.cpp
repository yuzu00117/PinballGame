// manager.cpp
#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "audio.h"
#include <windows.h>
#include "scene.h"

Manager::Scene Manager::m_CurrentScene = Manager::Scene::Title; // 初期シーンはタイトル
std::vector<GameObject*> Manager::m_SceneGameObjects;

void Manager::Init() {
    Renderer::Init();
    Audio::InitMaster(); // オーディオシステムの初期化

	// 初期シーンのゲームオブジェクトを生成
    m_SceneGameObjects = CreateSceneObjects(m_CurrentScene);
    for (auto obj : m_SceneGameObjects) obj->Init();
}

void Manager::Uninit() {

	// シーンのゲームオブジェクトを解放
    for (auto obj : m_SceneGameObjects) {
        obj->Uninit();
        delete obj;
    }
    m_SceneGameObjects.clear();

	Renderer::Uninit(); // レンダラーの解放
    Audio::UninitMaster(); // オーディオシステムの終了
}

void Manager::Update() {
    for (auto obj : m_SceneGameObjects) {
        obj->Update();
    }
        
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

void Manager::Draw() {

	Renderer::Begin(); // レンダリング開始

	// シーンのゲームオブジェクトを描画
    for (auto obj : m_SceneGameObjects) {
        obj->Draw();
    }

	Renderer::End(); // レンダリング終了
}

void Manager::ChangeScene(Scene newScene) {
    // 旧シーン解放
    for (auto obj : m_SceneGameObjects) {
        obj->Uninit();
        delete obj;
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
