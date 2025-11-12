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
    // --- 関数定義 ---
    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;

private:
    // --- 定数定義 ---
    static constexpr float HalfWidth  = 9.5f;                                       // フィールドの幅の半分
    static constexpr float HalfHeight = 14.5f;                                      // フィールドの高さの半分
    static constexpr float WallThick  = 1.0f;                                       // 壁の厚さ
    static constexpr float WallHeight = 2.0f;                                       // 壁の高さ
    static constexpr const wchar_t* TexturePath = L"asset\\texture\\field004.jpg";  // テクスチャのパス
    static constexpr const char* VertexShaderPath = "shader\\unlitTextureVS.cso";   // 頂点シェーダのパス
    static constexpr const char* PixelShaderPath  = "shader\\unlitTexturePS.cso";   // ピクセルシェーダのパス
    
    // --- 変数定義 ---
    MeshRenderer* m_Floor = nullptr;
    ColliderGroup* m_ColliderGroup = nullptr;
};
