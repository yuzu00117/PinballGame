#pragma once

#include <vector>
#include <set>
#include "gameObject.h"
#include "scene.h"

/// <summary>
/// ゲームの管理をするマネージャークラス
/// シーンの切り替え・GameObjectの管理・描画処理などを担当
/// </summary>
class GameManager {
public:
    // ----------------------------------------------------------------------
    // 型定義
    // ----------------------------------------------------------------------
    /// <summary>
    /// シーン列挙型のエイリアス
    /// </summary>
    using Scene = ::Scene;

    // ----------------------------------------------------------------------
    // 関数定義
    // ----------------------------------------------------------------------
    /// <summary>
    /// ライフサイクルメソッド
    /// </summary>
    static void Init();
    static void Uninit();
    static void Update(float deltaTime);
    static void Draw();

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
    // 構造体定義
    // ----------------------------------------------------------------------
    struct ColliderPair {
        Collider* first;
        Collider* second;

        // 比較演算子を定義して set で使用可能にする
        bool operator<(const ColliderPair& other) const {
            return std::tie(first, second) < std::tie(other.first, other.second);
        }
    };

    // ----------------------------------------------------------------------
    // 関数定義
    // ----------------------------------------------------------------------
    /// <summary>
    /// コライダー同士の当たり判定処理
    /// </summary>
    static void CheckCollisions();

    /// <summary>
    /// コライダーペアを作成（順序を気にせず一意に識別するため）
    /// </summary>
    static ColliderPair MakePair(Collider* first, Collider* second)
    {
        return (first < second) ? ColliderPair{ first, second } 
                                : ColliderPair{ second, first };
    }

    // ----------------------------------------------------------------------
    // 変数定義
    // ----------------------------------------------------------------------
    static Scene m_CurrentScene;                          // 現在のシーン
    static std::vector<GameObject*> m_SceneGameObjects;   // 現在のシーンのGameObjectリスト
    static std::set<ColliderPair> m_PreviousPairs;        // 前フレームの衝突ペア情報
    static std::set<ColliderPair> m_PreviousTriggerPairs; // 前フレームのトリガーペア情報

    
};
