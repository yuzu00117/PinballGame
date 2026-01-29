//------------------------------------------------------------------------------
// Camera
//------------------------------------------------------------------------------
// 役割:
// ゲーム内のカメラ（View / Projection の生成と Renderer への適用）を担当する。
// 通常時はボール追従カメラとして動作し、デバッグ時は自由移動カメラに切り替え可能。
//
// 設計意図:
// - GameObject のライフサイクル（Init / Update / Draw）に乗せて管理する。
// - 追従カメラ（ゲームプレイ用）と自由カメラ（デバッグ用）を同一クラスにまとめ、
//   F2 キーで切り替えられるようにする。
// - View/Projection は Draw() で毎フレーム確定し、Renderer に反映する。
//
// 構成:
// - Transform         : カメラ位置の保持（自由カメラ時の移動に使用）
// - View / Projection : 描画に必要な行列
// - Target            : LookAt の注視点（追従時はボール座標、自由時は前方）
// - Debug 操作        : マウスルック + キー移動、カーソルのクリップ制御
//
// NOTE:
// - 本クラスは GameObject::m_Transform を使わず、独自に Transform を保持している。
//   （GameObject 側にも Transform がある設計の場合、二重管理になるため整理対象）
// - SetDistance / Pitch/Yaw を保持しているが、現状 Update() では距離/ピッチの追従反映が無い。
//   （m_CameraOffset による固定オフセットで追従している）
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Camera.h
//------------------------------------------------------------------------------
#pragma once

#include <DirectXMath.h>
#include <windows.h>

#include "GameObject.h"
#include "Transform.h"

using namespace DirectX;

/// カメラクラス
/// - 通常：ボール追従カメラ（固定オフセット）
/// - デバッグ：自由移動 + マウスルック（_DEBUG のみ）
/// - Draw() で View/Projection を生成し Renderer に適用する
class Camera : public GameObject
{
public:
    // ----------------------------------------------------------------------
    // ライフサイクル
    // ----------------------------------------------------------------------
    /// 初期化
    /// - ボールが存在する場合、ボール位置を基準に追従カメラ位置/ターゲットを初期化する
    void Init() override;

    /// 終了処理
    /// - カーソルクリップを解除し、カーソル表示を戻す
    void Uninit() override;

    /// 更新処理
    /// - _DEBUG: F2 で自由カメラ切替、自由カメラ中はマウスルック + キー移動
    /// - Release: 常にボール追従
    void Update(float deltaTime) override;

    /// 描画処理
    /// - Projection / View を生成して Renderer に適用する
    void Draw() override;

public:
    // ----------------------------------------------------------------------
    // アクセサ
    // ----------------------------------------------------------------------
    /// カメラ位置（ワールド座標）
    const XMFLOAT3& GetPosition() const { return m_Position; }

    /// 注視点（LookAt）
    const XMFLOAT3& GetTarget() const { return m_Target; }

    /// Yaw（ラジアン）
    float GetYaw() const { return m_Yaw; }

    /// View 行列（Draw() で構築）
    const XMMATRIX& GetViewMatrix() const { return m_View; }

    /// Transform へのアクセス（主にデバッグ編集用）
    /// NOTE:
    /// - 戻り値は非所有ポインタ（Camera が生存している間のみ有効）
    Transform*       GetTransform()       { return &m_Transform; }
    const Transform* GetTransform() const { return &m_Transform; }

public:
    // ----------------------------------------------------------------------
    // 操作
    // ----------------------------------------------------------------------
    /// Yaw 回転を加算する
    /// - angle: ラジアン
    void RotateYaw(float angle) { m_Yaw += angle; }

    /// Pitch 回転を加算する（クランプ付き）
    /// - angle: ラジアン
    void RotatePitch(float angle);

    /// カメラ距離を設定する（クランプ付き）
    /// NOTE:
    /// - 現状、Update() の追従処理では m_Distance が反映されていない（整理対象）
    void SetDistance(float distance);

private:
    // ----------------------------------------------------------------------
    // 既定値
    // ----------------------------------------------------------------------
    static constexpr float   kDefaultYaw      = XM_PI;
    static constexpr float   kDefaultPitch    = 0.7f;
    static constexpr float   kDefaultDistance = 8.0f;
    static constexpr XMFLOAT3 kDefaultCameraOffset{ 0.0f, 25.0f, -15.0f };

private:
    // ----------------------------------------------------------------------
    // Transform / 行列
    // ----------------------------------------------------------------------
    Transform m_Transform;     // NOTE: GameObject にも Transform がある場合、二重管理になる

    XMMATRIX  m_Projection{};
    XMMATRIX  m_View{};

    // ----------------------------------------------------------------------
    // キャッシュ（位置/注視点/オフセット）
    // ----------------------------------------------------------------------
    XMFLOAT3  m_Position{ 0.0f, 0.0f, 0.0f };
    XMFLOAT3  m_Target  { 0.0f, 0.0f, 0.0f };
    XMFLOAT3  m_CameraOffset = kDefaultCameraOffset;

    // ----------------------------------------------------------------------
    // 姿勢/距離
    // ----------------------------------------------------------------------
    float     m_Yaw      = kDefaultYaw;
    float     m_Pitch    = kDefaultPitch;
    float     m_Distance = kDefaultDistance;

    // ----------------------------------------------------------------------
    // デバッグ操作状態
    // ----------------------------------------------------------------------
    bool      m_FirstMouse = true;
    POINT     m_CenterPos{};
    float     m_MouseSensitivity = 0.002f;

    // ----------------------------------------------------------------------
    // デバッグ自由カメラ
    // ----------------------------------------------------------------------
    bool      m_DebugCameraMode  = false;
    float     m_DebugCameraSpeed = 0.15f;
};