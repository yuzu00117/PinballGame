//------------------------------------------------------------------------------
// FieldBuilder
//------------------------------------------------------------------------------
// FieldLayout（レイアウト定義）を解釈し、
// Field 配下にゲーム内オブジェクトを構築するビルダークラス。
//
// 役割：
// - レイアウト定義（FieldLayout）から GameObject を生成する
// - オブジェクト間の参照関係を解決する
// - 生成後に Init を明示的に呼び出す
//
// 設計方針：
// - 配置データ（FieldLayout）と生成処理を分離する
// - Scene / Game 側に生成ロジックを持ち込まない
// - Build 完了時点で「プレイ可能な状態」を保証する
//
// 注意：
// - 生成されたオブジェクトの所有権は Field（GameObject 階層）側が持つ
// - FieldBuilder 自身は状態を保持しない（使い捨て前提）
//------------------------------------------------------------------------------
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "FieldLayout.h"

// 前方宣言
class Field;
class Hole;
class EnemySpawner;
class Flipper;
class Bumper;

/// 生成されたフィールド内オブジェクトの非所有参照一覧
struct LevelObjects
{
    std::unordered_map<std::string, Hole*> holesById;
    std::vector<EnemySpawner*> spawners;
    std::vector<Flipper*> flippers;
    std::vector<Bumper*> bumpers;
};

/// フィールドレイアウト定義から、ゲーム内オブジェクトを構築するビルダークラス
/// - Field をルートとして、各 GameObject（Hole / Flipper / Bumper / EnemySpawner）を生成する
/// - 生成順は FieldLayout に記述された配列順に従う
/// - 生成後にオブジェクト間の参照関係を接続し、最後に Init を明示的に呼び出す
/// 注意：
/// - 生成されたオブジェクトの所有権は Field（GameObject 階層）側が持つ
/// - Build の戻り値は、参照管理・後処理用の非所有ポインタ一覧である
class FieldBuilder
{
public:
    // ----------------------------------------------------------------------
    // フィールド構築
    // ----------------------------------------------------------------------
    /// レイアウト定義からフィールド内オブジェクトを生成する
    /// - Field を親として子 GameObject を生成する
    /// - 生成順：Hole → Flipper → Bumper → EnemySpawner
    /// - 生成後に参照関係を接続し、すべてのオブジェクトに Init を呼び出す
    /// 戻り値：
    /// - 生成された各オブジェクトへの非所有参照をまとめた構造体
    LevelObjects Build(Field& field, const FieldLayout& layout);

private:
    // ----------------------------------------------------------------------
    // 個別オブジェクト生成
    // ----------------------------------------------------------------------
    /// Hole を生成して登録する
    /// - Transform（Position / Scale）を設定する
    /// - Hole ID をキーとして LevelObjects::holesById に登録する
    /// 注意：
    /// - 同一 ID が既に存在する場合はアサートで停止する
    Hole* CreateHole(Field& field, const HoleDesc& desc, LevelObjects& out);

    /// Flipper を生成して登録する
    /// - side（左右）を指定して生成する
    /// - Transform::Position を設定する
    Flipper* CreateFlipper(Field& field, const FlipperDesc& desc, LevelObjects& out);

    /// Bumper を生成して登録する
    /// - Transform::Position を設定する
    Bumper* CreateBumper(Field& field, const BumperDesc& desc, LevelObjects& out);

    /// EnemySpawner を生成して登録する
    /// - Transform::Position を設定する
    /// - スポーン範囲（XMin / XMax / Z）を設定する
    EnemySpawner* CreateSpawner(Field& field, const SpawnerDesc& desc, LevelObjects& out);

    // ----------------------------------------------------------------------
    // 生成後処理
    // ----------------------------------------------------------------------
    /// 生成済みオブジェクト間の参照関係を接続する
    /// - EnemySpawner に TargetHole を関連付ける
    /// 注意：
    /// - レイアウトと生成結果の配列順が一致していることを前提とする
    void WireUp(LevelObjects& out, const FieldLayout& layout);

    /// 生成済みオブジェクトの Init を明示的に呼び出す
    /// - GameObject::Init は自動では呼ばれないため、ここで一括初期化する
    void InitAll(LevelObjects& out);
};
