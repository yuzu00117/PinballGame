#pragma once

#include "gameobject.h"
#include "vector3.h"

// 前方宣言
class MeshRenderer;
class ColliderGroup;

/// <summary>
/// ピンボールのホール（穴）オブジェクト
/// コライダーに当たったときに、Ball / Enemyを判定して処理する
/// </summary>
class Hole : public GameObject
{
public:
    // ------------------------------------------------------------------------------
    //  関数定義 
    // ------------------------------------------------------------------------------
    /// <summary>
    /// ライフサイクルメソッド
    /// </summary>
    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;

    /// <summary>
    /// 衝突コールバック
    /// Holeのコライダーに当たったときの処理
    /// </summary>
    void OnCollisionEnter(const CollisionInfo& info) override;

private:
    // ------------------------------------------------------------------------------
    // 定数定義
    // ------------------------------------------------------------------------------
    static constexpr const char* VertexShaderPath = // 頂点シェーダのパス
        "shader\\bin\\unlitTextureVS.cso";   
    static constexpr const char* PixelShaderPath  = // ピクセルシェーダのパス
        "shader\\bin\\unlitTexturePS.cso";   

    // ------------------------------------------------------------------------------
    // 変数定義
    // ------------------------------------------------------------------------------
    // 見た目・当たり判定用コンポーネント
    MeshRenderer* m_Floor = nullptr;
    ColliderGroup* m_ColliderGroup = nullptr;
};
