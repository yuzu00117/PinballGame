//------------------------------------------------------------------------------
// FieldLayout
//------------------------------------------------------------------------------
// フィールド構築に必要な「レイアウト定義（入力データ）」をまとめたヘッダ。
// - FieldBuilder がこの定義を解釈して、Field 配下に GameObject を生成する
// - 本ファイルは配置・接続に必要なデータのみを保持し、生成ロジックは持たない
//
// 設計意図：
// - 生成処理（FieldBuilder）と、配置定義（FieldLayout）を分離する
// - レベル差し替え・テスト・将来的な外部データ化（JSON等）を容易にする
//
// 注意：
// - id（Hole など）は参照解決に使うため一意であること
// - 座標系・単位は（プロジェクト標準の）ワールド座標/単位系に従う
//------------------------------------------------------------------------------
#pragma once

#include <string>
#include <vector>

#include "Flipper.h"

#include "Vector3.h"

/// Flipper の配置定義
/// - side により左右を指定する
/// - position はワールド座標
struct FlipperDesc
{
    Flipper::Side side; // 左右の別
    Vector3 position;   // 配置位置
};

/// Bumper の配置定義
struct BumperDesc
{
    Vector3 position; // 配置位置
};

/// Hole の配置定義
/// - id は Spawner から参照される一意な識別子
struct HoleDesc
{
    std::string id; // Hole 識別子
    Vector3 position; // 配置位置
    Vector3 scale;    // スケール
};

/// EnemySpawner の配置・接続定義
/// - targetHoleIds により、スポーン対象の Hole を指定する
struct SpawnerDesc
{
    Vector3 position;                    // 配置位置
    float spawnXMin = 0.0f;              // スポーン範囲（X 最小）
    float spawnXMax = 0.0f;              // スポーン範囲（X 最大）
    float spawnZ = 0.0f;                 // スポーン Z 座標
    std::vector<std::string> targetHoleIds; // 対象 Hole の ID 一覧
};

/// フィールド構成を定義するデータ構造
/// - 実体の生成は行わず、配置・接続情報のみを保持する
/// - FieldBuilder によって解釈・構築される
struct FieldLayout
{
    std::vector<FlipperDesc> flippers; // Flipper 定義一覧
    std::vector<BumperDesc> bumpers;   // Bumper 定義一覧
    std::vector<HoleDesc> holes;       // Hole 定義一覧
    std::vector<SpawnerDesc> spawners; // EnemySpawner 定義一覧
};