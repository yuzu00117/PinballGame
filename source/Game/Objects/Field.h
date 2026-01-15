#pragma once

#include "gameobject.h"
#include "vector3.h"
#include "FieldBuilder.h"

// 前方宣言
class MeshRenderer;
class ColliderGroup;

/// <summary>
/// ピンボールのフィールドオブジェクトクラス
/// </summary>
class Field : public GameObject
{
public:
    /// <summary>
    /// ライフサイクルメソッド
    /// </summary>
    void Init() override;
    void Uninit() override;
    void Update(float deltaTime) override;
    void Draw() override;

private:
    /// <summary>
    /// ステージ01のフィールドレイアウトを作成
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
    MeshRenderer* m_Floor = nullptr;
    ColliderGroup* m_ColliderGroup = nullptr;
    
    LevelObjects m_Level; // フィールド内のレベルオブジェクト群
};
