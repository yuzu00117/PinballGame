#pragma once

#include "gameobject.h"
#include "vector3.h"

// 前方宣言
class MeshRenderer;
class ColliderGroup;

/// <summary>
/// ピンボールのフィールドオブジェクトクラス
/// </summary>
class Field : public GameObject
{
public:
    // ----------------------------------------------------------------------
    // 関数定義
    // ----------------------------------------------------------------------
    void Init() override;
    void Uninit() override;
    void Update(float deltaTime) override;
    void Draw() override;

private:
    // ----------------------------------------------------------------------
    // 定数定義
    // ----------------------------------------------------------------------
    static constexpr float          kHalfWidth  = 9.5f;                               // フィールドの幅の半分
    static constexpr float          kHalfHeight = 14.5f;                              // フィールドの高さの半分
    static constexpr float          kWallThick  = 1.0f;                               // 壁の厚さ
    static constexpr float          kWallHeight = 2.0f;                               // 壁の高さ
    static constexpr const wchar_t* kFieldTexturePath = L"asset\\texture\\Field.png"; // テクスチャのパス
    static constexpr const wchar_t* kWallTexturePath  = L"asset\\texture\\Wall2.png";  // テクスチャのパス
    static constexpr const char*    kVertexShaderPath = "shader\\bin\\BaseLitVS.cso"; // 頂点シェーダのパス
    static constexpr const char*    kPixelShaderPath  = "shader\\bin\\BaseLitPS.cso"; // ピクセルシェーダのパス
    
    // ----------------------------------------------------------------------
    // 変数定義
    // ----------------------------------------------------------------------
    MeshRenderer* m_Floor = nullptr;
    ColliderGroup* m_ColliderGroup = nullptr;
};
