// Scene.h
#pragma once

#include <vector>
#include "GameObject.h"

// 各シーンで使うオブジェクトのヘッダー
#include "title.h"
#include "camera.h"
#include "field.h"
#include "player.h"
#include "polygon.h"
#include "result.h"
#include "score.h"
#include "soundManager.h"

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
        objs.push_back(new Field());
        objs.push_back(new Player());
        objs.push_back(new Polygon2D());
        objs.push_back(new Score()); // スコア表示を追加
        objs.push_back(new SoundManager()); // 音声管理を追加
        break;
    case Scene::Result:
        objs.push_back(new Result());
        break;
    }
    return objs;
}
