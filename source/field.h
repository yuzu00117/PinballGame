#pragma once

#include "gameobject.h"
#include "vector3.h"

// ????
class MeshRenderer;
class ColliderGroup;

/// <summary>
/// ??????????????????????
/// </summary>
class Field : public GameObject
{
public:
    // --- ???? ---
    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;

private:
    // --- ???? ---
    static constexpr float HalfWidth  = 9.5f;                                       // ????????
    static constexpr float HalfHeight = 14.5f;                                      // ?????????
    static constexpr float WallThick  = 0.4f;                                       // ????
    static constexpr float WallHeight = 2.0f;                                       // ????
    static constexpr const wchar_t* TexturePath = L"asset\\texture\\field004.jpg";  // ????????
    static constexpr const char* VertexShaderPath = "shader\\unlitTextureVS.cso";   // ????????
    static constexpr const char* PixelShaderPath  = "shader\\unlitTexturePS.cso";   // ??????????
    
    // --- ???? ---
    MeshRenderer*   m_MeshRenderer = nullptr;
    ColliderGroup*  m_ColliderGroup = nullptr;
};
