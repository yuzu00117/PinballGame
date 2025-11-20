#pragma once

#include <vector>
#include <set>
#include "gameObject.h"
#include "scene.h"

/// <summary>
/// ゲームの管理をするマネージャークラス
/// シーンの切り替え・GameObjectの管理・描画処理などを担当
/// </summary>
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

	/// <summary>
    /// シーン列挙型のエイリアス
    /// </summary>
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
    // 関数定義
    // ----------------------------------------------------------------------
    /// <summary>
    /// コライダー同士の当たり判定処理
    /// </summary>
    static void CheckCollisions();

    // ----------------------------------------------------------------------
    // 変数定義
    // ----------------------------------------------------------------------
    // TODO: [Q]これは関数定義？
    struct ColliderPair {
        Collider* a;
        Collider* b;

        // Set用ソート
        bool operator<(const ColliderPair& rhs) const
        {
            if (a < rhs.a) return true;
            if (a > rhs.a) return false;
            return b < rhs.b;
        }
    };

    // TODO: [Q]これは関数定義？
    static ColliderPair MakePair(Collider* a, Collider* b)
    {
        return (a < b) ? ColliderPair{ a, b } : ColliderPair{ b, a };
    }

    static Scene m_CurrentScene;                        // 現在のシーン
    static std::vector<GameObject*> m_SceneGameObjects; // 現在のシーンのGameObjectリスト
    static std::set<ColliderPair> m_PreviousPairs;      // 前フレームの衝突ペア情報

    
};
