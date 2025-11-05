#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "manager.h"
#include "Ball.h"

void Camera::Init()
{
    m_Yaw = XM_PI;   // プレイヤーの背後
    m_Pitch = 0.7f;
    m_Distance = 8.0f;

    // マウス初期化
	// クライアント領域の中心を計算してスクリーン座標に変換
	m_CenterPos.x = SCREEN_WIDTH / 2.0f;
	m_CenterPos.y = SCREEN_HEIGHT / 2.0f;
    ClientToScreen(GetWindow(), &m_CenterPos);

    // カーソルを中心にワープ
	SetCursorPos(m_CenterPos.x, m_CenterPos.y);
	m_FirstMouse = true;

    // シーン内にいる Ball を探す
    Ball* ball = nullptr;
    for (auto obj : Manager::GetGameObjects())
    {
        if ((ball = dynamic_cast<Ball*>(obj)))
            break;
    }
    if (!ball) return;

    // Ball の位置を注視点に
    auto p = ball->GetPosition();
    m_Target = XMFLOAT3(p.x, p.y, p.z);

    // 極座標からカメラ位置を計算
    float x = m_Distance * sinf(m_Pitch) * sinf(m_Yaw);
    float z = m_Distance * sinf(m_Pitch) * cosf(m_Yaw);
    float y = m_Distance * cosf(m_Pitch);

    m_Position = XMFLOAT3(
        p.x + x,
        p.y + y,
        p.z + z
    );
}

void Camera::Uninit()
{
}

void Camera::RotatePitch(float angle)
{
    m_Pitch += angle;
    
    // 仰角の制限（カメラが地面に潜り込まないよう制限）
    const float MAX_PITCH = 1.3f;  // 約75度
    const float MIN_PITCH = -0.3f; // 約-17度
    
    if (m_Pitch > MAX_PITCH) m_Pitch = MAX_PITCH;
    if (m_Pitch < MIN_PITCH) m_Pitch = MIN_PITCH;
}

void Camera::SetDistance(float distance)
{
    m_Distance = distance;
    
    // 距離の制限
    const float MIN_DISTANCE = 2.0f;
    const float MAX_DISTANCE = 10.0f;
    
    if (m_Distance < MIN_DISTANCE) m_Distance = MIN_DISTANCE;
    if (m_Distance > MAX_DISTANCE) m_Distance = MAX_DISTANCE;
}

void Camera::Update()
{
    Ball* ball = nullptr;
    for (auto obj : Manager::GetGameObjects()) {
        ball = dynamic_cast<Ball*>(obj);
        if (ball) break;
    }
    if (!ball) return;

    Vector3 ballPos = ball->GetPosition();
    
    // カメラのターゲットをプレイヤーに設定
    m_Target = XMFLOAT3(ballPos.x, ballPos.y, ballPos.z);
    
    // カメラをプレイヤーの周りに配置
    // キー入力でカメラを回転
    if (GetAsyncKeyState('Q') & 0x8000) {
        RotateYaw(-0.02f); // 左回転
    }
    if (GetAsyncKeyState('E') & 0x8000) {
        RotateYaw(+0.02f); // 右回転
    }
    if (GetAsyncKeyState('R') & 0x8000) {
        RotatePitch(0.02f); // 上向き
    }
    if (GetAsyncKeyState('F') & 0x8000) {
        RotatePitch(-0.02f); // 下向き
    }
    
    // ズームイン・アウト
    if (GetAsyncKeyState('Z') & 0x8000) {
        SetDistance(m_Distance - 0.1f); // ズームイン
    }
    if (GetAsyncKeyState('X') & 0x8000) {
        SetDistance(m_Distance + 0.1f); // ズームアウト
    }

	// --- マウスによるカメラ回転 ---
	ShowCursor(FALSE); // カーソルを非表示にする
	POINT cur;
	GetCursorPos(&cur);
	// 中心とのデルタを計算
	int dx = cur.x - m_CenterPos.x;
	int dy = cur.y - m_CenterPos.y;
	if (dx != 0 || dy != 0) {
		RotateYaw  (dx * m_MouseSensitivity);
		RotatePitch(-dy * m_MouseSensitivity);
		// 処理後カーソルを再度中央に
		SetCursorPos(m_CenterPos.x, m_CenterPos.y);
	}

    // --- マウスホイールによるズーム ---  
    // ※WM_MOUSEWHEEL を WinProc で拾ってグローバル変数に delta を持ってきても OK。
    // ここでは簡易的に GetAsyncKeyState(VK_XBUTTON1/2) で左右ボタン代用例：
    if (GetAsyncKeyState(VK_XBUTTON1) & 0x8000) {
        SetDistance(m_Distance - 0.1f);
    }
    if (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) {
        SetDistance(m_Distance + 0.1f);
    }
    
    // 極座標からカメラ位置を計算
    float x = m_Distance * sinf(m_Pitch) * sinf(m_Yaw);
    float z = m_Distance * sinf(m_Pitch) * cosf(m_Yaw);
    float y = m_Distance * cosf(m_Pitch);
    
    // 斜め後ろからの視点となるようオフセットを調整
    m_Position = XMFLOAT3(
        ballPos.x + x,
        ballPos.y + y,
        ballPos.z + z
    );
}

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

    // ビュー・マトリクス
    XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };
    m_View = XMMatrixLookAtLH(
        XMLoadFloat3(&m_Position),
        XMLoadFloat3(&m_Target),
        XMLoadFloat3(&up)
    );

    Renderer::SetViewMatrix(m_View);
}
