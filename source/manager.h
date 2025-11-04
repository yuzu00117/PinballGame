// manager.h
#pragma once
#include <vector>
#include "gameObject.h"
#include "scene.h"

class Manager {
public:
    static void Init();
    static void Uninit();
    static void Update();
    static void Draw();

	using Scene = ::Scene;
    static void ChangeScene(Scene newScene);

	// Œ»İ‚ÌƒV[ƒ“‚Ì GameObject‚ğæ“¾
    static const std::vector<GameObject*>& GetGameObjects() {
        return m_SceneGameObjects;
    }

private:
    static Scene m_CurrentScene;
    static std::vector<GameObject*> m_SceneGameObjects;
};
