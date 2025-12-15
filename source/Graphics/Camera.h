#pragma once

// システム関連ヘッダ
#include <DirectXMath.h>
#include <windows.h>
// ゲーム関連ヘッダ
#include "GameObject.h"
// コンポーネント関連ヘッダ
#include "Transform.h"

using namespace DirectX;

/// <summary>
/// カメラクラス
/// </summary>
class Camera : public GameObject
{
public:
    // ------------------------------------------------------------------------------
    // 関数定義
    // ------------------------------------------------------------------------------
    /// <summary>
    /// ライフサイクルメソッド
    /// </summary>
    void Init() override;
    void Uninit() override;
    void Update(float deltaTime) override;
    void Draw() override;

    /// <summary>
    /// カメラ位置のgetter
    /// </summary>
    const XMFLOAT3& GetPosition() const { return m_Position; }

    /// <summary>
    /// カメラの注視点のgetter
    /// </summary>
    const XMFLOAT3& GetTarget() const { return m_Target; }
    
    /// <summary>
    /// カメラの回転角を取得
    /// </summary>
    float GetYaw() const { return m_Yaw; }

    /// <summary>
    /// ビュー行列のgetter
    /// </summary>
    const XMMATRIX& GetViewMatrix() const { return m_View; }

    /// <summary>
    /// Transformへのアクセス
    /// カメラの位置・回転・スケールを外部から触りたい場合用
    /// </summary>
    Transform*       GetTransform()       { return &m_Transform; }
    const Transform* GetTransform() const { return &m_Transform; }
    
    /// <summary>
    /// カメラ回転制御
    /// </summary>
    void RotateYaw(float angle) { m_Yaw += angle; }
    void RotatePitch(float angle);
    void SetDistance(float distance);

private:
    // ------------------------------------------------------------------------------
    // 定数定義
    // ------------------------------------------------------------------------------
    // カメラの初期設定値
    static constexpr float kDefaultYaw = XM_PI;                             // デフォルトのY軸回りの回転角（ラジアン）
    static constexpr float kDefaultPitch = 0.7f;                            // デフォルトのX軸回りの回転角（ラジアン）
    static constexpr float kDefaultDistance = 8.0f;                         // デフォルトのプレイヤーからの距離
    static constexpr XMFLOAT3 kDefaultCameraOffset{ 0.0f, 25.0f, -15.0f };  // デフォルトのカメラオフセット

    // ------------------------------------------------------------------------------
    // 変数定義
    // ------------------------------------------------------------------------------
    // コンポーネント
    Transform m_Transform;                                                  // Transformコンポーネント

    // カメラ行列関連
    XMMATRIX m_Projection;                                                  // プロジェクション行列
    XMMATRIX m_View;                                                        // ビュー行列

    // 便宜上キャッシュ
    XMFLOAT3 m_Position{ 0.0f, 0.0f, 0.0f };                                // カメラ位置
    XMFLOAT3 m_Target{ 0.0f, 0.0f, 0.0f };                                  // カメラの注視点
    XMFLOAT3 m_CameraOffset = kDefaultCameraOffset;                         // カメラのオフセット
    
    // カメラの回転角と距離
    float m_Yaw = kDefaultYaw;                                              // Y軸回りの回転角（ラジアン）
    float m_Pitch = kDefaultPitch;                                          // X軸回りの回転角（ラジアン）
    float m_Distance = kDefaultDistance;                                    // プレイヤーからの距離

    // マウス操作用
	bool    m_FirstMouse = true;                                            // 初回マウス操作フラグ
	POINT   m_CenterPos;                                                    // マウスの中心位置
	float   m_MouseSensitivity = 0.002f;                                    // マウス感度

    // デバッグカメラ用
    bool m_DebugCameraMode = false;                                         // デバッグカメラモードフラグ
    float m_DebugCameraSpeed = 0.15f;                                        // デバッグカメラの移動速度
};
