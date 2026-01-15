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

struct LevelObjects
{
    std::unordered_map<std::string, Hole*> holesById; // id -> Hole（非所有）
    std::vector<EnemySpawner*> spawners;             // 敵スポーナー一覧（非所有）
    std::vector<Flipper*> flippers;                 // フリッパー一覧（非所有）
    std::vector<Bumper*> bumpers;                   // バンパー一覧（非所有）
};

class FieldBuilder
{
public:
    /// <summary>
    /// レイアウト定義からフィールド内オブジェクトを生成し、参照一覧を返す
    /// </summary>
    LevelObjects Build(Field& field, const FieldLayout& layout);

private:
    /// <summary>
    /// Hole を生成して out に登録する
    /// </summary>
    Hole* CreateHole(Field& field, const HoleDesc& desc, LevelObjects& out);
    /// <summary>
    /// Flipper を生成して out に登録する
    /// </summary>
    Flipper* CreateFlipper(Field& field, const FlipperDesc& desc, LevelObjects& out);
    /// <summary>
    /// Bumper を生成して out に登録する
    /// </summary>
    Bumper* CreateBumper(Field& field, const BumperDesc& desc, LevelObjects& out);
    /// <summary>
    /// EnemySpawner を生成して out に登録する
    /// </summary>
    EnemySpawner* CreateSpawner(Field& field, const SpawnerDesc& desc, LevelObjects& out);

    /// <summary>
    /// 生成済みオブジェクト間の参照関係を接続する
    /// </summary>
    void WireUp(LevelObjects& out, const FieldLayout& layout);
    /// <summary>
    /// 生成済みオブジェクトの Init を呼び出す
    /// </summary>
    void InitAll(LevelObjects& out);
};