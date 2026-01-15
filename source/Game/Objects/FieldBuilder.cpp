#include "FieldBuilder.h"

#include <cassert>

#include "Bumper.h"
#include "EnemySpawner.h"
#include "Field.h"
#include "Flipper.h"
#include "Hole.h"

// ----------------------------------------------------------------------
// レイアウト定義からフィールドを構築する
// ----------------------------------------------------------------------
// - レイアウト順に生成し参照を保持する
// - 生成後に参照接続と Init を行う
LevelObjects FieldBuilder::Build(Field& field, const FieldLayout& layout)
{
    LevelObjects out;

    for (const auto& holeDesc : layout.holes)
    {
        CreateHole(field, holeDesc, out);
    }

    for (const auto& flipperDesc : layout.flippers)
    {
        CreateFlipper(field, flipperDesc, out);
    }

    for (const auto& bumperDesc : layout.bumpers)
    {
        CreateBumper(field, bumperDesc, out);
    }

    for (const auto& spawnerDesc : layout.spawners)
    {
        CreateSpawner(field, spawnerDesc, out);
    }

    WireUp(out, layout);
    InitAll(out);
    return out;
}

// ----------------------------------------------------------------------
// Hole の生成
// ----------------------------------------------------------------------
// - Transform を配置し、id を登録する
Hole* FieldBuilder::CreateHole(Field& field, const HoleDesc& desc, LevelObjects& out)
{
    Hole* hole = field.CreateChild<Hole>();
    hole->m_Transform.Position = desc.position;
    hole->m_Transform.Scale = desc.scale;

    const auto [it, inserted] = out.holesById.emplace(desc.id, hole);
    if (!inserted)
    {
        assert(false && "Duplicate hole id.");
    }

    return hole;
}

// ----------------------------------------------------------------------
// Flipper の生成
// ----------------------------------------------------------------------
// - 生成後に配置位置を設定する
Flipper* FieldBuilder::CreateFlipper(Field& field, const FlipperDesc& desc, LevelObjects& out)
{
    Flipper* flipper = field.CreateChild<Flipper>(desc.side);
    flipper->m_Transform.Position = desc.position;
    out.flippers.push_back(flipper);
    return flipper;
}

// ----------------------------------------------------------------------
// Bumper の生成
// ----------------------------------------------------------------------
// - 生成後に配置位置を設定する
Bumper* FieldBuilder::CreateBumper(Field& field, const BumperDesc& desc, LevelObjects& out)
{
    Bumper* bumper = field.CreateChild<Bumper>();
    bumper->m_Transform.Position = desc.position;
    out.bumpers.push_back(bumper);
    return bumper;
}

// ----------------------------------------------------------------------
// EnemySpawner の生成
// ----------------------------------------------------------------------
// - 生成後にスポーン範囲を設定する
EnemySpawner* FieldBuilder::CreateSpawner(Field& field, const SpawnerDesc& desc, LevelObjects& out)
{
    EnemySpawner* spawner = field.CreateChild<EnemySpawner>();
    spawner->m_Transform.Position = desc.position;
    spawner->SetSpawnArea(desc.spawnXMin, desc.spawnXMax, desc.spawnZ);
    out.spawners.push_back(spawner);
    return spawner;
}

// ----------------------------------------------------------------------
// 参照関係の接続
// ----------------------------------------------------------------------
// - Spawner に TargetHole を関連付ける
void FieldBuilder::WireUp(LevelObjects& out, const FieldLayout& layout)
{
    assert(out.spawners.size() == layout.spawners.size());

    for (size_t i = 0; i < layout.spawners.size(); ++i)
    {
        EnemySpawner* spawner = out.spawners[i];
        const SpawnerDesc& desc = layout.spawners[i];

        for (const auto& holeId : desc.targetHoleIds)
        {
            auto it = out.holesById.find(holeId);
            if (it == out.holesById.end())
            {
                assert(false && "Unknown hole id.");
                continue;
            }

            spawner->AddTargetHole(it->second);
        }
    }
}

// ----------------------------------------------------------------------
// 生成済みオブジェクトの初期化
// ----------------------------------------------------------------------
// - GameObject::Init を明示的に呼ぶ
void FieldBuilder::InitAll(LevelObjects& out)
{
    for (const auto& pair : out.holesById)
    {
        pair.second->Init();
    }

    for (Flipper* flipper : out.flippers)
    {
        flipper->Init();
    }

    for (Bumper* bumper : out.bumpers)
    {
        bumper->Init();
    }

    for (EnemySpawner* spawner : out.spawners)
    {
        spawner->Init();
    }
}


