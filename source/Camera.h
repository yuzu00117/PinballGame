#pragma once

#include <DirectXMath.h>
#include "gameobject.h"
#include <windows.h> // POINT構造体を使用するために必要
using namespace DirectX;

class Camera : public GameObject
{
private:
    XMMATRIX m_Projection;
    XMMATRIX m_View;
    XMFLOAT3 m_Position{ 0.0f, 0.0f, 0.0f };
    XMFLOAT3 m_Target{ 0.0f, 0.0f, 0.0f };
    
    // カメラの回転角と距離
    float m_Yaw = 0.0f;     // Y軸回りの回転角（ラジアン）
    float m_Pitch = 0.3f;   // X軸回りの回転角（ラジアン）
    float m_Distance = 5.0f; // プレイヤーからの距離

    // マウス操作用
	bool    m_FirstMouse = true;    // 初回マウス操作フラグ
	POINT   m_CenterPos;             // マウスの中心位置
	float   m_MouseSensitivity = 0.002f; // マウス感度

public:
    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;

    // カメラ位置・ターゲットのgetter
    const XMFLOAT3& GetPosition() const { return m_Position; }
    const XMFLOAT3& GetTarget() const { return m_Target; }
    
    // カメラの回転角を取得
    float GetYaw() const { return m_Yaw; }
    // ビュー行列のgetterを追加
    const XMMATRIX& GetViewMatrix() const { return m_View; }
    
    // カメラ回転制御
    void RotateYaw(float angle) { m_Yaw += angle; }
    void RotatePitch(float angle);
    void SetDistance(float distance);
};
