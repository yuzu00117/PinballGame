//------------------------------------------------------------------------------
// FieldLayout
//------------------------------------------------------------------------------
// 役割:
// フィールド構築に必要な「レイアウト定義（入力データ）」をまとめたヘッダ。
// - FieldBuilder がこの定義を解釈して、Field 配下に GameObject を生成する
// - 本ファイルは配置・接続に必要なデータのみを保持し、生成ロジックは持たない
//
// 設計意図:
// - 生成処理（FieldBuilder）と、配置定義（FieldLayout）を分離する
// - レベル差し替え・テスト・将来的な外部データ化（JSON 等）を容易にする
//
// 構成:
// - FlipperDesc  : Flipper の配置定義（左右/座標）
// - BumperDesc   : Bumper の配置定義（座標）
// - HoleDesc     : Hole の配置定義（ID/座標/スケール）
// - SpawnerDesc  : EnemySpawner の配置・接続定義（範囲/接続 Hole ID）
// - FieldLayout  : 上記定義の一覧を束ねる
//
// NOTE:
// - id（Hole 等）は参照解決に使うため一意であること
// - 座標系・単位は（プロジェクト標準の）ワールド座標/単位系に従う
//------------------------------------------------------------------------------
#pragma once
// 標準ライブラリ
#include <string>
#include <vector>

// システム
#include "Vector3.h"

// ゲームオブジェクト
#include "Flipper.h"

//------------------------------------------------------------------------------
// レイアウト定義（入力データ）
//------------------------------------------------------------------------------
/// Flipper の配置定義
/// - side により左右を指定する
/// - position はワールド座標
struct FlipperDesc
{
    Flipper::Side side;   // 左右の別
    Vector3       position; // 配置位置（ワールド座標）
};

/// Bumper の配置定義
/// - position はワールド座標
struct BumperDesc
{
    Vector3 position; // 配置位置（ワールド座標）
};

/// Hole の配置定義
/// - id は Spawner から参照される一意な識別子
/// - position / scale はワールド座標系（scale は実装側の解釈に依存）
struct HoleDesc
{
    std::string id;     // Hole 識別子（参照解決用。ユニークであること）
    Vector3     position; // 配置位置（ワールド座標）
    Vector3     scale;    // スケール
};

/// EnemySpawner の配置・接続定義
/// - targetHoleIds によりスポーン対象の Hole を指定する
/// - spawnXMin/spawnXMax/spawnZ はスポーン位置の決定に利用する（実装側の解釈に依存）
struct SpawnerDesc
{
    Vector3 position;                         // 配置位置（ワールド座標）
    float   spawnXMin = 0.0f;                 // スポーン範囲（X 最小）
    float   spawnXMax = 0.0f;                 // スポーン範囲（X 最大）
    float   spawnZ    = 0.0f;                 // スポーン Z 座標
    std::vector<std::string> targetHoleIds;   // 対象 Hole の ID 一覧
};

/// フィールド構成を定義するデータ構造
/// - 実体の生成は行わず、配置・接続情報のみを保持する
/// - FieldBuilder によって解釈・構築される
struct FieldLayout
{
    std::vector<FlipperDesc> flippers; // Flipper 定義一覧
    std::vector<BumperDesc>  bumpers;  // Bumper 定義一覧
    std::vector<HoleDesc>    holes;    // Hole 定義一覧
    std::vector<SpawnerDesc> spawners; // EnemySpawner 定義一覧
};
