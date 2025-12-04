#include "main.h"
#include "renderer.h"
#include "field.h"

// コンポーネント関連
#include "MeshRenderer.h"
#include "ColliderGroup.h"
#include "BoxCollider.h"

// フィールドのオブジェクト
#include "Flipper.h"
#include "Hole.h"
#include "EnemyBase.h"

// 初期化処理
void Field::Init()
{
    // ----------------------------------------------------------------------
    // 床の作成
    // ----------------------------------------------------------------------
    // 床メッシュの作成
    m_Floor = AddComponent<MeshRenderer>();
    m_Floor->LoadShader(VertexShaderPath, PixelShaderPath);
    m_Floor->SetTexture(TexturePath);
    m_Floor->CreateUnitPlane();
    m_Floor->SetLocalScale(HalfWidth * 2.0f, 1.0f, HalfHeight * 2.0f);

    // 床コライダーの作成
    m_ColliderGroup = AddComponent<ColliderGroup>();
    {
        auto floorCollider = m_ColliderGroup->AddCollider<BoxCollider>();

        // 位置を微調整して床の上面に合わせる
        floorCollider->Center = { 0.0f, -0.25f, 0.0f };
        floorCollider->Size = { HalfWidth * 2.0f, 0.5f, HalfHeight * 2.0f };
    }

    // ----------------------------------------------------------------------
    // 壁の作成
    // ----------------------------------------------------------------------
    const float yCenter = WallHeight * 0.5f;

    auto MakeWall = [&](const Vector3& position, const Vector3& scale)
    {
        // 子オブジェクトとして壁を作成
        GameObject* wallObj = CreateChild();
        wallObj->m_Transform.Position = position;
        wallObj->m_Transform.Scale = scale;

        // 見た目の設定
        auto wallMesh = wallObj->AddComponent<MeshRenderer>();
        wallMesh->LoadShader(VertexShaderPath, PixelShaderPath);    // シェーダーの設定
        wallMesh->CreateUnitBox();                                  // メッシュの作成
        wallMesh->m_Color = XMFLOAT4(0.8f, 0.8f, 0.85f, 1.0f);      // 色の設定
        // wallMesh->SetTexture(TexturePath);                       // テクスチャの設定

        // 当たり判定の設定
        // CenterとSizeはTransformから自動計算されるので設定不要
        auto wallColliderGroup = wallObj->AddComponent<ColliderGroup>();
        auto boxCollider = wallColliderGroup->AddCollider<BoxCollider>();
    };

    // 壁の作成
    MakeWall({ 0.0f, yCenter,  HalfHeight + WallThick * 0.5f }, 
             { HalfWidth * 2.0f + WallThick * 2.0f, WallHeight, WallThick }); // 前
    MakeWall({ 0.0f, yCenter, -HalfHeight - WallThick * 0.5f }, 
             { HalfWidth * 2.0f + WallThick * 2.0f, WallHeight, WallThick }); // 後
    MakeWall({ -HalfWidth - WallThick * 0.5f, yCenter, 0.0f }, 
             { WallThick, WallHeight, HalfHeight * 2.0f + WallThick * 2.0f }); // 左
    MakeWall({ HalfWidth + WallThick * 0.5f, yCenter, 0.0f }, 
             { WallThick, WallHeight, HalfHeight * 2.0f + WallThick * 2.0f }); // 右

    // ----------------------------------------------------------------------
    // 斜めガイド（左右インレーンガイド）
    // TODO: 将来は、左右下の空いている部分をなくすために、三角形のモデルを描画し、
    //       当たり判定はBoxでやるなど修正を行う
    // ----------------------------------------------------------------------
    auto MakeGuide = [&](const Vector3& position, float rotY)
    {
        GameObject* guideObj = CreateChild();
        guideObj->m_Transform.Position = position;
        guideObj->m_Transform.Scale = { 1.0f, WallHeight, 4.0f };   // 細長いガイド
        guideObj->m_Transform.Rotation.y = rotY;

        // 見た目
        auto mesh = guideObj->AddComponent<MeshRenderer>();
        mesh->LoadShader(VertexShaderPath, PixelShaderPath);
        mesh->CreateUnitBox();
        mesh->m_Color = XMFLOAT4(0.85f, 0.85f, 0.9f, 1.0f);  // 壁より少し明るめ

        // 当たり判定
        auto colGroup = guideObj->AddComponent<ColliderGroup>();
        colGroup->AddCollider<BoxCollider>();  // Transform から自動反映
    };

    // ガイドの位置
    const float guideZ = -HalfHeight + 3.9f;
    const float guideY = WallHeight * 0.5f;
    const float guideX = HalfWidth - 1.5f;     // 外壁との隙間をなくすため少し内側

    // 左ガイド（内側へ  +30°）
    MakeGuide({ -guideX, guideY, guideZ }, 120.0f);

    // 右ガイド（内側へ  -30°）
    MakeGuide({ +guideX, guideY, guideZ }, -120.0f);


    // ----------------------------------------------------------------------
    // フリッパーの作成
    // ----------------------------------------------------------------------
    const float flipperZ = -HalfHeight + 3.0f;  // 奥行き位置
    const float flipperY = 0.5f;                // 高さ位置
    const float flipperX = HalfWidth - 2.5f;    // 左右位置

    // 左フリッパー
    {
        auto leftFlipper = CreateChild<Flipper>(Flipper::Side::Left);
        leftFlipper->m_Transform.Position = { -flipperX, flipperY, flipperZ };
        leftFlipper->Init();
    }
    // 右フリッパー
    {
        auto rightFlipper = CreateChild<Flipper>(Flipper::Side::Right);
        rightFlipper->m_Transform.Position = { flipperX, flipperY, flipperZ };
        rightFlipper->Init();
    }    

    // ----------------------------------------------------------------------
    // ホール（穴）の作成
    // ----------------------------------------------------------------------
    const float holeY = 1.0f;               // 高さ位置
    const float holeZ = -HalfHeight - 0.2f; // 手前側に配置
    {
        auto hole = CreateChild<Hole>();
        hole->m_Transform.Position = {0.0f, holeY, holeZ};
        hole->m_Transform.Scale = {3.0f, 1.5f, 1.0f}; // 少し大きめに
        hole->Init();

        // テストでエネミーを生成
        auto enemy = CreateChild<EnemyBase>();

        // ステージ上部あたりから出現させる（必要に応じて調整）
        const float enemyY = 1.0f;
        const float enemyZ = HalfHeight + 5.0f; // 上側
        enemy->m_Transform.Position = {0.0f, enemyY, enemyZ};

        enemy->Init();

        // EnemyBase に用意したターゲット設定関数でホール位置をセット
        enemy->SetTargetPosition(hole->m_Transform.Position);
    }
}

void Field::Uninit()
{
    m_Floor = nullptr;
    m_ColliderGroup = nullptr;
}

void Field::Update()
{
    // フィールド固有の更新処理
    GameObject::Update();
}

void Field::Draw()
{
    // フィールド固有の描画処理
    GameObject::Draw();

}