#pragma once

#include "gameobject.h"
#include "vector3.h"
#include "FieldBuilder.h"

// 前方宣言
class MeshRenderer;
class ColliderGroup;

/// ピンボールのフィールドオブジェクトクラス
class Field : public GameObject
{
public:
    /// ライフサイクルメソッド
    void Init() override;
    void Uninit() override;
    void Update(float deltaTime) override;
    void Draw() override;

private:
    /// <summary>
    /// ステージ01のフィールドレイアウトを作成
    /// FieldBuilder に渡す初期配置定義を組み立てる
    /// </summary>
    static FieldLayout MakeStage01Layout();
    
    // ----------------------------------------------------------------------
    // フィールド寸法
    // ----------------------------------------------------------------------
    static constexpr float          kHalfWidth  = 9.5f;                               // フィールドの幅の半分
    static constexpr float          kHalfHeight = 14.5f;                              // フィールドの高さの半分
    static constexpr float          kWallThick  = 1.0f;                               // 壁の厚さ
    static constexpr float          kWallHeight = 2.0f;                               // 壁の高さ
    
    // ----------------------------------------------------------------------
    // リソース
    // ----------------------------------------------------------------------
    static constexpr const wchar_t* kFieldTexturePath = L"asset\\texture\\Field.png"; // フィールドテクスチャのパス
    static constexpr const wchar_t* kWallTexturePath  = L"asset\\texture\\Wall2.png"; // 壁テクスチャのパス
    static constexpr const char*    kVertexShaderPath = "shader\\bin\\BaseLitVS.cso"; // 頂点シェーダのパス
    static constexpr const char*    kPixelShaderPath  = "shader\\bin\\BaseLitPS.cso"; // ピクセルシェーダのパス

    // ----------------------------------------------------------------------
    // コンポーネントの参照ポインタ
    // ----------------------------------------------------------------------
    MeshRenderer* m_Floor = nullptr;        // 床メッシュ描画用（非所有）
    ColliderGroup* m_ColliderGroup = nullptr; // 床/壁の当たり判定（非所有）
    LevelObjects m_Level; // フィールド内のレベルオブジェクト群（非所有参照の集合）
};
