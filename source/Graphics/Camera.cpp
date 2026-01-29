#include "Camera.h"

// システム
#include "main.h"
#include "renderer.h"
#include "Input.h"
#include "GameManager.h"

// オブジェクト
#include "Ball.h"


namespace
{
    // ----------------------------------------------------------------------
    // ヘルパー
    // ----------------------------------------------------------------------
    /// 現在のシーンから Ball を探索する
    /// - 戻り値：非所有ポインタ（Ball の生存期間内のみ有効）
    /// - 見つからない場合は nullptr
    Ball* FindBall()
    {
        for (auto obj : GameManager::GetGameObjects())
        {
            if (auto* ball = dynamic_cast<Ball*>(obj))
            {
                return ball;
            }
        }
        return nullptr;
    }
}


//------------------------------------------------------------------------------
// ライフサイクル
//------------------------------------------------------------------------------
/// 初期化
/// - ボールが存在する場合、追従ターゲットをボール座標に設定する
/// - カメラ位置は固定オフセット（m_CameraOffset）で初期化する
void Camera::Init()
{
    Ball* ball = FindBall();
    if (!ball)
    {
        return;
    }

    const auto p = ball->GetPosition();
    m_Target = XMFLOAT3(p.x, p.y, p.z);

    const XMFLOAT3 offset = m_CameraOffset;
    m_Transform.Position =
    {
        p.x + offset.x,
        p.y + offset.y,
        p.z + offset.z
    };

    const XMMATRIX world = m_Transform.GetWorldMatrix();
    XMStoreFloat3(&m_Position, world.r[3]);
}

/// 終了処理
/// - カーソルクリップを解除し、カーソル表示を戻す
void Camera::Uninit()
{
    ClipCursor(nullptr);
    ShowCursor(TRUE);
}

/// 更新処理
/// - _DEBUG:
///   - F2 で自由カメラモードを切り替える
///   - 通常時：ボール追従（固定オフセット）
///   - 自由時：マウスルック + キー移動（矢印/PGUP/PGDN）
/// - Release:
///   - 常にボール追従（固定オフセット）
/// NOTE:
/// - deltaTime は現状未使用（速度がフレーム依存になる）。必要なら移動量に乗算する設計へ変更する。
void Camera::Update(float deltaTime)
{
#if defined(_DEBUG)
    // ----------------------------------------------------------------------
    // 自由カメラ切替
    // ----------------------------------------------------------------------
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

                POINT lt{ rect.left,  rect.top };
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

    // ----------------------------------------------------------------------
    // 通常：ボール追従
    // ----------------------------------------------------------------------
    if (!m_DebugCameraMode)
    {
        Ball* ball = FindBall();
        if (!ball)
        {
            return;
        }

        const Vector3 ballPos = ball->GetPosition();
        m_Target = XMFLOAT3(ballPos.x, ballPos.y, ballPos.z);

        const XMFLOAT3 offset = m_CameraOffset;
        m_Transform.Position =
        {
            ballPos.x + offset.x,
            ballPos.y + offset.y,
            ballPos.z + offset.z
        };

        const XMMATRIX world = m_Transform.GetWorldMatrix();
        XMStoreFloat3(&m_Position, world.r[3]);
        return;
    }

    // ----------------------------------------------------------------------
    // デバッグ：マウスルック + 移動
    // ----------------------------------------------------------------------
    HWND  hWnd = GetActiveWindow();
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

        const float dx = float(curPos.x - centerScreen.x) * m_MouseSensitivity;
        const float dy = float(curPos.y - centerScreen.y) * m_MouseSensitivity;

        // カーソルを中央へ戻して相対移動量を測定
        SetCursorPos(centerScreen.x, centerScreen.y);

        m_Yaw += dx;
        m_Pitch -= dy;

        // Pitch クランプ（ジンバルロック回避）
        const float kMaxPitch = XM_PIDIV2 - 0.1f;
        const float kMinPitch = -XM_PIDIV2 + 0.1f;
        if (m_Pitch > kMaxPitch) m_Pitch = kMaxPitch;
        if (m_Pitch < kMinPitch) m_Pitch = kMinPitch;
    }

    // ----------------------------------------------------------------------
    // 前方/右ベクトル構築（Yaw/Pitch）
    // ----------------------------------------------------------------------
    XMVECTOR forward = XMVectorSet(
        cosf(m_Pitch) * sinf(m_Yaw),
        sinf(m_Pitch),
        cosf(m_Pitch) * cosf(m_Yaw),
        0.0f);
    forward = XMVector3Normalize(forward);

    XMVECTOR right = XMVector3Normalize(
        XMVectorSet(
            sinf(m_Yaw - XM_PIDIV2),
            0.0f,
            cosf(m_Yaw - XM_PIDIV2),
            0.0f));

    XMVECTOR move = XMVectorZero();

    // 矢印キー：カメラ軸移動
    if (Input::GetKeyPress(VK_UP))    move += forward * m_DebugCameraSpeed;
    if (Input::GetKeyPress(VK_DOWN))  move -= forward * m_DebugCameraSpeed;
    if (Input::GetKeyPress(VK_RIGHT)) move -= right   * m_DebugCameraSpeed;
    if (Input::GetKeyPress(VK_LEFT))  move += right   * m_DebugCameraSpeed;

    // PageUp/PageDown：上下移動
    if (Input::GetKeyPress(VK_PRIOR)) move += XMVectorSet(0, 1, 0, 0) * m_DebugCameraSpeed; // PageUp
    if (Input::GetKeyPress(VK_NEXT))  move -= XMVectorSet(0, 1, 0, 0) * m_DebugCameraSpeed; // PageDown

    XMFLOAT3 d{};
    XMStoreFloat3(&d, move);

    m_Transform.Position.x += d.x;
    m_Transform.Position.y += d.y;
    m_Transform.Position.z += d.z;

    // 注視点は前方へ 1.0 だけ伸ばす
    m_Target.x = m_Transform.Position.x + XMVectorGetX(forward);
    m_Target.y = m_Transform.Position.y + XMVectorGetY(forward);
    m_Target.z = m_Transform.Position.z + XMVectorGetZ(forward);

#else
    // ----------------------------------------------------------------------
    // Release：常にボール追従
    // ----------------------------------------------------------------------
    m_DebugCameraMode = false;

    Ball* ball = FindBall();
    if (!ball)
    {
        return;
    }

    const Vector3 ballPos = ball->GetPosition();
    m_Target = XMFLOAT3(ballPos.x, ballPos.y, ballPos.z);

    const XMFLOAT3 offset = m_CameraOffset;
    m_Transform.Position =
    {
        ballPos.x + offset.x,
        ballPos.y + offset.y,
        ballPos.z + offset.z
    };

    const XMMATRIX world = m_Transform.GetWorldMatrix();
    XMStoreFloat3(&m_Position, world.r[3]);
#endif
}


//------------------------------------------------------------------------------
// 描画処理
//------------------------------------------------------------------------------
/// 描画処理
/// - Projection を生成して Renderer に適用する
/// - Transform からカメラ位置を求め、Target とともに View を生成して適用する
void Camera::Draw()
{
    m_Projection = XMMatrixPerspectiveFovLH(
        XMConvertToRadians(45.0f),
        (float)SCREEN_WIDTH / SCREEN_HEIGHT,
        1.0f,
        1000.0f);

    Renderer::SetProjectionMatrix(m_Projection);

    const XMMATRIX world = m_Transform.GetWorldMatrix();
    XMStoreFloat3(&m_Position, world.r[3]);

    const XMFLOAT3 up{ 0.0f, 1.0f, 0.0f };
    m_View = XMMatrixLookAtLH(
        XMLoadFloat3(&m_Position),
        XMLoadFloat3(&m_Target),
        XMLoadFloat3(&up));

    Renderer::SetViewMatrix(m_View);
}


//------------------------------------------------------------------------------
// 操作
//------------------------------------------------------------------------------
/// Pitch 回転を加算する（ゲームプレイ用の範囲でクランプ）
/// - angle: ラジアン
void Camera::RotatePitch(float angle)
{
    m_Pitch += angle;

    const float kMaxPitch = 1.3f;
    const float kMinPitch = -0.3f;

    if (m_Pitch > kMaxPitch) m_Pitch = kMaxPitch;
    if (m_Pitch < kMinPitch) m_Pitch = kMinPitch;
}

/// 距離を設定する（クランプ）
/// - distance: 距離
/// NOTE:
/// - 現状、追従位置計算に m_Distance は未使用（m_CameraOffset を使っている）
///   反映する場合は「Yaw/Pitch/Distance からオフセットを算出」する実装へ整理する。
void Camera::SetDistance(float distance)
{
    m_Distance = distance;

    const float kMinDistance = 2.0f;
    const float kMaxDistance = 10.0f;

    if (m_Distance < kMinDistance) m_Distance = kMinDistance;
    if (m_Distance > kMaxDistance) m_Distance = kMaxDistance;
}
