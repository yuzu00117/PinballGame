// Scene.h
#pragma once

#include <vector>
#include "GameObject.h"

// 各シーンで使うオブジェクトのヘッダー
#include "title.h"
#include "camera.h"
#include "field.h"
#include "Ball.h"
#include "polygon.h"
#include "result.h"
#include "score.h"
#include "soundManager.h"
#include "Bumper.h"
#include "Flipper.h"

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
        objs.push_back(new Ball());
        // // バンパーを追加
        // objs.push_back(new Bumper({ 0.0f, 0.0f, 10.0f }, 1.2f, 1.15f, 1.5f));
        // objs.push_back(new Bumper({-3.0f, 0.0f, 8.0f }, 1.0f, 1.12f, 1.2f));
        // objs.push_back(new Bumper({ 3.0f, 0.0f, 8.0f }, 1.0f, 1.12f, 1.2f));
        // // ▼ フリッパー（左右）を追加
        // {
        //     // 左フリッパー
        //     auto* left = new Flipper();
        //     FlipperDesc L;
        //     L.pivot      = { -1.2f, 0.5f, -10.5f }; // XZの位置、Yは高さ。床y=0で半径0.5なら0.5付近が安定
        //     L.length     = 1.30f;
        //     L.thickness  = 0.20f;
        //     L.restAngle  = -0.35f;   // 少し開いた待機角
        //     L.maxAngle   =  0.75f;   // 叩き上げ角
        //     L.upSpeed    = 18.0f;    // 上げる速さ
        //     L.downSpeed  = 10.0f;    // 戻る速さ
        //     L.restitution= 0.35f;    // 反発
        //     L.hitBoost   = 1.2f;     // “弾き”ブースト
        //     L.invert     = false;    // 左
        //     L.key        = VK_LEFT;  // ←キー
        //     left->Reset(L);
        //     objs.push_back(left);

        //     // 右フリッパー
        //     auto* right = new Flipper();
        //     FlipperDesc R = L;
        //     R.pivot.x   =  1.2f;
        //     R.restAngle =  0.35f;
        //     R.maxAngle  = -0.75f;    // 符号反転で上げる
        //     R.invert    = true;      // 右
        //     R.key       = VK_RIGHT;  // →キー
        //     right->Reset(R);
        //     objs.push_back(right);
        // }
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
