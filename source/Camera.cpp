#include "Camera.h"
#include "main.h"
#include "renderer.h"
#include "manager.h"
#include "Ball.h"

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
}

// 更新処理
void Camera::Update()
{
    // シーン内にいる Ball を探す
    Ball* ball = FindBall();
    if (!ball) return;

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