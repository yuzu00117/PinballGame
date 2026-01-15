/**
 * @file FieldLayout.h
 * @brief フィールドレイアウト定義ヘッダ
 * @author yuzu00117
 * @date 2026/01/15
 */
#pragma once

#include <string>
#include <vector>

#include "Flipper.h"
#include "Vector3.h"

struct FlipperDesc
{
    Flipper::Side side;
    Vector3 position;
};

struct BumperDesc
{
    Vector3 position;
};

struct HoleDesc
{
    std::string id;
    Vector3 position;
    Vector3 scale;
};

struct SpawnerDesc
{
    Vector3 position;
    float spawnXMin = 0.0f;
    float spawnXMax = 0.0f;
    float spawnZ = 0.0f;
    std::vector<std::string> targetHoleIds;
};

struct FieldLayout
{
    std::vector<FlipperDesc> flippers;
    std::vector<BumperDesc> bumpers;
    std::vector<HoleDesc> holes;
    std::vector<SpawnerDesc> spawners;
};

