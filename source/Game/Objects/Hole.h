#pragma once

#include "gameobject.h"
#include "vector3.h"

class MeshRenderer;
class ColliderGroup;

/// <summary>
/// </summary>
class Hole : public GameObject
{
public:
    // ------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------
    /// <summary>
    /// </summary>
    void Init() override;
    void Uninit() override;
    void Update(float deltaTime) override;
    void Draw() override;

    /// <summary>
    /// </summary>
    void OnCollisionEnter(const CollisionInfo& info) override;

    /// <summary>
    /// </summary>
    Vector3 GetHolePosition() const { return m_Transform.Position; }

private:
    // ------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------
    static constexpr const char* VertexShaderPath =
        "shader\\bin\\BaseLitVS.cso";   
    static constexpr const char* PixelShaderPath  =
        "shader\\bin\\BaseLitPS.cso";   

    // ------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------
    MeshRenderer* m_MeshRenderer = nullptr;
    ColliderGroup* m_ColliderGroup = nullptr;
};
