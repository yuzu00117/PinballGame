#pragma once

#include "GameObject.h"
#include "Vector3.h"

// 前方宣言
class CollliderGroup;
class ModelRenderer;

/// <summary>
/// ピンボールのバンパークラス
/// TODO: 現在の実装では、どの位置で反射しても同じ力で弾く仕様になっているため、
///       将来的に「当たった位置で弾く力が変わる」ように改良したい
/// </summary>
class Bumper : public GameObject
{
public:
    // ----------------------------------------------------------------------
    // 関数定義
    // ----------------------------------------------------------------------
    /// <summary>
    /// ライフサイクルメソッド
    /// </summary>
    void Init() override;
    void Update(float deltaTime) override;
    void Draw() override;
    void Uninit() override;

    /// <summary>
    /// 衝突コールバック
    /// バンパーでボールを弾くための簡易実装
    /// </summary>
    void OnCollisionStay(const CollisionInfo& info) override;

private:
    // ----------------------------------------------------------------------
    // 定数定義
    // ----------------------------------------------------------------------
    static constexpr float kDefaultColliderRadius = 1.0f;           // デフォルトのコライダー半径

    // ----------------------------------------------------------------------
    // 変数定義
    // ----------------------------------------------------------------------
    // コンポーネント
    ModelRenderer* m_ModelRenderer = nullptr;                       // モデルレンダラー

    // シェーダー
    static constexpr const char* VertexShaderPath =                 // 頂点シェーダのパス
        "shader\\bin\\BaseLitVS.cso";   
    static constexpr const char* PixelShaderPath  =                 // ピクセルシェーダのパス
        "shader\\bin\\BaseLitPS.cso";
};  