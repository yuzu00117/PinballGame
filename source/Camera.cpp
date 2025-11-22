#include "Camera.h"
#include "main.h"
#include "renderer.h"
#include "manager.h"
#include "Ball.h"
#include "Input.h"

// シーン内のボールを探すヘルパー関数
namespace
{
    Ball* FindBall()
    {
        for (auto obj : Manager::GetGameObjects())
        {
            if (auto ball = dynamic_cast<Ball*>(obj))
            {
                return ball;
            }
        }
        return nullptr;
    }
}

// 初期化処理
void Camera::Init()
{
    // シーン内にいる Ball を探して、初期ターゲットを設定
    Ball* ball = FindBall();
    if (!ball) return;

    // Ball の位置を注視点に
    auto p = ball->GetPosition();
    m_Target = XMFLOAT3(p.x, p.y, p.z);

    // 斜め上固定カメラ用の初期位置
    const XMFLOAT3 offset = m_CameraOffset;
    m_Transform.Position = {
        p.x + offset.x,
        p.y + offset.y,
        p.z + offset.z
    };

    // Transformから位置を取得してキャッシュ
    const XMMATRIX world = m_Transform.GetWorldMatrix();
    XMStoreFloat3(&m_Position, world.r[3]);
}

// 終了処理
void Camera::Uninit()
{
    // 念のためカーソル制限と表示を元に戻す
    ClipCursor(nullptr);
    ShowCursor(TRUE);
}

// 更新処理
void Camera::Update()
{
#if defined(_DEBUG)
    // --------------------------------------------------------------
    // デバッグカメラモード切替処理
    // --------------------------------------------------------------
    // F2キーでデバッグカメラモード切替
    if (Input::GetKeyTrigger(VK_F2))
    {
        m_DebugCameraMode = !m_DebugCameraMode;
        m_FirstMouse = true; // モード切り替え時にマウス初期化

        if (m_DebugCameraMode)
        {
            // ----- デバッグカメラ ON：カーソル非表示＆ウィンドウ内に固定 -----
            ShowCursor(FALSE);

            // ウィンドウのクライアント領域をスクリーン座標に変換して ClipCursor
            HWND hWnd = GetActiveWindow(); // 必要なら自前の HWND に差し替え OK
            if (hWnd)
            {
                RECT rect;
                GetClientRect(hWnd, &rect); // クライアント座標
                POINT lt{ rect.left,  rect.top    };
                POINT rb{ rect.right, rect.bottom };

                ClientToScreen(hWnd, &lt); // 左上をスクリーン座標へ
                ClientToScreen(hWnd, &rb); // 右下をスクリーン座標へ

                rect.left   = lt.x;
                rect.top    = lt.y;
                rect.right  = rb.x;
                rect.bottom = rb.y;

                ClipCursor(&rect); // この範囲から出られない
            }
        }
        else
        {
            // ----- デバッグカメラ OFF：カーソル表示＆クリップ解除 -----
            ClipCursor(nullptr); // 制限解除
            ShowCursor(TRUE);
        }
    }

    // デバッグカメラがOFFのときは通常のカメラ処理だけして終了
    if (!m_DebugCameraMode)
    {
        // シーン内にいる Ball を探す
        Ball* ball = FindBall();
        if (!ball)
            return;

        Vector3 ballPos = ball->GetPosition();

        // カメラのターゲットをボールの位置に更新
        m_Target = XMFLOAT3(ballPos.x, ballPos.y, ballPos.z);

        // カメラの固定オフセット
        const XMFLOAT3 offset = m_CameraOffset;

        // カメラ位置を更新
        m_Transform.Position = {
            ballPos.x + offset.x,
            ballPos.y + offset.y,
            ballPos.z + offset.z
        };

        // Transformから位置を取得してキャッシュ
        const XMMATRIX world = m_Transform.GetWorldMatrix();
        XMStoreFloat3(&m_Position, world.r[3]);

        return;    // ★デバッグカメラOFFならここで終わる
    }

    // デバッグカメラ ON 時の処理
    HWND hWnd = GetActiveWindow(); // 必要なら外部の HWND を使う
    POINT curPos{};
    POINT centerClient{};
    POINT centerScreen{};

    if (hWnd)
    {
        // クライアント中央のスクリーン座標を計算
        RECT rect;
        GetClientRect(hWnd, &rect);

        centerClient.x = (rect.right - rect.left) / 2;
        centerClient.y = (rect.bottom - rect.top) / 2;

        centerScreen = centerClient;
        ClientToScreen(hWnd, &centerScreen);

        // 現在のカーソル位置取得
        GetCursorPos(&curPos);

        // 中央からの差分を回転量として使う
        float dx = float(curPos.x - centerScreen.x) * m_MouseSensitivity;
        float dy = float(curPos.y - centerScreen.y) * m_MouseSensitivity;

        // 毎フレーム、カーソルを中央に戻す（中央固定）
        SetCursorPos(centerScreen.x, centerScreen.y);

        // Yaw: 左右回転
        m_Yaw += dx;

        // Pitch: 上下回転（反転済）
        m_Pitch -= dy;

        // ピッチ角の制限
        const float MAX_PITCH = XM_PIDIV2 - 0.1f;
        const float MIN_PITCH = -XM_PIDIV2 + 0.1f;
        if (m_Pitch > MAX_PITCH) m_Pitch = MAX_PITCH;
        if (m_Pitch < MIN_PITCH) m_Pitch = MIN_PITCH;
    }

    // 2. カメラの向きベクトルを計算
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

    // 3. キー入力による移動（向き基準）
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

    // forward 方向へターゲットを設定
    m_Target.x = m_Transform.Position.x + XMVectorGetX(forward);
    m_Target.y = m_Transform.Position.y + XMVectorGetY(forward);
    m_Target.z = m_Transform.Position.z + XMVectorGetZ(forward);

#else
    // -----------------------------------------------
    // Release ビルド専用：通常カメラのみ
    // -----------------------------------------------
    // デバッグカメラは使わない
    m_DebugCameraMode = false;

    // シーン内にいる Ball を探す
    Ball* ball = FindBall();
    if (!ball)
        return;

    Vector3 ballPos = ball->GetPosition();

    // カメラのターゲットをボールの位置に更新
    m_Target = XMFLOAT3(ballPos.x, ballPos.y, ballPos.z);

    // カメラの固定オフセット
    const XMFLOAT3 offset = m_CameraOffset;

    // カメラ位置を更新
    m_Transform.Position = {
        ballPos.x + offset.x,
        ballPos.y + offset.y,
        ballPos.z + offset.z
    };

    // Transformから位置を取得してキャッシュ
    const XMMATRIX world = m_Transform.GetWorldMatrix();
    XMStoreFloat3(&m_Position, world.r[3]);
#endif
}



// 描画処理
void Camera::Draw()
{
    // プロジェクションマトリクス
    m_Projection = XMMatrixPerspectiveFovLH(
        XMConvertToRadians(45.0f),
        (float)SCREEN_WIDTH / SCREEN_HEIGHT,
        1.0f,
        1000.0f
    );

    Renderer::SetProjectionMatrix(m_Projection);

    // 念の為Transformから位置を再取得
    const XMMATRIX world = m_Transform.GetWorldMatrix();
    XMStoreFloat3(&m_Position, world.r[3]);

    // ビュー・マトリクス
    XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };
    m_View = XMMatrixLookAtLH(
        XMLoadFloat3(&m_Position),
        XMLoadFloat3(&m_Target),
        XMLoadFloat3(&up)
    );

    Renderer::SetViewMatrix(m_View);
}

// カメラのピッチ回転を制御
void Camera::RotatePitch(float angle)
{
    m_Pitch += angle;
    
    // 仰角の制限（カメラが地面に潜り込まないよう制限）
    const float MAX_PITCH = 1.3f;  // 約75度
    const float MIN_PITCH = -0.3f; // 約-17度
    
    if (m_Pitch > MAX_PITCH) m_Pitch = MAX_PITCH;
    if (m_Pitch < MIN_PITCH) m_Pitch = MIN_PITCH;
}

// カメラの距離を設定
void Camera::SetDistance(float distance)
{
    m_Distance = distance;
    
    // 距離の制限
    const float MIN_DISTANCE = 2.0f;
    const float MAX_DISTANCE = 10.0f;
    
    if (m_Distance < MIN_DISTANCE) m_Distance = MIN_DISTANCE;
    if (m_Distance > MAX_DISTANCE) m_Distance = MAX_DISTANCE;
}