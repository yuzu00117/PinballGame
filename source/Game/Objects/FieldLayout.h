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
    Flipper::Side side; // 左右
    Vector3 position;   // 配置位置
};

struct BumperDesc
{
    Vector3 position; // 配置位置
};

struct HoleDesc
{
    std::string id; // 識別用ID
    Vector3 position; // 配置位置
    Vector3 scale;    // サイズ
};

struct SpawnerDesc
{
    Vector3 position;                    // スポナー配置位置
    float spawnXMin = 0.0f;             // 生成範囲X最小
    float spawnXMax = 0.0f;             // 生成範囲X最大
    float spawnZ = 0.0f;                // 生成位置Z
    std::vector<std::string> targetHoleIds; // 目標HoleのID一覧
};

struct FieldLayout
{
    std::vector<FlipperDesc> flippers; // フリッパー一覧
    std::vector<BumperDesc> bumpers;   // バンパー一覧
    std::vector<HoleDesc> holes;       // ホール一覧
    std::vector<SpawnerDesc> spawners; // スポナー一覧
};


