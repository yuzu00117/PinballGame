#include "Camera.h"
#include "main.h"
#include "renderer.h"
#include "GameManager.h"
#include "Ball.h"
#include "Input.h"

namespace
{
    Ball* FindBall()
    {
        for (auto obj : GameManager::GetGameObjects())
        {
            if (auto ball = dynamic_cast<Ball*>(obj))
            {
                return ball;
            }
        }
        return nullptr;
    }
}

void Camera::Init()
{
    Ball* ball = FindBall();
    if (!ball) return;

    auto p = ball->GetPosition();
    m_Target = XMFLOAT3(p.x, p.y, p.z);

    const XMFLOAT3 offset = m_CameraOffset;
    m_Transform.Position = {
        p.x + offset.x,
        p.y + offset.y,
        p.z + offset.z
    };

    const XMMATRIX world = m_Transform.GetWorldMatrix();
    XMStoreFloat3(&m_Position, world.r[3]);
}

void Camera::Uninit()
{
    ClipCursor(nullptr);
    ShowCursor(TRUE);
}

void Camera::Update(float deltaTime)
{
#if defined(_DEBUG)
    // --------------------------------------------------------------
    // --------------------------------------------------------------
    if (Input::GetKeyTrigger(VK_F2))
    {
        m_DebugCameraMode = !m_DebugCameraMode;
        m_FirstMouse = true;

        if (m_DebugCameraMode)
        {
            ShowCursor(FALSE);

            HWND hWnd = GetActiveWindow();
            if (hWnd)
            {
                RECT rect;
                GetClientRect(hWnd, &rect);
                POINT lt{ rect.left,  rect.top    };
                POINT rb{ rect.right, rect.bottom };

                ClientToScreen(hWnd, &lt);
                ClientToScreen(hWnd, &rb);

                rect.left   = lt.x;
                rect.top    = lt.y;
                rect.right  = rb.x;
                rect.bottom = rb.y;

                ClipCursor(&rect);
            }
        }
        else
        {
            ClipCursor(nullptr);
            ShowCursor(TRUE);
        }
    }

    if (!m_DebugCameraMode)
    {
        Ball* ball = FindBall();
        if (!ball)
            return;

        Vector3 ballPos = ball->GetPosition();

        m_Target = XMFLOAT3(ballPos.x, ballPos.y, ballPos.z);

        const XMFLOAT3 offset = m_CameraOffset;

        m_Transform.Position = {
            ballPos.x + offset.x,
            ballPos.y + offset.y,
            ballPos.z + offset.z
        };

        const XMMATRIX world = m_Transform.GetWorldMatrix();
        XMStoreFloat3(&m_Position, world.r[3]);

        return;
    }

    HWND hWnd = GetActiveWindow();
    POINT curPos{};
    POINT centerClient{};
    POINT centerScreen{};

    if (hWnd)
    {
        RECT rect;
        GetClientRect(hWnd, &rect);

        centerClient.x = (rect.right - rect.left) / 2;
        centerClient.y = (rect.bottom - rect.top) / 2;

        centerScreen = centerClient;
        ClientToScreen(hWnd, &centerScreen);

        GetCursorPos(&curPos);

        float dx = float(curPos.x - centerScreen.x) * m_MouseSensitivity;
        float dy = float(curPos.y - centerScreen.y) * m_MouseSensitivity;

        SetCursorPos(centerScreen.x, centerScreen.y);

        m_Yaw += dx;

        m_Pitch -= dy;

        const float MAX_PITCH = XM_PIDIV2 - 0.1f;
        const float MIN_PITCH = -XM_PIDIV2 + 0.1f;
        if (m_Pitch > MAX_PITCH) m_Pitch = MAX_PITCH;
        if (m_Pitch < MIN_PITCH) m_Pitch = MIN_PITCH;
    }

    XMVECTOR forward = XMVectorSet(
        cosf(m_Pitch) * sinf(m_Yaw),
        sinf(m_Pitch),
        cosf(m_Pitch) * cosf(m_Yaw),
        0.0f
    );
    forward = XMVector3Normalize(forward);

    XMVECTOR right = XMVector3Normalize(
        XMVectorSet(
            sinf(m_Yaw - XM_PIDIV2),
            0.0f,
            cosf(m_Yaw - XM_PIDIV2),
            0.0f
        )
    );

    XMVECTOR move = XMVectorZero();

    if (Input::GetKeyPress(VK_UP))    move += forward * m_DebugCameraSpeed;
    if (Input::GetKeyPress(VK_DOWN))  move -= forward * m_DebugCameraSpeed;
    if (Input::GetKeyPress(VK_RIGHT)) move -= right   * m_DebugCameraSpeed;
    if (Input::GetKeyPress(VK_LEFT))  move += right   * m_DebugCameraSpeed;

    if (Input::GetKeyPress(VK_PRIOR)) // PageUp
        move += XMVectorSet(0, 1, 0, 0) * m_DebugCameraSpeed;
    if (Input::GetKeyPress(VK_NEXT))  // PageDown
        move -= XMVectorSet(0, 1, 0, 0) * m_DebugCameraSpeed;

    XMFLOAT3 d;
    XMStoreFloat3(&d, move);

    m_Transform.Position.x += d.x;
    m_Transform.Position.y += d.y;
    m_Transform.Position.z += d.z;

    m_Target.x = m_Transform.Position.x + XMVectorGetX(forward);
    m_Target.y = m_Transform.Position.y + XMVectorGetY(forward);
    m_Target.z = m_Transform.Position.z + XMVectorGetZ(forward);

#else
    // -----------------------------------------------
    // -----------------------------------------------
    m_DebugCameraMode = false;

    Ball* ball = FindBall();
    if (!ball)
        return;

    Vector3 ballPos = ball->GetPosition();

    m_Target = XMFLOAT3(ballPos.x, ballPos.y, ballPos.z);

    const XMFLOAT3 offset = m_CameraOffset;

    m_Transform.Position = {
        ballPos.x + offset.x,
        ballPos.y + offset.y,
        ballPos.z + offset.z
    };

    const XMMATRIX world = m_Transform.GetWorldMatrix();
    XMStoreFloat3(&m_Position, world.r[3]);
#endif
}



void Camera::Draw()
{
    m_Projection = XMMatrixPerspectiveFovLH(
        XMConvertToRadians(45.0f),
        (float)SCREEN_WIDTH / SCREEN_HEIGHT,
        1.0f,
        1000.0f
    );

    Renderer::SetProjectionMatrix(m_Projection);

    const XMMATRIX world = m_Transform.GetWorldMatrix();
    XMStoreFloat3(&m_Position, world.r[3]);

    XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };
    m_View = XMMatrixLookAtLH(
        XMLoadFloat3(&m_Position),
        XMLoadFloat3(&m_Target),
        XMLoadFloat3(&up)
    );

    Renderer::SetViewMatrix(m_View);
}

void Camera::RotatePitch(float angle)
{
    m_Pitch += angle;
    
    const float MAX_PITCH = 1.3f;
    const float MIN_PITCH = -0.3f;
    
    if (m_Pitch > MAX_PITCH) m_Pitch = MAX_PITCH;
    if (m_Pitch < MIN_PITCH) m_Pitch = MIN_PITCH;
}

void Camera::SetDistance(float distance)
{
    m_Distance = distance;
    
    const float MIN_DISTANCE = 2.0f;
    const float MAX_DISTANCE = 10.0f;
    
    if (m_Distance < MIN_DISTANCE) m_Distance = MIN_DISTANCE;
    if (m_Distance > MAX_DISTANCE) m_Distance = MAX_DISTANCE;
}
