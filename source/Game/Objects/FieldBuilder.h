#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "FieldLayout.h"

// ëOï˚êÈåæ
class Field;
class Hole;
class EnemySpawner;
class Flipper;
class Bumper;

struct LevelObjects
{
    std::unordered_map<std::string, Hole*> holesById;
    std::vector<EnemySpawner*> spawners;
    std::vector<Flipper*> flippers;
    std::vector<Bumper*> bumpers;
};

class FieldBuilder
{
public:
    LevelObjects Build(Field& field, const FieldLayout& layout);

private:
    Hole* CreateHole(Field& field, const HoleDesc& desc, LevelObjects& out);
    Flipper* CreateFlipper(Field& field, const FlipperDesc& desc, LevelObjects& out);
    Bumper* CreateBumper(Field& field, const BumperDesc& desc, LevelObjects& out);
    EnemySpawner* CreateSpawner(Field& field, const SpawnerDesc& desc, LevelObjects& out);

    void WireUp(LevelObjects& out, const FieldLayout& layout);
    void InitAll(LevelObjects& out);
};

