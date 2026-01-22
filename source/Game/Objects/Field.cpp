#include "main.h"
#include "renderer.h"
#include "field.h"

// コンポーネント関連
#include "MeshRenderer.h"
#include "ColliderGroup.h"
#include "BoxCollider.h"

// フィールドオブジェクト
#include "Flipper.h"
#include "Bumper.h"
#include "Hole.h"
#include "EnemySpawner.h"

//------------------------------------------------------------------------------
// Field
//------------------------------------------------------------------------------
// - 床/壁などの環境を生成し、FieldBuilder によりフィールド内オブジェクトを構築する
// - 子オブジェクトの所有権は GameObject 階層（親が子を所有）に従う

// ----------------------------------------------------------------------
// ステージ01の配置定義
// ----------------------------------------------------------------------
// - FieldBuilder に渡すための初期配置をまとめる
// - 位置やサイズはフィールド寸法から算出する
FieldLayout Field::MakeStage01Layout()
{
    FieldLayout layout;

    const float flipperZ = -kHalfHeight + 3.0f;
    const float flipperY = 0.5f;
    const float flipperX = kHalfWidth - 2.5f;

    layout.flippers.push_back({ Flipper::Side::Left, { -flipperX, flipperY, flipperZ } });
    layout.flippers.push_back({ Flipper::Side::Right, { flipperX, flipperY, flipperZ } });

    const float bumperY = 0.5f;
    const float bumperZ = 7.5f;
    const float bumperOffsetX = 4.0f;
    const float bumperLowerZ = -3.0f;
    const float bumperLowerOffsetX = 9.0f;

    layout.bumpers.push_back({ { -bumperOffsetX, bumperY, bumperZ } });
    layout.bumpers.push_back({ { bumperOffsetX, bumperY, bumperZ } });
    layout.bumpers.push_back({ { -bumperLowerOffsetX, bumperY, bumperLowerZ } });
    layout.bumpers.push_back({ { bumperLowerOffsetX, bumperY, bumperLowerZ } });

    const float holeY = 1.0f;
    const float holeZ = -kHalfHeight - 0.2f;

    layout.holes.push_back({ "main", { 0.0f, holeY, holeZ }, { 3.0f, 1.5f, 1.0f } });

    SpawnerDesc spawner;
    spawner.position = { 0.0f, 0.5f, 0.0f };
    spawner.spawnZ = kHalfHeight - 2.0f;
    spawner.spawnXMin = -kHalfWidth + 1.0f;
    spawner.spawnXMax = kHalfWidth - 1.0f;
    spawner.targetHoleIds = { "main" };

    layout.spawners.push_back(spawner);

    return layout;
}

// ----------------------------------------------------------------------
// 初期化
// ----------------------------------------------------------------------
// - 床/壁などの環境を生成する
// - FieldLayout を作成し、FieldBuilder により子オブジェクトを生成する
// 注意：FieldBuilder::Build は生成後に各オブジェクトの Init を呼び出す
void Field::Init()
{
    // ----------------------------------------------------------------------
    // 環境（床/壁/ガイド）の生成
    // ----------------------------------------------------------------------

    // ----------------------------------------------------------------------
    // 床の作成
    // ----------------------------------------------------------------------
    // 床メッシュの作成
    m_Floor = AddComponent<MeshRenderer>();
    m_Floor->LoadShader(kVertexShaderPath, kPixelShaderPath);
    m_Floor->SetTexture(kFieldTexturePath);
    m_Floor->CreateUnitPlane();
    m_Floor->SetLocalScale(kHalfWidth * 2.0f, 1.0f, kHalfHeight * 2.0f);

    // 床コライダーの作成
    m_ColliderGroup = AddComponent<ColliderGroup>();
    {
        auto floorCollider = m_ColliderGroup->AddCollider<BoxCollider>();

        // 位置を微調整して床の上面に合わせる
        floorCollider->Center = { 0.0f, -0.25f, 0.0f };
        floorCollider->Size = { kHalfWidth * 2.0f, 0.5f, kHalfHeight * 2.0f };
    }

    // ----------------------------------------------------------------------
    // 壁の作成
    // ----------------------------------------------------------------------
    const float yCenter = kWallHeight * 0.5f;

    // 1枚の壁を生成する簡易ヘルパー
    auto MakeWall = [&](const Vector3& position, const Vector3& scale)
    {
        // 子オブジェクトとして壁を作成
        GameObject* wallObj = CreateChild();
        wallObj->m_Transform.Position = position;
        wallObj->m_Transform.Scale = scale;

        // 見た目の設定
        auto wallMesh = wallObj->AddComponent<MeshRenderer>();
        wallMesh->LoadShader(kVertexShaderPath, kPixelShaderPath);
        wallMesh->SetTexture(kWallTexturePath);
        wallMesh->CreateUnitBox();
        wallMesh->m_Color = XMFLOAT4(0.8f, 0.8f, 0.85f, 1.0f);

        // 当たり判定（Center/Size は Transform から算出される想定）
        auto wallColliderGroup = wallObj->AddComponent<ColliderGroup>();
        wallColliderGroup->AddCollider<BoxCollider>();
    };

    // 壁の作成
    MakeWall({ 0.0f, yCenter,  kHalfHeight + kWallThick * 0.5f },
             { kHalfWidth * 2.0f + kWallThick * 2.0f, kWallHeight, kWallThick }); // 奥
    MakeWall({ 0.0f, yCenter, -kHalfHeight - kWallThick * 0.5f },
             { kHalfWidth * 2.0f + kWallThick * 2.0f, kWallHeight, kWallThick }); // 手前
    MakeWall({ -kHalfWidth - kWallThick * 0.5f, yCenter, 0.0f },
             { kWallThick, kWallHeight, kHalfHeight * 2.0f + kWallThick * 2.0f }); // 左
    MakeWall({ kHalfWidth + kWallThick * 0.5f, yCenter, 0.0f },
             { kWallThick, kWallHeight, kHalfHeight * 2.0f + kWallThick * 2.0f }); // 右

    // ----------------------------------------------------------------------
    // 斜めガイド（左右インレーンガイド）
    // TODO: 本来は、左右下の空き部分をなくすために、三角形のメッシュを描画し、
    //       当たり判定は Box で丸めるなど修正を行う
    // ----------------------------------------------------------------------

    // 1本のガイドを生成する簡易ヘルパー
    auto MakeGuide = [&](const Vector3& position, float rotYDeg)
    {
        GameObject* guideObj = CreateChild();
        guideObj->m_Transform.Position = position;
        guideObj->m_Transform.Scale = { 1.0f, kWallHeight, 4.0f };   // 細長いガイド
        guideObj->m_Transform.Rotation.y = rotYDeg;

        // 見た目
        auto guideMesh = guideObj->AddComponent<MeshRenderer>();
        guideMesh->LoadShader(kVertexShaderPath, kPixelShaderPath);
        guideMesh->SetTexture(kWallTexturePath);
        guideMesh->CreateUnitBox();
        guideMesh->m_Color = XMFLOAT4(0.85f, 0.85f, 0.9f, 1.0f);  // 壁より少し明るめ

        // 当たり判定（Transform から自動反映）
        auto colGroup = guideObj->AddComponent<ColliderGroup>();
        colGroup->AddCollider<BoxCollider>();
    };

    // ガイドの位置
    const float guideZ = -kHalfHeight + 3.9f;
    const float guideY = kWallHeight * 0.5f;
    const float guideX = kHalfWidth - 1.5f;     // 外壁との隙間をなくすため少し内側

    // 左ガイド（内側へ向ける）
    MakeGuide({ -guideX, guideY, guideZ }, 120.0f);

    // 右ガイド（内側へ向ける）
    MakeGuide({ +guideX, guideY, guideZ }, -120.0f);

    // ----------------------------------------------------------------------
    // フィールド内オブジェクトの作成
    // ----------------------------------------------------------------------
    // - レイアウト定義に従って子オブジェクトを生成する
    // - Build 内で参照接続と Init を行う（Build 後に即プレイ可能な状態を想定）
    FieldLayout layout = MakeStage01Layout();
    FieldBuilder builder;
    m_Level = builder.Build(*this, layout);
}

void Field::Uninit()
{
    // ----------------------------------------------------------------------
    // 終了処理
    // ----------------------------------------------------------------------
    // コンポーネントの参照をクリア
    m_Floor = nullptr;
    m_ColliderGroup = nullptr;
}

void Field::Update(float deltaTime)
{
    // ----------------------------------------------------------------------
    // 更新処理
    // ----------------------------------------------------------------------
    // 現状は基底クラスの処理のみ（フィールド固有は必要に応じて追加）
    GameObject::Update(deltaTime);
}

void Field::Draw()
{
    // ----------------------------------------------------------------------
    // 描画処理
    // ----------------------------------------------------------------------
    // 現状は基底クラスの処理のみ（フィールド固有は必要に応じて追加）
    GameObject::Draw();
}
