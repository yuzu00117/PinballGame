#include "main.h"
#include "renderer.h"
#include "field.h"

// コンポーネント関連
#include "MeshRenderer.h"
#include "ColliderGroup.h"
#include "BoxCollider.h"

// 初期化処理
void Field::Init()
{
    // --- メッシュレンダラーの追加と設定 ---
    m_MeshRenderer = AddComponent<MeshRenderer>();
    m_MeshRenderer->LoadShader(VertexShaderPath, PixelShaderPath);
    // m_MeshRenderer->SetTexture(TexturePath);
    m_MeshRenderer->CreateUnitPlane();

    // --- スケール調整 ---
    m_MeshRenderer->SetLocalScale(HalfWidth * 2.0f, 1.0f, HalfHeight * 2.0f);

    // --- 当たり判定の追加 ---
    // 外周4辺の壁をBoxColliderで設定
    m_ColliderGroup = AddComponent<ColliderGroup>();

    // 下
    {
        auto* BoxCollier = m_ColliderGroup->AddCollider<BoxCollider>();
        BoxCollier->m_ColliderType = ColliderType::Box;
        BoxCollier->Size = { HalfWidth * 2.0f, WallHeight, WallThick };
        BoxCollier->Center = { 0.0f, WallHeight * 0.5f, -HalfHeight + WallThick * 0.5f };
    }
    // 上
    {
        auto* BoxCollier = m_ColliderGroup->AddCollider<BoxCollider>();
        BoxCollier->m_ColliderType = ColliderType::Box;
        BoxCollier->Size = { HalfWidth * 2.0f, WallHeight, WallThick };
        BoxCollier->Center = { 0.0f, WallHeight * 0.5f, HalfHeight - WallThick * 0.5f };
    }
    // 左
    {
        auto* BoxCollier = m_ColliderGroup->AddCollider<BoxCollider>();
        BoxCollier->m_ColliderType = ColliderType::Box;
        BoxCollier->Size = { WallThick, WallHeight, HalfHeight * 2.0f };
        BoxCollier->Center = { -HalfWidth + WallThick * 0.5f, WallHeight * 0.5f, 0.0f };
    }
    // 右
    {
        auto* BoxCollier = m_ColliderGroup->AddCollider<BoxCollider>();
        BoxCollier->m_ColliderType = ColliderType::Box;
        BoxCollier->Size = { WallThick, WallHeight, HalfHeight * 2.0f };
        BoxCollier->Center = { HalfWidth - WallThick * 0.5f, WallHeight * 0.5f, 0.0f };
    }
}

void Field::Uninit()
{
    m_MeshRenderer = nullptr;
    m_ColliderGroup = nullptr;
}

void Field::Update()
{
    // 今は特に無し
}

void Field::Draw()
{
    // MeshRenderer側で描画処理を行う
    m_MeshRenderer->Draw();
}