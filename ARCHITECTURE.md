# アーキテクチャ設計ドキュメント

本ドキュメントは PinballGame の **設計思想（拡張性・責務分離・依存関係の整理）** を俯瞰するためのものです。
個々のクラスの詳細は各ファイル先頭のヘッダコメント（`役割 / 設計意図 / 構成 / NOTE`）を参照してください。
コメントの書式・命名・所有権表記の規約は [CodingRule.md](./CodingRule.md) に定義されています。

---

## 1. 設計目標

| 目標 | 実現方針 |
| --- | --- |
| **拡張性** | Entity-Component 合成・仮想ライフサイクル・多態・データ駆動で、追加を「派生1つ」「enum+1行」「データ追記」で済ませる |
| **責務分離** | `source/` をレイヤーで分割し、1クラス1責務に保つ。GameObject は「実体」、Component は「振る舞い」に分ける |
| **依存関係の整理** | 依存方向を Game → 基盤層 の一方向に固定。前方宣言で循環回避、所有権を型とコメントで明示 |

---

## 2. レイヤー構成

```
source/
├── Core/      エンジン基盤      … GameManager / GameObject / Component / Transform / Input / TimeSystem / SceneFader
├── Graphics/  描画             … Renderer / MeshRenderer / modelRenderer / Sprite / SpriteSheet / Camera / AnimationModel / PostProcess
├── Physics/   衝突・物理        … Collider(基底)+Box/Sphere/Group / RigidBody / ColliderUtility / CollisionInfo
├── Audio/     音               … Audio(XAudio2薄ラッパー) / SoundManager / SoundID
├── Math/      数学             … Vector3 / MathUtil / Easing
└── Game/      ゲーム固有        … Objects/ , Scene/ , HP , Score
```

**依存方向は一方向**：`Game → (Core / Graphics / Physics / Audio / Math)`。
基盤層はゲーム固有の型を知りません（基盤層を別ゲームに再利用可能な状態に保つ意図）。

### 各レイヤーの責務
- **Core** … 全オブジェクト共通のライフサイクル（Init/Update/Draw）、Component 合成、シーン遷移、入力・時間。
- **Graphics** … DirectX11 の状態管理（Renderer）と、各描画手段（メッシュ/モデル/スプライト/ポストプロセス）。
- **Physics** … 「当たり判定（Collider）」と「物理挙動（RigidBody）」を**別物として分離**。両者は `CollisionInfo` 経由で連携。
- **Audio** … XAudio2 の薄ラッパー（Audio）と、ID でアクセスする管理層（SoundManager）。
- **Math** … 依存ゼロの純粋な数値・ベクトル・補間ユーティリティ。
- **Game** … 上記基盤を組み合わせて作るゲーム固有の実体・シーン・UI。

---

## 3. 拡張性の中核機構

### 3.1 Entity-Component 合成（設計の心臓部）
`GameObject` は描画・衝突・物理を**直接持たず**、`Component` を組み合わせて多様なオブジェクトを表現します。

- `AddComponent<T>(args...)` … 型安全に Component を生成・所有・Init（[GameObject.h](source/Core/GameObject.h)）。
- `GetComponent<T>()` … 型で取得。
- Component 側は所有を持たず、`m_Owner`（非所有）経由で連携（[component.h](source/Core/component.h)）。

→ 新しい振る舞いは「Component を1つ派生して AddComponent するだけ」で追加できます。

### 3.2 仮想ライフサイクル
GameObject / Component とも `Init / Update / Draw` を virtual 化。GameManager は基底ポインタで同一ループ管理し、派生型を意識しません。

### 3.3 Collider の多態 + Composite
- `Collider`（基底）が `CheckCollision()` を純粋仮想で定義し、衝突イベントを GameObject へ中継。
- `BoxCollider` / `SphereCollider` が形状別の判定を実装（判定アルゴリズムは `ColliderUtility` や各 .cpp の static ヘルパに局所化）。
- `ColliderGroup` は Composite パターンで複数 Collider を1つの Collider として束ねる（[ColliderGroup.h](source/Physics/ColliderGroup.h)）。入れ子も可能。

→ 新形状（Capsule 等）は `Collider` を派生して `CheckCollision()` を実装するだけで追加できます。

### 3.4 Enemy 継承（Template Method）
`EnemyBase` が HP・移動・被ダメージ・撃破（OnKilled）を共通化し、`EnemyStraight` は `Init()` のみ override。
→ 新しい敵は EnemyBase を派生して差分のみ実装します。

### 3.5 データ駆動のフィールド構築
- `FieldLayout` … 配置定義データ（FlipperDesc / BumperDesc / HoleDesc / SpawnerDesc）の POD のみ（[FieldLayout.h](source/Game/Objects/FieldLayout.h)）。
- `FieldBuilder` … 定義を解釈して生成 → `WireUp()` で参照接続 → `InitAll()`（[FieldBuilder.h](source/Game/Objects/FieldBuilder.h)）。

→ レベルの差し替えや JSON 外出しが容易。生成ロジックと配置データが完全分離されています。

### 3.6 シーン抽象
[Scene.h](source/Game/Scene/Scene.h) の `CreateSceneObjects(Scene)` がシーンごとの生成を一元化。
GameManager はシーンの中身を知らず、生成された GameObject 群をループに乗せるだけ。
→ 新シーン追加はこの関数への分岐追加のみ。

### 3.7 音の ID 抽象
`SoundID`（enum class）→ `SoundManager` の固定長テーブル → `Audio`（[SoundID.h](source/Audio/SoundID.h) / [SoundManager.h](source/Audio/SoundManager.h)）。
→ 新しい音は enum に値を1つ追加し、Load を1行足すだけ。マジックナンバーを排除。

---

## 4. 依存関係と所有権の整理

### 4.1 循環回避
ヘッダ依存を最小化するため前方宣言を活用（例：[component.h](source/Core/component.h) は `class GameObject;` のみ）。

### 4.2 所有権規約（[CodingRule.md](./CodingRule.md) §5 で明文化）
| 種別 | 表現 | 例 |
| --- | --- | --- |
| **所有** | `std::unique_ptr` | `m_Components` / `m_Children`（GameObject）, `ColliderGroup.colliders` |
| **非所有参照** | 生ポインタ | `m_Owner`（Component）, `m_Parent`（GameObject）, `m_Transform`（Collider） |

ポインタには必ず「所有/非所有」「有効期間」をコメントで明記します。

### 4.3 疎結合のための通信路
- **衝突 → 物理**：`CollisionInfo`（値オブジェクト）経由で判定層から応答層（RigidBody）へ受け渡し。物理と判定が直接結合しない（[CollisionInfo.h](source/Physics/CollisionInfo.h)）。
- **Spawner ↔ Hole**：実体参照ではなく **ID 参照**。`FieldBuilder::WireUp()` が ID→Hole* を遅延解決するため、Spawner は Hole の実装を知らない。
- **敵一覧の集約**：`EnemyManager`（非所有参照のみ保持の簡易シングルトン）が窓口になり、他オブジェクトは敵を直接保持しない（[EnemyManager.h](source/Game/Objects/EnemyManager.h)）。
- **衝突イベント**：Collider → GameObject の `OnCollisionEnter/Stay/Exit`・`OnTrigger*` コールバック（Observer 的通知）。

---

## 5. 設計を支える規約
- [CodingRule.md](./CodingRule.md) … 命名規則、ファイル先頭ヘッダ（役割/設計意図/構成/NOTE）必須、cpp のセクション区切り、public API の契約コメント、所有権コメントを規定。
- [AGENTS.md](./AGENTS.md) … PR レビュー時に上記準拠を日本語でチェック。

> コメント方針：「何をしているか」ではなく **設計意図・責務・契約（所有権/ライフサイクル/副作用）** を書く。

---

## 6. 適用パターン早見表

| パターン | 実装箇所 | 効果 |
| --- | --- | --- |
| Entity-Component | GameObject / Component | 機能の合成・組み換え |
| Composite | ColliderGroup | 複数形状を1つの当たり判定に |
| Template Method | EnemyBase → EnemyStraight | 共通処理＋差分実装 |
| Builder | FieldBuilder | 複雑な生成を段階化（生成→接続→初期化） |
| データ駆動 | FieldLayout | 配置を実装から分離 |
| ID 参照（遅延解決） | Spawner ↔ Hole（WireUp） | オブジェクト間の疎結合 |
| 集約窓口（簡易 Singleton） | EnemyManager / SoundManager | 依存の一点集中 |
| Value Object | CollisionInfo | 判定層↔応答層の通信 |
