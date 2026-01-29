#include "field.h"

// システムｋ
#include "main.h"
#include "renderer.h"

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

    constexpr float kFlipperZOffset = 3.0f;
    constexpr float kFlipperY = 0.5f;
    constexpr float kFlipperXInset = 2.5f;

    constexpr float kBumperY = 0.5f;
    constexpr float kBumperUpperZ = 7.5f;
    constexpr float kBumperUpperXOffset = 4.0f;
    constexpr float kBumperLowerZ = -3.0f;
    constexpr float kBumperLowerXOffset = 9.0f;

    constexpr float kHoleY = 1.0f;
    constexpr float kHoleZOffset = 0.2f;
    constexpr float kHoleSizeX = 3.0f;
    constexpr float kHoleSizeY = 1.5f;
    constexpr float kHoleSizeZ = 1.0f;
    constexpr const char* kMainHoleId = "main";

    constexpr float kSpawnerY = 0.5f;
    constexpr float kSpawnerZInset = 2.0f;
    constexpr float kSpawnerXInset = 1.0f;

    const float flipperZ = -kHalfHeight + kFlipperZOffset;
    const float flipperX = kHalfWidth - kFlipperXInset;

    layout.flippers.push_back({ Flipper::Side::Left, { -flipperX, kFlipperY, flipperZ } });
    layout.flippers.push_back({ Flipper::Side::Right, { flipperX, kFlipperY, flipperZ } });

    layout.bumpers.push_back({ { -kBumperUpperXOffset, kBumperY, kBumperUpperZ } });
    layout.bumpers.push_back({ { kBumperUpperXOffset, kBumperY, kBumperUpperZ } });
    layout.bumpers.push_back({ { -kBumperLowerXOffset, kBumperY, kBumperLowerZ } });
    layout.bumpers.push_back({ { kBumperLowerXOffset, kBumperY, kBumperLowerZ } });

    const float holeZ = -kHalfHeight - kHoleZOffset;
    layout.holes.push_back({ kMainHoleId, { 0.0f, kHoleY, holeZ }, { kHoleSizeX, kHoleSizeY, kHoleSizeZ } });

    SpawnerDesc spawner;
    spawner.position = { 0.0f, kSpawnerY, 0.0f };
    spawner.spawnZ = kHalfHeight - kSpawnerZInset;
    spawner.spawnXMin = -kHalfWidth + kSpawnerXInset;
    spawner.spawnXMax = kHalfWidth - kSpawnerXInset;
    spawner.targetHoleIds = { kMainHoleId };

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
    constexpr float kFloorScaleY = 1.0f;
    constexpr float kFloorColliderThickness = 0.5f;
    constexpr float kFloorColliderCenterY = -kFloorColliderThickness * 0.5f;

    // 床メッシュの作成
    m_Floor = AddComponent<MeshRenderer>();
    m_Floor->LoadShader(kVertexShaderPath, kPixelShaderPath);
    m_Floor->SetTexture(kFieldTexturePath);
    m_Floor->CreateUnitPlane();
    m_Floor->SetLocalScale(kHalfWidth * 2.0f, kFloorScaleY, kHalfHeight * 2.0f);

    // 床コライダーの作成
    m_ColliderGroup = AddComponent<ColliderGroup>();
    {
        auto floorCollider = m_ColliderGroup->AddCollider<BoxCollider>();

        // 位置を微調整して床の上面に合わせる
        floorCollider->Center = { 0.0f, kFloorColliderCenterY, 0.0f };
        floorCollider->Size = { kHalfWidth * 2.0f, kFloorColliderThickness, kHalfHeight * 2.0f };
    }

    // ----------------------------------------------------------------------
    // 壁の作成
    // ----------------------------------------------------------------------
    const float yCenter = kWallHeight * 0.5f;
    const XMFLOAT4 kWallColor = XMFLOAT4(0.8f, 0.8f, 0.85f, 1.0f);

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
        wallMesh->m_Color = kWallColor;

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
    constexpr float kGuideWidth = 1.0f;
    constexpr float kGuideLength = 4.0f;
    constexpr float kGuideRotationDeg = 120.0f;
    constexpr float kGuideZOffset = 3.9f;
    constexpr float kGuideXInset = 1.5f;
    const XMFLOAT4 kGuideColor = XMFLOAT4(0.85f, 0.85f, 0.9f, 1.0f);

    auto MakeGuide = [&](const Vector3& position, float rotYDeg)
    {
        GameObject* guideObj = CreateChild();
        guideObj->m_Transform.Position = position;
        guideObj->m_Transform.Scale = { kGuideWidth, kWallHeight, kGuideLength };   // 細長いガイド
        guideObj->m_Transform.Rotation.y = rotYDeg;

        // 見た目
        auto guideMesh = guideObj->AddComponent<MeshRenderer>();
        guideMesh->LoadShader(kVertexShaderPath, kPixelShaderPath);
        guideMesh->SetTexture(kWallTexturePath);
        guideMesh->CreateUnitBox();
        guideMesh->m_Color = kGuideColor;  // 壁より少し明るめ

        // 当たり判定（Transform から自動反映）
        auto colGroup = guideObj->AddComponent<ColliderGroup>();
        colGroup->AddCollider<BoxCollider>();
    };

    // ガイドの位置
    const float guideZ = -kHalfHeight + kGuideZOffset;
    const float guideY = yCenter;
    const float guideX = kHalfWidth - kGuideXInset;     // 外壁との隙間をなくすため少し内側

    // 左ガイド（内側へ向ける）
    MakeGuide({ -guideX, guideY, guideZ }, kGuideRotationDeg);

    // 右ガイド（内側へ向ける）
    MakeGuide({ +guideX, guideY, guideZ }, -kGuideRotationDeg);

    // ガイドの下に、更にコライダーのみを追加（ボールが潜り抜けないようにするため）
    constexpr float kGuideColliderLength = 10.0f;
    constexpr float kGuideColliderZOffset = 2.5f;
    constexpr float kGuideColliderXInset = 3.5f;

    auto MakeGuideCollider = [&](const Vector3& position, float rotYDeg)
    {
        GameObject* guideObj = CreateChild();
        guideObj->m_Transform.Position = position;
        guideObj->m_Transform.Scale = { kGuideWidth, kWallHeight, kGuideColliderLength };   // 細長いガイド
        guideObj->m_Transform.Rotation.y = rotYDeg;

        // 当たり判定（Transform から自動反映）
        auto colGroup = guideObj->AddComponent<ColliderGroup>();
        colGroup->AddCollider<BoxCollider>();
    };

    // ガイドの位置
    const float guideColliderX = kHalfWidth - kGuideColliderXInset;     // 外壁との隙間をなくすため少し内側
    const float guideColliderY = yCenter;
    const float guideColliderZ = -kHalfHeight + kGuideColliderZOffset;

    // 左ガイド（内側へ向ける）
    MakeGuideCollider({ -guideColliderX, guideColliderY, guideColliderZ }, kGuideRotationDeg);
    // 右ガイド（内側へ向ける）
    MakeGuideCollider({ +guideColliderX, guideColliderY, guideColliderZ }, -kGuideRotationDeg);

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
