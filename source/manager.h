// manager.h
#pragma once
#include <vector>
#include "gameObject.h"
#include "scene.h"

class Manager {
public:
    // ----------------------------------------------------------------------
    // 関数定義
    // ----------------------------------------------------------------------
    /// <summary>
    /// ライフサイクルメソッド
    /// </summary>
    static void Init();
    static void Uninit();
    static void Update();
    static void Draw();

    // WHY: usingって変数？
	using Scene = ::Scene;

    /// <summary>
    /// シーン変更
    /// </summary>
    static void ChangeScene(Scene newScene);

	// 現在のシーンの GameObjectを取得
    static const std::vector<GameObject*>& GetGameObjects() {
        return m_SceneGameObjects;
    }

private:
    // ----------------------------------------------------------------------
    // 変数定義
    // ----------------------------------------------------------------------
    static Scene m_CurrentScene;
    static std::vector<GameObject*> m_SceneGameObjects;
};
