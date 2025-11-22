#include "Flipper.h"
#include "Input.h"

// コンポーネント
#include "BoxCollider.h"
#include "ColliderGroup.h"
#include "MeshRenderer.h"

namespace
{
    inline float DegToRad(float deg)
    {
        return deg * 3.14159265f / 180.0f;
    }
}

Flipper::Flipper(Side side)
    : m_Side(side)
{
}

// 初期化処理
void Flipper::Init()
{
    // 左右で基準角度を決める
    if (m_Side == Side::Left)
    {
        m_DefaultAngle = -30.0f;
        m_ActiveAngle  = +30.0f;
    }
    else // Right
    {
        m_DefaultAngle = +30.0f;
        m_ActiveAngle =  -30.0f;
    }

    // 親オブジェクトは回転軸のみ
    m_Transform.Rotation.y = DegToRad(m_DefaultAngle);

    // ----------------------------------------------------------------------
    // アーム用子オブジェクトの生成
    // ----------------------------------------------------------------------
    m_ArmObject = CreateChild();
    // 左右でアームの向きを変える
    if (m_Side == Side::Left)
    {
        // 左フリッパーは+X方向
        m_ArmObject->m_Transform.Position = { m_ArmLength * 0.5f, 0.0f, 0.0f }; // 回転軸からアーム中央まで移動
    }
    else
    {
        // 右フリッパーは-X方向
        m_ArmObject->m_Transform.Position = { -m_ArmLength * 0.5f, 0.0f, 0.0f }; // 回転軸からアーム中央まで移動
    }   
    m_ArmObject->m_Transform.Scale = { m_ArmLength, m_ArmHeight, m_ArmThickness };

    // メッシュレンダラー追加
    auto meshRenderer = m_ArmObject->AddComponent<MeshRenderer>();
    meshRenderer->LoadShader(VertexShaderPath, PixelShaderPath);    // シェーダーの設定
    meshRenderer->CreateUnitBox();
    meshRenderer->m_Color = XMFLOAT4(0.9f, 0.9f, 0.95f, 1.0f);

    // ボックスコライダー追加
    auto colliderGroup = m_ArmObject->AddComponent<ColliderGroup>();
    auto boxCollider = colliderGroup->AddCollider<BoxCollider>();
    (void)boxCollider; // 現在は特に設定なし
}

void Flipper::Update()
{
    // キー入力取得
    const BYTE key     = GetActiveKey();
    const bool isPress = Input::GetKeyPress(key);

    // 目標角度（度数）
    float targetDeg = isPress ? m_ActiveAngle : m_DefaultAngle;

    // 今回は一気に切り替え（必要なら補間処理を追加）
    m_Transform.Rotation.y = DegToRad(targetDeg);

    GameObject::Update();
}

void Flipper::Draw()
{
    GameObject::Draw();
}

void Flipper::Uninit()
{
    m_ArmObject = nullptr;
}

BYTE Flipper::GetActiveKey() const
{
    if (m_Side == Side::Left)
    {
        return VK_LSHIFT;
    }
    else // Right
    {
        return VK_OEM_2; // TODO: 後ほどバーチャルキーコードを定義する
    }
}