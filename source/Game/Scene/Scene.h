// Scene.h
#pragma once

#include <vector>
#include "GameObject.h"

// 各シーンで使うオブジェクトのヘッダー
#include "Title.h"
#include "Camera.h"
#include "Field.h"
#include "Ball.h"
#include "Polygon.h"
#include "Result.h"
#include "Score.h"
#include "SoundManager.h"
#include "HP.h"
#include "EnemyManager.h"

/// シーン列挙型
enum class Scene {
    Title,
    Game,
    Result
};

/// 指定シーンの GameObject* を生成して返す（inline 実装）
inline std::vector<GameObject*> CreateSceneObjects(Scene scene) {
    std::vector<GameObject*> objs;
    switch (scene) {
    case Scene::Title:
        objs.push_back(new Title());
		break;
    case Scene::Game:
        objs.push_back(new Camera());
        objs.push_back(new EnemyManager());
        objs.push_back(new Field());
        objs.push_back(new Ball());
        objs.push_back(new Polygon2D());
        objs.push_back(new Score());
        objs.push_back(new HP());
        objs.push_back(new SoundManager());
        break;
    case Scene::Result:
        objs.push_back(new Result());
        break;
    }
    return objs;
}
