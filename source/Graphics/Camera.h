#pragma once

#include <DirectXMath.h>
#include <windows.h>
#include "GameObject.h"
#include "Transform.h"

using namespace DirectX;

/// <summary>
/// </summary>
class Camera : public GameObject
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
    const XMFLOAT3& GetPosition() const { return m_Position; }

    /// <summary>
    /// </summary>
    const XMFLOAT3& GetTarget() const { return m_Target; }
    
    /// <summary>
    /// </summary>
    float GetYaw() const { return m_Yaw; }

    /// <summary>
    /// </summary>
    const XMMATRIX& GetViewMatrix() const { return m_View; }

    /// <summary>
    /// </summary>
    Transform*       GetTransform()       { return &m_Transform; }
    const Transform* GetTransform() const { return &m_Transform; }
    
    /// <summary>
    /// </summary>
    void RotateYaw(float angle) { m_Yaw += angle; }
    void RotatePitch(float angle);
    void SetDistance(float distance);

private:
    // ------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------
    static constexpr float kDefaultYaw = XM_PI;
    static constexpr float kDefaultPitch = 0.7f;
    static constexpr float kDefaultDistance = 8.0f;
    static constexpr XMFLOAT3 kDefaultCameraOffset{ 0.0f, 25.0f, -15.0f };

    // ------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------
    Transform m_Transform;

    XMMATRIX m_Projection;
    XMMATRIX m_View;

    XMFLOAT3 m_Position{ 0.0f, 0.0f, 0.0f };
    XMFLOAT3 m_Target{ 0.0f, 0.0f, 0.0f };
    XMFLOAT3 m_CameraOffset = kDefaultCameraOffset;
    
    float m_Yaw = kDefaultYaw;
    float m_Pitch = kDefaultPitch;
    float m_Distance = kDefaultDistance;

	bool    m_FirstMouse = true;
	POINT   m_CenterPos;
	float   m_MouseSensitivity = 0.002f;

    bool m_DebugCameraMode = false;
    float m_DebugCameraSpeed = 0.15f;
};
